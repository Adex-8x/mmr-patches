.nds
.include "symbols.asm"

.ifdef CustomCreateTopMenu

.open "overlay0.bin", overlay0_start
	.org 0x022be924 ; randint
	.area 0x4
		mov r0,#0
	.endarea

	.org 0x022be1a4 ; bgm
	.area 0x4
		nop
	.endarea

	.org 0x022be950 ; load bgp background
	.area 0x4
		nop
	.endarea
.close

.open "overlay1.bin", overlay1_start
	.org CreateMainMenus
	.area 0x4
		b CustomCreateTopMenu
	.endarea

	.org 0x02331784
	.area 0x4
		b CustomUpdateTopMenu
	.endarea

	.org 0x0232FC70
	.area 0x10
		nop
		nop
		nop
		nop
	.endarea

	.org 0x0232FCA4
	.area 0xC
		nop
		nop
		mov r0,#0x0
	.endarea
.close

.open "overlay11.bin", overlay11_start
	.org 0x022e94b4 ; GenerateDailyMissions callsite
	.area 0x4
		nop
	.endarea
.close

.open "overlay29.bin", overlay29_start
	.org 0x022ea950
	.area 0x4
		nop
	.endarea
	
	.org DisplayUi
	.area 0x4
		bx r14
	.endarea
	
	.org 0x0234bd68 ; RunDungeon callsite
	.area 0x4
		bl CustomRunDungeon
	.endarea
	
    .org 0x022df810 ; StartDungeonFadeIn
    .area 0x4
        bl CustomIntro
    .endarea
	
	.org 0x022df65c ; DisplayFloorCard callsite
	.area 0x4
		nop
	.endarea
	
	.org 0x022df670 ; GenerateFloor callsite
	.area 0x4
		bl CustomGenerateFloor
	.endarea
	
	.org SpawnMonster
	.area 0x8
		eor r0,r0,r0
		bx r14
	.endarea
	
	.org SpawnItem
	.area 0x8
		eor r0,r0,r0
		bx r14
	.endarea
	
	.org SpawnTrap
	.area 0x8
		eor r0,r0,r0
		bx r14
	.endarea
	
	.org 0x022f08e0
	.area 0x4
		bl TryAdvanceColorFrame
	.endarea
	
	.org 0x022df4dc
	.area 0x4
		mov r0,#39
	.endarea
.close

.endif