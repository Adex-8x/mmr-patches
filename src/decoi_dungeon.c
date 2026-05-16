#include <pmdsky.h>
#include <cot.h>
#include "extern.h"
#include "decoi_dungeon.h"

#define FADE_IN 1
#define FADE_OUT 2
#define WHITE_IN 4
#define WHITE_OUT 5
#define FLASH_WAIT_TIME 0xA000

#ifdef DECOI_ENABLED

extern void DisplayMessage3(undefined4* param1, struct portrait_box* portrait, const char* message, bool flag);
extern void ResetTextColorPalette(void);
extern void ChangeDungeonColors(struct rgba* colors, int duration);
extern void FreezeAnim(struct entity* entity);
extern void UnfreezeAnim(struct entity* entity);
extern void SetEntityDirection(struct entity* entity, int direction);
extern void SetColorField(undefined field, int val);
extern void MoveEntityGraphically(struct entity* entity, int x, int y);
extern void LoadDungeonTileset(void);
extern void UpdateTileGraphics(void);
extern void ShakeScreen(int intensity);

SECTION_CHAINS uint16_t KETSUBAN_MAGIC = 581;
SECTION_CHAINS char FATAL_ERROR[] = "!!!!! Fatal !!!!!\nDecoy LocateSet 0x%x buffer\n232 size can't locate\n!!!!! see log for info !!!!!";
SECTION_CHAINS char MESSAGE_LOG[] = "[CN]キミが　きめる　こと　しか　しらないの";
SECTION_CHAINS char INTERMISSION[] = "";
SECTION_CHAINS bool should_wait = false;

SECTION_CHAINS static int OFFSETS_XY[8][2] = { {0x0, 0x200}, {0x200, 0x200}, {0x200, 0x0}, {0x200, 0xFFFFFE00}, {0x0, 0xFFFFFE00}, {0xFFFFFE00, 0xFFFFFE00}, {0xFFFFFE00, 0x0}, {0xFFFFFE00, 0x200} };

SECTION_HELPER static void WaitFrames(int n) {
	for (int i = 0; i < n; i++)
		AdvanceFrame(39);
}

SECTION_HELPER static void MoveTileOffset(struct entity* entity, int offset_tile_count) {
	if(!(EntityIsValid(entity)))
		return;
	UnfreezeAnim(entity);
	struct monster* monster = entity->info;
	int dir = monster->action.direction.val;
	if(offset_tile_count < 0) {
		offset_tile_count *= -1;
		dir = (dir + 4) & 7;
	}
	int offset_tile_x = OFFSETS_XY[dir][0];
	int offset_tile_y = OFFSETS_XY[dir][1];
	offset_tile_x = offset_tile_x >> 1;
	offset_tile_y = offset_tile_y >> 1;
	offset_tile_count = offset_tile_count << 1;
	ChangeMonsterAnimation(entity, 0, dir);
	for(int i = 0; i < offset_tile_count*12; i++) {
		MoveEntityGraphically(entity, offset_tile_x, offset_tile_y);
		AdvanceFrame(39);
	}
	ChangeMonsterAnimation(entity, 7, dir);
	FreezeAnim(entity);
}

SECTION_HELPER void DUNGEON_SwapFont(const char* filepath, bool swap_unkno) {
	struct iovec iov;
	void** base = NULL;
	void** data = NULL;
	if(swap_unkno) {
		base = &(FONT_DATA.unkno_rd_base);
		data = &(FONT_DATA.unkno_rd_data);
	}
	else {
		base = &(FONT_DATA.kanji_rd_base);
		data = &(FONT_DATA.kanji_rd_data);
	}
	MemFree(*base);
	LoadFileFromRom(&iov, filepath, 1);
	*base = iov.iov_base;
	*data = (void*)((uint32_t)(iov.iov_base) + 0x4);
}

