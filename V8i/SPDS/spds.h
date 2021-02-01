#pragma once

// TODO –ј«ќЅ–ј“№

#define DIALOGID_MDLDialog	    1
#define HOOKDIALOGID_MDLDialog	    (DIALOGID_MDLDialog * 100) + 1
#define DIALOGID_HostedDialog	    2
#define HOOKDIALOGID_HostedDialog   (DIALOGID_HostedDialog * 100) + 1



#define PUSHBUTTONID_ModalDialog	2
#define PUSHBUTTONID_ModelessDialog	3
#define PUSHBUTTONID_HostedDialog	4
#define PUSHBUTTONID_MDLButton		10
#define PUSHBUTTONID_DockableDialog	5
#define PUSHBUTTONID_ToolSettingsDialog	6

#define MESSAGELISTID_Commands	    1
#define MESSAGELISTID_Messages	    2
#define MESSAGELISTID_Prompts	    3
#define MESSAGELISTID_Misc	    4
#define	MESSAGEID_ResourceLoadError 5

#define MESSAGEID_ToolSettings  1
#define MESSAGEID_abc		2
#define COMMANDID_PlaceDate	3

#define PROMPTMESSAGEID_abc	1
#define PROMPTID_EnterPoint	2

#define MISCMESSAGEID_abc	1


// -----------------------------------------------------------------------------

#define APPID_LEADER       1

#define MAX_STR_LEN MAX_LINKAGE_STRING_LENGTH

// масштабный коэффициент дл€ трансл€ции размеров в мм на активную модель
#define ACTIVE_SCALE (mdlModelRef_getUorPerMeter(ACTIVEMODEL)/1000)
// масштаб аннотаций активной модели
#define ACTIVE_ANN_SCALE (mdlModelRef_getEffectiveAnnotationScale(ACTIVEMODEL))

#define PI 3.1415926535897932384626433