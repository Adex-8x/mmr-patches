.nds
.include "symbols.asm"

.open "arm9.bin", arm9_start
	.org PlayBgmCheck
	.area 0x4
		cmp r6,#0x2000
	.endarea

	.org PlayBgm2Check
	.area 0x4
		cmp r6,#0x2000
	.endarea

	.org SaveFileIdentifier
	.area 0x10
		.asciiz "MYSTERYMAIL"
		.fill NoteSetHeaderMode-., 0x0
	.endarea
	
	.org CreateDefaultDialogueBoxCallsite
	.area 0x4
		bl CustomCreateDialogueBox
	.endarea

	.org CreatePortraitBoxCallsite
	.area 0x4
		bl CustomCreatePortraitBox
	.endarea
	
	.org UppercaseTextTagFatalError
	.area 0x28
		mov r0,r4 ; dialogue_display_state pointer
		ldr r1,[r13,#0x70] ; tag string
		add r2,r13,#0x74 ; the rest of the tag params
		sub r3,r6,#0x1 ; tag count
		bl ParseCustomUppercaseTextTags
		cmp r0,#0x0
		bleq CardPullOut
		b AfterUppercaseTagIsFound
	.endarea
	
	.org GetTextSpeed
	.area 0x4
		bleq HijackTextSpeed
	.endarea
	
	.org DoTextLoop
	.area 0x4
		bl HijackTextLoop
	.endarea
.close

.open "overlay11.bin", overlay11_start
	.org GetActingSceneName
	.area 0x4
		bl CustomGetActingSceneName
	.endarea
	
	.org ShouldReloadCachedScene
	.area 0x18
		nop :: nop :: nop :: nop :: nop :: nop
	.endarea
	
	.org DefaultActorTypeBranch
	.area 0x4
		b CheckSpecialActorType
	.endarea

	.org AfterAttributeBitfieldSwitchCase
	.area 0x4
		b ManipulateActorLayering
	.endarea
	
	.org CreateSaveMenuCallsite
	.area 0x4
		bl CardPullOut
	.endarea
	
	.org CreateDayCareMenuCallsite
	.area 0x4
		bl CardPullOut
	.endarea
	
	.org PlayTimerTickWrapperCallsite
    .area 0x4
        bl YouCanDoAnything
    .endarea
.close

.open "overlay34.bin", overlay34_start
	.org ExplorersOfSkyMain
	.area 0x8
		eor r0,r0,r0
		bx r14
	.endarea
.close