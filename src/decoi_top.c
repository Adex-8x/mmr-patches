#include <pmdsky.h>
#include <cot.h>
#include "extern.h"
#include "decoi_top.h"

#ifdef DECOI_ENABLED

SECTION_DATA struct preprocessor_flags DBOX_PREPROCESSOR_FLAGS = {.flags_1 = 0b1110, .flags_11 = 0b10}; // 0x1018

SECTION_DATA int CURRENT_SPEAKER_ID = -1;
SECTION_DATA int CASTLE_WINDOW_ID = -1;
SECTION_DATA int KETSUBAN_WINDOW_ID = -1;
SECTION_DATA int pos = -1;
SECTION_DATA int delay = 120;
SECTION_DATA bool kill = false;
SECTION_DATA bool everything_that_i_can_say_is = false;

SECTION_DATA bool* TOP_MENU_ACTIVE = (bool*)0x0231E2A8;
SECTION_DATA uint32_t* TOP_MENU_STATUSES = (uint32_t*)0x0231E2A0;
SECTION_DATA uint32_t* TOP_MENU_UNK1 = (uint32_t*)0x0231E2B8;
SECTION_DATA uint32_t* TOP_MENU_UNK2 = (uint32_t*)0x0231E2FC;

SECTION_DATA char OLD_FONT[] = "FONT/unkno_rd.dat";
SECTION_DATA char NEW_FONT[] = "FONT/kanji_jp.dat";
SECTION_DATA char FOREBODING_HINT[] = "\n\n== FORCEFULLY PASTORALIST ==\n\n";

SECTION_CREATE void TOP_SwapFont(const char* filepath, bool swap_unkno) {
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

SECTION_CREATE void CustomCreateTopMenu(void) {
	struct window_params castle_params = { .x_offset = 0x1, .y_offset = 0x1, .width = 0xD, .height = 0xC, .box_type = {0xFA} };
	struct window_params ketsuban_params = { .x_offset = 0x10, .y_offset = 0xB, .width = 0xD, .height = 0xC, .box_type = {0xFA} };
	DebugPrint0(FOREBODING_HINT);
	CASTLE_WINDOW_ID = CreateDialogueBox(&castle_params);
	KETSUBAN_WINDOW_ID = CreateDialogueBox(&ketsuban_params);
	PlayBgmByIdVolumeVeneer(190, 90, 255);
	TOP_SwapFont(NEW_FONT, true);
}

SECTION_UPDATE int CustomUpdateTopMenu(void) {
	undefined ctx[0x180];
	struct top_dialogue* current_dialogue;
	if(delay > 0) {
		delay--;
		return 0;
	}
	if(kill) {
		MemsetSimple(FOREBODING_HINT+10, ' ', 5);
		MemsetSimple(FOREBODING_HINT+20, ' ', 4);
		FOREBODING_HINT[25] = ' ';
		DebugPrint0(FOREBODING_HINT);
		CardPullOut();
		TOP_SwapFont(OLD_FONT, true);
		// Exit the Top Menu gracefully
		TOP_MENU_ACTIVE = false;
		TOP_MENU_STATUSES[0] = 4;
		TOP_MENU_STATUSES[1] = 4;
		TOP_MENU_UNK1[1] = 0;
		TOP_MENU_UNK2[1] = 0;
		return 4;
	}
	if(!everything_that_i_can_say_is || !IsDialogueBoxActive(CURRENT_SPEAKER_ID)) {
		pos++;
		everything_that_i_can_say_is = true;
		if(pos >= ARRAY_LENGTH(TOP_DIALOGUE)-1) { // -1 is intentional!
			delay = 240;
			ChangeVolumeBgm(120, 0);
			CloseDialogueBox(CASTLE_WINDOW_ID);
			CloseDialogueBox(KETSUBAN_WINDOW_ID);
			kill = true;
			return 0;
		}
		current_dialogue = &(TOP_DIALOGUE[pos]);
		CURRENT_SPEAKER_ID = current_dialogue->is_ketsuban ? KETSUBAN_WINDOW_ID : CASTLE_WINDOW_ID;
		Crypto_RC4Init(ctx, current_dialogue->key, sizeof(current_dialogue->key));
		Crypto_RC4Encrypt(ctx, current_dialogue->string, current_dialogue->size, current_dialogue->string);
		ShowStringInDialogueBox(CURRENT_SPEAKER_ID, DBOX_PREPROCESSOR_FLAGS, (char*)current_dialogue->string, NULL);
		// just for fun in case someone is lazy and tries to memory view their way to success
		Crypto_RC4Init(ctx, current_dialogue->key, sizeof(current_dialogue->key));
		Crypto_RC4Encrypt(ctx, current_dialogue->string, current_dialogue->size, current_dialogue->string);
	}
	return 0;
}

#endif