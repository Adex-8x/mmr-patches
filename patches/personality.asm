.nds
.include "symbols.asm"

.open "arm9.bin", arm9_start
    .org PersonalityPartnerInit
        bl TryRandomizePartnerFirstKind
    
    .org InsertActorSpeciesHook
        b InsertActorSpecies
.close

.open "overlay13.bin", overlay13_start
    .org PersonalityScoreInit
        mov r9,#128 // Initial personality scores are 128

    .org PersonalityResetR9Hook
        add r3,r3,#1 // skip initialization of used_question_batches
        cmp r3,#4 // we only need to loop 4 times
        blt PersonalityResetR9Branch
        mov r9,#0

    .org PersonalitySkipBatchesHook
        b PersonalitySkipBatchesBranch // skip batch selection as it's unneeded for our system

    .org PersonalityUseNumAsId // Don't load from question_ids, just take the question number
        nop
        nop

    .org Personality5Answers
        mov r8,#5 // Force 5 answers for every question

    .org PersonalityAnswerStrings
        add r9,r7,#0x5d0

    .org PersonalityNoSpecialQuestions
        b PersonalityAfterQuestion // Skip special behavior for first+last questions

    .org PersonalityAfterQuestion
        nop
        mov r2,r1

    .org PersonalityCustomScoring
        // r0 holds menu result
        ldrb  r2,[r1,r2] // get question type
        sub   r0,r0,#2 // [0,4] -> [-2,2]
        tst   r2,#0b100 // 4 thru 7 are negative equivalents of 0 thru 3
        movne r1,#0
        subne r0,r1,r0 // flip score addition to negative if in a negative mode
        and   r2,r2,#0b11 // Now that negatives are handled, trim to first 2 bits to get actual mode
        ldr   r1,[r3] // PERSONALITY_TEST_PTR
        add   r1,r1,r2 // Index with mode
        ldrb  r2,[r1,#0x34] // Get current score for this trait
        add   r2,r2,r0 // Increment based on answer
        strb  r2,[r1,#0x34]
        b     PersonalityAfterScoring // Done increasing scores!

    .org PersonalityQuestionCount
        cmp r2,#40 // 40 total questions

    .org PersonalityAssignVar
        bl AssignPartnerFirstKind

    .org PersonalityBanishWindow3
        nop

    .org PersonalitySkipPartner
        mov r0,#0x36

    .org PersonalitySkipAura
        moveq r0,#0x21 // skip aura
    
    .org PersonalityBanishWindow3Again
        nop

    .org GetPersonality
        b CustomGetPersonality

    .org QUIZ_QUESTION_ANSWER_ASSOCIATIONS // Now holds the different "type" for each question
        .byte 0,5,2,3,4,1,6,7,0,5,2,3,4,1,6,7,0,5,2,3,4,1,6,7,0,5,2,3,4,1,6,3,0,1,2,3,4,1,2,3
.close