SECTION_HELPER static void HeadShake(struct entity* entity) {
	struct monster* monster = entity->info;
	int current_direction = monster->action.direction.val;
	current_direction = (current_direction - 1) & 7;
	ChangeMonsterAnimation(entity, 7, current_direction);
	WaitFrames(4);
	current_direction = (current_direction + 2) & 7;
	ChangeMonsterAnimation(entity, 7, current_direction);
	WaitFrames(4);
	current_direction = (current_direction - 2) & 7;
	ChangeMonsterAnimation(entity, 7, current_direction);
	WaitFrames(4);
	current_direction = (current_direction + 1) & 7;
	ChangeMonsterAnimation(entity, 7, current_direction);
	WaitFrames(6);
}

SECTION_HELPER static void JumpAngry(struct entity* entity)
{
	struct monster* monster = entity->info;
	ChangeMonsterAnimation(entity, 0, monster->action.direction.val);
	for(int j = 0; j < 2; j++)
	{
		for(int i = 0; i < 6; i++)
		{
			entity->elevation += 0x160;
			if(i & 1)
			AdvanceFrame(39);
		}
		AdvanceFrame(39);
		for(int i = 0; i < 6; i++)
		{
			entity->elevation -= 0x160;
			if(i & 1)
			AdvanceFrame(39);
		}
	}
	ChangeMonsterAnimation(entity, 7, monster->action.direction.val);
}

SECTION_HELPER void TryAdvanceColorFrame(void) {
	if(should_wait)
		AdvanceFrame(39);
}

SECTION_HELPER static void Turn2Direction(struct entity* entity, int speed, bool clockwise, int start_direction, int end_direction) {
	int current_direction = start_direction;
	int iteration = 1;
	int frame_count = speed << 1;
	if(clockwise)
		iteration = -1;
	while(current_direction != end_direction) {
		current_direction = (current_direction + iteration) & 7;
		SetEntityDirection(entity, current_direction);
		WaitFrames(frame_count);
	}
}

SECTION_HELPER static void ManualShakeScreen(int duration, int pixel_intensity, int speed) {
	struct entity* camera_target = DUNGEON_PTR->display_data.camera_target;
	struct entity dummy;
	MemcpySimple(&dummy, camera_target, sizeof(struct entity));
	dummy.is_visible = false;
	DUNGEON_PTR->display_data.camera_target = &dummy;
	for(int i = 0; i < duration; i++)
	{
		int x_offset = (DungeonRandInt(pixel_intensity*2) - pixel_intensity) * 0x200;
		int y_offset = (DungeonRandInt(pixel_intensity*2) - pixel_intensity) * 0x200;
		WaitFrames(speed);
		MoveEntityGraphically(&dummy, x_offset, y_offset);
		WaitFrames(speed);
		MoveEntityGraphically(&dummy, -x_offset, -y_offset);
	}
	DUNGEON_PTR->display_data.camera_target = camera_target;
}

SECTION_HELPER void CustomRunDungeon(struct dungeon_init* dungeon_init_data, struct dungeon* dungeon) {
	struct options options;
	char* DEFAULT_FONT = (char*)0x0209ABF0;
	MemZero(dungeon_init_data, sizeof(struct dungeon_init));
	dungeon_init_data->id.val = 255;
	dungeon_init_data->floor = 1;
	MemZero((void*)(0x20AFF00), 0x28);
	DEFAULT_HERO_ID.val = 553;
	DEFAULT_PARTNER_ID.val = 0;
	MemZero(TEAM_MEMBER_TABLE_PTR->members, 0x44 << 1);
	InitMainTeamAfterQuiz();
	SetTeamSetupHeroOnly();
	LoadOverlay(OGROUP_OVERLAY_31);
	DEFAULT_FONT[12] = 'j';
	DEFAULT_FONT[13] = 'p';
	DUNGEON_SwapFont(DEFAULT_FONT, false);
	GetOptions(&options);
	options.top_screen = 2;
	SetOptions(&options);
	RunDungeon(dungeon_init_data, dungeon);
}

