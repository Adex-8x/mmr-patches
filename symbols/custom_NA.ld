/* !file overlay11 */
OpcodeCheck = 0x022DDDD4;
GetParameterCount = 0x022DDD8C;
ScriptEngineReturnTwo = 0x022E246C;
ScriptMenuRequestDefaultCase = 0x022E60F0;
ScriptMenuRequestFinalize = 0x022E68AC;
ScriptMenuUpdateDefaultCase = 0x022E68F8;
ScriptMenuUpdateFinalize = 0x022E6E58;
IS_BASE_GAME_MENU_FINISHED = 0x02324C9C;
GetSceneName = 0x022E4600;
GetActingSceneName = 0x022DEBF4;
CreateLivePerformer = 0x022FD72C;
AfterAttributeBitfieldSwitchCase = 0x022F816C;
DefaultActorTypeBranch = 0x022F80D0;
ShouldReloadCachedScene = 0x022E573C;
CreateSaveMenuCallsite = 0x022E670C;
CreateDayCareMenuCallsite = 0x022E63D0;

/* !file arm9 */
ShowKeyboardTypeCase3 = 0x02036914;
ShowKeyboardTypeDefaultCase = 0x02036A40;
ShowKeyboardReturn = 0x02036CAC;
PreprocessStringFromIdCallsite = 0x02039400;
TextboxSolid = 0x0202715C;
TextboxTransparent = 0x02027148;
GreaterThanActorId0xABranch = 0x02023F6C;
SaveFileIdentifier = 0x0209CD3C;
NoteSetHeaderMode = 0x0209CD4C;
CreatePortraitBoxCallsite = 0x02035118;
CreateDefaultDialogueBoxCallsite = 0x0203512C;
PlayBgmCheck = 0x02019948;
PlayBgm2Check = 0x02019E14;
ChangeVolumeBgm = 0x02017F64;
ChangeVolumeBgm2 = 0x02018194;
StopBgm = 0x02019b28;
UppercaseTextTagFatalError = 0x02021A10;
AfterUppercaseTagIsFound = 0x02021AA0;
GetTextSpeed = 0x02020ED0;
DoTextLoop = 0x02020EE0;
AnalyzeTextReturn = 0x02021EC8;
PlayBgm2ByIdVolumeVeneer = 0x02017BE0;
ChangeSeVolumeVeneer = 0x020183A0;
SoundStop = 0x02017AB0;
GetOptionsMenuAllChoices = 0x0202d5b0;
TEXTBOX_COLOR_ATTRIBUTES = 0x02027800;

/* PERSONALITY */
FadeOutBgm = 0x2017F28; /* EU: 0x2017FC4 */ 
PersonalityPartnerInit = 0x204B290; /* EU: 0x204B5C8 */
InsertActorSpeciesHook = 0x20656B4; /* EU: 0x2065A30 */
InsertActorSpeciesRet = 0x20656B8; /* EU: 0x2065A34 */
PartnerFirstKind = 0x2065808; /* EU: 0x2065B84 */ 
/* PersonalityTestMainManager tweaks - EU: +0xB40 */
PersonalityScoreInit = 0x238A7E4;
PersonalityResetR9Hook = 0x238A80C;
PersonalityResetR9Branch = 0x238A800;
PersonalitySkipBatchesHook = 0x238A830;
PersonalitySkipBatchesBranch = 0x238A870;
PersonalityUseNumAsId = 0x238A8F0;
Personality5Answers = 0x238A914;
PersonalityAnswerStrings = 0x238A96C;
PersonalityNoSpecialQuestions = 0x238AA9C;
PersonalityAfterQuestion = 0x238ABA4;
PersonalityCustomScoring = 0x238ABB4;
PersonalityAfterScoring = 0x238ABF8;
PersonalityQuestionCount = 0x238AC6C;
PersonalityAssignVar = 0x238AE04;
PersonalityBanishWindow3 = 0x238AE8C;
PersonalitySkipPartner = 0x238AEE8;
PersonalitySkipAura = 0x238AF2C;
PersonalityBanishWindow3Again = 0x238AFFC;
/* Already in pmdsky-debug but i need to define it separately under a different name so i can give it a different type */
PERSONALITY_TEST_PTR = 0x238CEA0;
HEROS = 0x238C0B8;

/* !file ram */
FONT_DATA = 0x022A7A54;
TSXPosLive = 0x22A35DC;
TSYPosLive = 0x22A35E0;
TSPressed = 0x22A35E4;
TSXPosLastMitDrag = 0x22A35FC;
TSYPosLastMitDrag = 0x22A3600;
