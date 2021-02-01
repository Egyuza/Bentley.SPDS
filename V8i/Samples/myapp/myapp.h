/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/myapp/myapp.h,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   myapp.h  $
|   $Revision: 1.3 $
|   	$Date: 2013/06/28 20:40:44 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   myapp.h - MyApp Dialog Box constants and types			|
|                                                                       |
+----------------------------------------------------------------------*/

#if !defined (__myappH__)
#define __myappH__

/*----------------------------------------------------------------------+
|									|
|   Dialog Box ID's							|
|									|
+----------------------------------------------------------------------*/
#define DIALOGID_MyApp  	   1
#define DIALOGID_MyAppFrame	   2
#define TOOLBOXID_MyCommands	   3

/*----------------------------------------------------------------------+
|									|
|   Dialog Item ID's							|
|									|
+----------------------------------------------------------------------*/
#define MENUBARID_Menu		    1

#define PULLDOWNID_Palette	    1
#define PULLDOWNID_HelpMenu	    2

#define TEXTID_UserInfo		    1

#define COLORPICKERID_ActiveColor   1

#define OPTIONBUTTONID_LineStyle    1

#define PUSHBUTTONID_MyDone	    1

#define ICONCMDFRAMEID_MyApp	    1

#define ICONCMDPALETTEID_MyCommands 1

#define ICONCMDID_PlaceMyLine       1
#define ICONCMDID_PlaceMyDate       2

/*----------------------------------------------------------------------+
|									|
|   Message list IDs							|
|									|
+----------------------------------------------------------------------*/
#define	MESSAGELISTID_Commands      0
#define	MESSAGELISTID_Prompts       1
#define	MESSAGELISTID_Msgs	    2

/*----------------------------------------------------------------------+
|									|
|   Prompt IDs - used in the Message list definition for command prompts|
|									|
+----------------------------------------------------------------------*/
#define	PROMPTID_EnterFirst	1
#define	PROMPTID_EnterNext  	2
#define	PROMPTID_EnterPoint	3
#define	PROMPTID_AcceptFence	4
#define	PROMPTID_IdentifyElt	5

/*----------------------------------------------------------------------+
|									|
|   Command IDs - used in the Message list definition for command names	|
|									|
+----------------------------------------------------------------------*/
#define	COMMANDID_PlaceMyLine		1
#define	COMMANDID_PlaceDate		2
#define	COMMANDID_ChangeSymbology 	3
#define	COMMANDID_ChangeSymbologyFence 	4

/*----------------------------------------------------------------------+
|									|
|   Command IDs - used in the Message list definition for command names	|
|									|
+----------------------------------------------------------------------*/
#define	MSGID_LoadCmdTbl	1
#define	MSGID_DialogOpen	2
#define	MSGID_DateString	3
#define	MSGID_RscOpenErr	4
#define	MSGID_SureString 	5
#define MSGID_PaletteOpen 	6
#define	MSGID_NoFence       	7
#define MSGID_InvalidLineStyle  8

/*----------------------------------------------------------------------+
|									|
|   Dialog Hook ID's							|
|									|
+----------------------------------------------------------------------*/
#define HOOKDIALOGID_Dlog	    1
#define HOOKITEMID_MyDone	    2
#define HOOKITEMID_Palette	    3
#define HOOKITEMID_UseFence	    4
#define HOOKITEMID_MyElementColor   5
#define HOOKITEMID_MyLineStyle      6
#define HOOKITEMID_MyDate	    7

/*----------------------------------------------------------------------+
|									|
|   Typedefs for Dialog access strings					|
|									|
+----------------------------------------------------------------------*/
typedef struct dlogboxinfo
{
    char      fmtStr[255];
    int	      style;
} DlogBoxInfo;

#endif /* #if !defined (__myappH__) */