SECTION_DUNGEON void CustomIntro(void) {
	undefined ctx[0x180];
	undefined ketsuban_cipher[0x180];
	uint32_t hash[0x4];
	struct window_params window_params = { .x_offset = 0x2, .y_offset = 0x1, .width = 0x1C, .height = 0xA, .box_type = {0xFA} };
	struct preprocessor_flags flags = {.flags_1 = 0b1110, .flags_11 = 0b10}; // 0x1018
	struct preprocessor_flags instant_flags = {.flags_1 = 0b000000010, .timer_2 = true}; // Instant text without waiting for any input!
	ChangeDungeonMusic(0);
	struct entity* decoy = GetLeader();
	while(!EntityIsValid(decoy))
		CardPullOut();
	MoveMonsterToPos(decoy, decoy->pos.x+1, decoy->pos.y, 0);
	UpdateEntityPixelPos(decoy, NULL);
	struct monster* monster = decoy->info;
	monster->display_shadow = false;
	DUNGEON_PTR->display_data.natural_lighting = true;
	SetGameMode(3);
	SetSpecialEpisodeType(2);
	SetBothScreensWindowsColor(2);
	struct rgba* color_map = GetWeatherColorTable(GetApparentWeather(NULL));
	MemZero(color_map, 0x400);
	ChangeDungeonColors(color_map, 6);
	SetEntityDirection(decoy, DIR_UP);
	FreezeAnim(decoy);
	StartFadeDungeonWrapper(FADE_IN, 0xA000, SCREEN_MAIN);
	WaitFrames(5);
	ResetTextColorPalette();
	MD5_Init(ctx);
	int window_id = CreateDialogueBox(&window_params);
	int volume = 0;
	int duration = 0;
	int pixel_intensity = 0;
	Crypto_RC4Init(ketsuban_cipher, &KETSUBAN_MAGIC, sizeof(KETSUBAN_MAGIC));
	for(int i = 0; i < ARRAY_LENGTH(CHAINS_DIALOGUE); i++) {
		uint8_t* current_dialogue = CHAINS_DIALOGUE[i];
		int k = 0;
		do {
			Crypto_RC4Encrypt(ketsuban_cipher, current_dialogue+k, 1, current_dialogue+k);
		} while(current_dialogue[k++] != 0x0);
		MD5_Update(ctx, current_dialogue, strlen((char*)current_dialogue));
		ShowStringInDialogueBox(window_id, flags, (char*)current_dialogue, NULL);
		while(IsDialogueBoxActive(window_id)) {
			// lol
			int dungeon_event_local = LoadScriptVariableValue(NULL, VAR_DUNGEON_EVENT_LOCAL);
			switch(dungeon_event_local) {
				case 0:
					goto advance_frame;
				case 1:
				head_shake:;
					HeadShake(decoy);
					break;
				case 2:
					JumpAngry(decoy);
					break;
				case 3:
					// i'm gonna have a stroke
					ChangeMonsterAnimation(decoy, 11, monster->action.direction.val);
					UnfreezeAnim(decoy);
					goto advance_frame;
				case 4:
					ChangeMonsterAnimation(decoy, 7, monster->action.direction.val);
					goto head_shake;
				case 5:
					if(volume == 0) {
						volume = 128;
						PlaySeByIdVolume(10753, volume);
					}
					else {
						volume += 64;
						if(volume > 255)
							volume = 255;
						ChangeSeVolumeVeneer(10753, 60, volume);
					}
					duration = 1;
					pixel_intensity += 1;
					break;
				default:
					if(dungeon_event_local >= 39) {
						MoveTileOffset(decoy, dungeon_event_local-39);
					}
			}
			SaveScriptVariableValue(NULL, VAR_DUNGEON_EVENT_LOCAL, 0);
			advance_frame:;
			if(pixel_intensity > 0)
				ManualShakeScreen(duration, pixel_intensity, 1);
			else
				AdvanceFrame(39);

		}
		// Post-dialogue actions
		if(i == 0) {
			// First iteration; fade in the screen
			ShowStringInDialogueBox(window_id, instant_flags, INTERMISSION, NULL);
			for(int j = 0; j < 256; j++) {
				if(!(j & 0b1101))
					color_map[j].b = 0xA0;
				else if((j & 0b1000000 && j & 0b1))
					color_map[j].g = 24;
			}
			should_wait = true;
			PlayBgmByIdVolumeVeneer(MUSIC_OCEAN_SFX, 120, 192);
			ChangeDungeonColors(color_map, 6);
			WaitFrames(120);
		}
		else if(i == 8) {
			// Turn around slowly after "it's not his to ask"
			ShowStringInDialogueBox(window_id, instant_flags, INTERMISSION, NULL);
			WaitFrames(120);
			Turn2Direction(decoy, 10, true, DIR_UP, DIR_DOWN);
			WaitFrames(60);
		}
		else if(i == 11) {
			// stroke gaming
			ChangeMonsterAnimation(decoy, 6, monster->action.direction.val);
			FreezeAnim(decoy);
			ShowStringInDialogueBox(window_id, instant_flags, INTERMISSION, NULL);
			PlaySeByIdIfNotSilence(10761);
			StartFadeDungeonWrapper(WHITE_OUT, FLASH_WAIT_TIME, SCREEN_MAIN);
			WaitFrames(5);
			StopBgm(0);
			StartFadeDungeonWrapper(WHITE_IN, FLASH_WAIT_TIME, SCREEN_MAIN);
			DUNGEON_PTR->gen_info.tileset_id = 19;
			LoadDungeonTileset();
			UpdateTileGraphics();
			ManualShakeScreen(5, 1, 1);
			for(int j = 0; j < 20; j++)
				LogMessageQuiet(decoy, MESSAGE_LOG);
			StartFadeDungeonWrapper(WHITE_IN, FLASH_WAIT_TIME, SCREEN_SUB);
			StartFadeDungeonWrapper(WHITE_OUT, FLASH_WAIT_TIME, SCREEN_MAIN);
			WaitFrames(5);
			for(int j = 0; j < 256; j++) {
				if(!(j & 0b1101))
					color_map[j].b = 0xA8;
				else if((j & 0b1000000 && j & 0b1))
					color_map[j].g = 0x20;
				if((j & 0b1))
					color_map[j].r = 13;
			}
			should_wait = false;
			ChangeDungeonColors(color_map, 6);
			StartFadeDungeonWrapper(FADE_OUT, 0x400, SCREEN_SUB);
			StartFadeDungeonWrapper(WHITE_IN, 0x400, SCREEN_MAIN);
			PlayBgmByIdVolumeVeneer(MUSIC_THUNDERSTORM_SFX, 90, 192);
			ShakeScreen(3);
			SaveScriptVariableValue(NULL, VAR_DUNGEON_EVENT_LOCAL, 0);
			WaitFrames(60);
		}
		else if(i == 15) {
			ChangeMonsterAnimation(decoy, 6, monster->action.direction.val);
			FreezeAnim(decoy);
		}
		else if(i == 16) {
			ShowStringInDialogueBox(window_id, instant_flags, INTERMISSION, NULL);
			ManualShakeScreen(duration+30, pixel_intensity, 1);
			SoundStop();
			ManualShakeScreen(duration+10, pixel_intensity+4, 2);
			MemZero(color_map, 0x400);
			ChangeDungeonColors(color_map, 6);
		}
	}
	CloseDialogueBox(window_id);
	WaitFrames(4);
	char buffer[0x100];
	sprintf(buffer, FATAL_ERROR, decoy);
	window_params.x_offset = 1;
	window_id = CreateDialogueBox(&window_params);
	ShowStringInDialogueBox(window_id, instant_flags, buffer, NULL);
	MD5_Digest(hash, ctx);
	Crypto_RC4Init(ctx, hash, sizeof(hash));
	Crypto_RC4Encrypt(ctx, FINAL_STAGE, sizeof(FINAL_STAGE), FINAL_STAGE);
	while(true) {
		DebugPrint0((char*)FINAL_STAGE);
		WaitFrames(60);
	}
	PM_ForceToPowerOff();
}

SECTION_HELPER void CustomGenerateFloor(void) {
	DUNGEON_PTR->gen_info.tileset_id = 117;
	DUNGEON_PTR->floor_properties.layout.val = LAYOUT_CROSSROADS;
	DUNGEON_PTR->floor_properties.visibility_range = 0;
	DUNGEON_PTR->floor_properties.enemy_density = 0;
	DUNGEON_PTR->floor_properties.f_secondary_structures = true;
	DUNGEON_PTR->floor_properties.secondary_terrain_density = 255;
	DUNGEON_PTR->floor_properties.trap_density = 0;
	GenerateFloor();
}

#endif