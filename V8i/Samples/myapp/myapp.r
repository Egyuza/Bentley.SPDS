/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/myapp/myapp.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   myapp.r  $
|   $Revision: 1.5 $
|   	$Date: 2013/06/28 20:40:46 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   myapp.r - MyApp Dialog Box resource definitions			|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <dlogbox.h>
#include <dlogids.h>
#include <cmdlist.h>
#include <pselect.ids>

#include <keys.h>

#include "myapp.h"
#include "myappcmd.h"
#include "myapptxt.h"

/*----------------------------------------------------------------------+
|									|
| Use of predefined MicroStation symbol MSVERSION to compile the correct|
| version of your code.							|
|									|
|   #if defined (MSVERSION) && (MSVERSION >= 0x551)			|
|	code new to MicroStation Version 5.5.1 and higher		|
|   #endif								|
|									|
|   #if defined (MSVERSION) && (MSVERSION >= 0x550)			|
|	code new to MicroStation PowerDraft and higher			|
|   #endif								|
|									|
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
|									|
|   Dialog Box Resource Specification					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   DialogBoxRsc Structure						|
|									|
|   The DialogBoxRsc data structure (defined in "dlogbox.h") defines 	|
|   a dialog box in a resource file. Its header section contains	|
|   information for the entire dialog (like its width and height), 	|
|   and then a list of items.						|
|									|
|   typedef struct dialogboxrsc						|
|    	{								|
|    	ULong		attributes;     # dialog attributes 		|
|    	int		width;          # dialog coords 		|
|    	int		height;         # dialog coords 		|
|    	ULong		helpInfo;       # help for entire dialog 	|
|    	char		*helpSource;    # help task ID			|
|    	long		dialogHookId;   # dialog hook ID 		|
|    	long		parentDialogId; # to inform when destroyed 	|
|   #if defined (resource)						|
|    	char		label[];	# dialog label (title)		|
|	DialogItemRsc   itemList[];     # array of dialog items 	|
|   #else								|
|    	long		labelLength;    # length of label (title)       |
|    	char            label[1];       # dialog label (title)		|
|   #endif								|
|   	} DialogBoxRsc;							|
|									|
|									|
|   Each item in a dialog box needs to have an DialogItemRsc entry in	|
|   the DialogBoxRsc field, itemList. The order that items are listed	|
|   is important in two respects: the items are drawn in the order	|
|   specified, and the input focus moves from item to item based on	|
|   this order. DialogItemRsc data structure is declared as follows:	|
|									|
|   typedef struct dialogitemrsc					|
|    	{								|
|    	Sextent extent;		    # item area, origin			|
|				       (in dialog coords),		|
|				       if width/height is zero, use	|
|				       dimensions specified in item 	|
|    	long        type;	    # item type 			|
|    	long        id;             # item ID   			|
|    	byte        attributes;     # item attributes   		|
|    	long        itemArg;        # item argument 			|
|   #if defined (resource)						|
|    	char        label[];        # item label 			|
|    	char        auxInfo[];      # item auxiliary information 	|
|   #else								|
|    	int         labelLength;    # length of item label 		|
|    	char        label[1];       # item label 			|
|   #endif								|
|    	} DialogItemRsc;						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   MyApp Dialog Box							|
|									|
+----------------------------------------------------------------------*/

#define DW  20*XC
#define DH  12*YC

DialogBoxRsc DIALOGID_MyApp=
    {
    DIALOGATTR_DEFAULT | DIALOGATTR_SINKABLE,
    DW, DH,
    NOHELP, MHELP, HOOKDIALOGID_Dlog, NOPARENTID,
    TXT_Main,
{
{{0, 0, 0, 0},            MenuBar, MENUBARID_Menu, ON, 0, "", ""},
{{7*XC,  GENY(2), 12*XC, 0}, Text, TEXTID_UserInfo, ON, 0, "", ""},
{{7*XC,  GENY(4), 5*XC,  0}, Text, TEXTID_ElementColor, ON, 0, "", ""},
{{13*XC, GENY(4), 0,     0}, ColorPicker, COLORPICKERID_ElementColor, ON, 0, "", ""},

/* Note: The width of the option button is negative which represents
    a number of pixels instead of a number of dialog coordinate units */
{{7*XC,  GENY(6), -10*XC, 0}, OptionButton, OPTIONBUTTONID_LineStyle, ON, 0, "", ""},
{{6*XC,  GENY(8), BUTTON_STDWIDTH, 0}, PushButton, PUSHBUTTONID_MyDone, ON, 0, "", ""},
}
}
extendedIntAttributes
    {{
    {EXTINTATTR_DLOGATTRS, DIALOGATTRX_NETDOCKABLE}
    }

   };

#undef DW
#undef DH


/*----------------------------------------------------------------------+
|									|
|   Item Resource Specifications					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Menu Bar Item Resource      					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   The menu bar item is defined in a resource file with the		|
|   following structure:						|
|									|
|   typedef struct ditem_menubarrsc					|
|    	{								|
|    	long    itemHookId;						|
|    	long    itemHookArg;						|
|   #if defined (resource)						|
|    	DItem_MenuBarItemRsc menuBarItems[];				|
|   #else								|
|    	long    nMenus;							|
|    	DItem_MenuBarItemRsc pulldownMenus[1];				|
|   #endif								|
|    	} DItem_MenuBarRsc;						|
|									|
|   The only unique field in the DItem_MenuBarRsc structure is the	|
|   array of DItem_MenuBarItemRsc that is the menu bar item's list of	|
|   pull-down menus.							|
|									|
|   typedef struct ditem_menubaritemrsc					|
|   	{								|
|   	long    type;							|
|   	long    ID;							|
|   	} DItem_MenuBarItemRsc;						|
|									|
+----------------------------------------------------------------------*/
DItem_MenuBarRsc MENUBARID_Menu =
    {
    NOHOOK, NOARG,
	{
	{PulldownMenu, PULLDOWNID_Palette},
        {PulldownMenu, PULLDOWNID_HelpMenu},
	}
    };

/*----------------------------------------------------------------------+
|									|
|   PullDown Menu Item Resource						|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|								        |
|   The text pull-down menu item is defined in a resource file with    	|
|   the following structure:						|
|									|
|   typedef struct ditem_pulldownmenursc				|	
|      {								|
|      ULong   helpInfo;						|
|      ULong   helpSource;						|
|      long    pulldownHookId;						|
|      ULong   attributes;						|
|   #if defined (resource)						|
|      char    menuTitle[];						|
|      DItem_PulldownMenuItemRsc pulldownItems[];			|
|   #else								|
|      long    menuTitleLength;						|
|      char    menuTitle[1];						|
|   #endif								|
|      } DItem_PulldownMenuRsc;						|
|									|
|   The text pull-down item resource specification is defined as	|
|   follows:								|
|									|
|   typedef struct ditem_pulldownmenuitemrsc				|
|      {								|
|   #if defined (resource)						|
|      char    label[];							|
|      ULong   accelerator;						|
|      byte    enabled;							|
|      byte    mark;							|
|      long    subMenuType;						|
|      long    submenuId;						|
|      ULong   helpInfo;						|
|      char    helpTaskId[];						|
|      long    pulldownItemHookId;					|
|      long    searchId;           # used to search for item		|
|      ULong   commandNumber;						|
|      char    commandTaskId[];						|
|      char    unparsed[];						|
|   #else								|
|      long    labelLength;						|
|      char    label[1];						|
|   #endif								|
|      } DItem_PulldownMenuItemRsc;					|
|									|
+----------------------------------------------------------------------*/
DItem_PulldownMenuRsc PULLDOWNID_Palette =
    {
    NOHELP, MHELP, NOHOOK, ON | ALIGN_LEFT, TXT_PaletteMenu,
{
{TXT_MyappMenuItem,	    	0, ON, NOMARK, 0, NOSUBMENU, 
			    	NOHELP, MHELP,HOOKITEMID_Palette, NOARG, 0, 
			    	MCMD, ""},
{"-",			    	0, ON, NOMARK, 0, NOSUBMENU, 
			    	NOHELP, MHELP, NOHOOK, NOARG, 0, 
			    	MCMD, ""},
{TXT_ChangeSymItem,	    	VKEY_END, ON, NOMARK, 0, NOSUBMENU, NOHELP, 
			    	MHELP, NOHOOK, NOARG, 
				CMD_CHANGE_MYSYMBOLOGY, LCMD, ""},
{TXT_PlaceLineItem,	    	VBIT_ALT | VKEY_INSERT, ON, NOMARK, 0, NOSUBMENU, 
			    	NOHELP, MHELP, NOHOOK, NOARG, 
			    	CMD_PLACE_MYLINE, LCMD, ""},
{TXT_PlaceDateItem,	    	VBIT_CTRL | 'p', ON, NOMARK, 0, NOSUBMENU, 
			    	NOHELP, MHELP, NOHOOK, NOARG, 
			    	CMD_PLACE_MYDATE, LCMD, ""},
}
    };

DItem_PulldownMenuRsc PULLDOWNID_HelpMenu =
    {
    NOHELP, MHELP, NOHOOK, ON | ALIGN_RIGHT, TXT_Help,
{
#if defined (MSVERSION) && (MSVERSION >= 0x550)
/*-----------------------------------------------------
| In MicroStation 95, the MDL application HELPLOAD is the
| help system.
+----------------------------------------------------*/
{TXT_HelpContents,		NOACCEL, ON,NOMARK,0,NOSUBMENU,
			        NOHELP, MHELP, NOHOOK, 0,
			        CMD_MDL_KEYIN, MTASKID,
				"HELPLOAD HELP CONTENTS MYAPP"},
{TXT_HelpSearch,	        NOACCEL, ON,NOMARK,0,NOSUBMENU,
			        NOHELP, MHELP, NOHOOK, 0,
			        CMD_MDL_KEYIN, MTASKID,
				"HELPLOAD HELP SEARCH MYAPP"},
{TXT_HelpOnHelp,	        NOACCEL, ON,NOMARK,0,NOSUBMENU,
			        NOHELP, MHELP, NOHOOK, 0,
			        CMD_MDL_KEYIN, MTASKID,
				"HELPLOAD HELP HELP"},
#else
/*-----------------------------------------------------
| In MicroStation V5, the MDL application USTNHELP is the
| help system.
+----------------------------------------------------*/
{TXT_HelpContents,		NOACCEL, ON,NOMARK,0,NOSUBMENU,
			        NOHELP, MHELP, NOHOOK, 0,
			        CMD_MDL_KEYIN, MTASKID,
				"USTNHELP HELP CONTENTS MYAPP"},
{TXT_HelpSearch,	        NOACCEL, ON,NOMARK,0,NOSUBMENU,
			        NOHELP, MHELP, NOHOOK, 0,
			        CMD_MDL_KEYIN, MTASKID,
				"USTNHELP HELP SEARCH MYAPP"},
{TXT_HelpOnHelp,	        NOACCEL, ON,NOMARK,0,NOSUBMENU,
			        NOHELP, MHELP, NOHOOK, 0,
			        CMD_MDL_KEYIN, MTASKID,
				"USTNHELP HELP HELP"},
{TXT_HelpAllFiles,       	NOACCEL, ON,NOMARK,0,NOSUBMENU,
			        NOHELP, MHELP, NOHOOK, 0,
			        CMD_MDL_KEYIN, MTASKID,
				"USTNHELP HELP ALL"},
#endif
}
    };
/*----------------------------------------------------------------------+
|                                                                       |
|   Text Item Resource Definitions					|
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   The text item is defined in a resource file with the following	|
|   structure:								|
|									|
|   typedef struct ditem_textrsc					|
|      {								|
|      ULong   commandNumber;						|
|      ULong   commandSource;						|
|      long    synonymsId;						|
|      ULong   helpInfo;						|
|      ULong   helpSource;						|
|      long    itemHookId;						|
|      long    itemHookArg;						|
|      byte    maxSize;              /* max # of chars in field 	|
|      char    formatToDisplay[16];  /* conv to display<-internal 	|
|      char    formatToInternal[16]; /* conv to internal->display 	|
|      char    minimum[16];          /* minimum value 			|
|      char    maximum[16];          /* maximum value 			|
|      ULong   mask;                 /* only used with int types 	|
|      UShort  attributes;           /* other attributes		|
|   #if defined (resource)						|
|      char    label[];							|
|      char    accessStr[];						|
|   #else								|
|     long    labelLength;						|
|      char    label[1];						|
|   #endif								|
|      } DItem_TextRsc;							|
|									|
+----------------------------------------------------------------------*/
DItem_TextRsc TEXTID_UserInfo =
   {
   NOCMD, LCMD, NOSYNONYM, NOHELP, MHELP, HOOKITEMID_MyDate, NOARG, 
   25, "%s", "%s", "", "", NOMASK, NOCONCAT, 
   TXT_DateItemLabel, 
   "dlogBoxInfo.fmtStr"
   };

DItem_TextRsc TEXTID_ElementColor =
   {
   NOCMD, MCMD, SYNONYMID_ElementColor, NOHELP, MHELP, 
   HOOKITEMID_MyElementColor, NOARG, 
   25, "%d", "%d", "", "", NOMASK, NOCONCAT, 
   TXT_ColrItemLabel, 
   "tcb->symbology.color"
   };

/*----------------------------------------------------------------------+
|                                                                       |
|   Option Button Item Resource Definitions				|
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   The option button item is defined in a resource file with the	|
|   following structure:						|
|									|
|   typedef struct ditem_optionbuttonrsc				|
|      {								|
|      long    synonymsId;						|
|      ULong   helpInfo;						|
|      ULong   helpSource;						|
|      long    itemHookId;						|
|      long    itemHookArg;						|
|   #if defined (resource)						|
|      char    label[];							|
|      char    accessStr[];						|
|      DItem_OptionButtonItemRsc optio	nButtonItems[];			|
|   #else								|
|      long    labelLength;						|
|      char    label[1];						|
|   #endif								|
|   } DItem_OptionButtonRsc;						|
|									|
|   The only unique field in the DItem_OptionButtonRsc structure is	|
|   the array of DItem_OptionButtonItemRscs that is the option button's	|
|   subitem list. For consistency with MicroStation's dialog boxes, an	|
|   option button's label should always end with a colon.		|
|									|
|   typedef struct ditem_optionbuttonitemrsc				|
|      {								|
|      ULong   iconType;						|
|      long    iconId;							|
|      ULong   commandNumber;						|
|      ULong   commandSource;						|
|      ULong   value;							|
|      ULong   mask;							|
|      byte    enabled;							|
|   #if defined (resource)						|
|      char    label[];							|
|   #else								|
|      long    labelLength;						|
|      char    label[1];						|
|   #endif								|
|      } DItem_OptionButtonItemRsc;					|
|									|
+----------------------------------------------------------------------*/
DItem_OptionButtonRsc OPTIONBUTTONID_LineStyle =
    {
    NOSYNONYM, NOHELP, LHELP, 
    HOOKITEMID_MyLineStyle, OPTNBTNATTR_NEWSTYLE | NOARG, 
    TXT_OptnItemLabel, 
    "dlogBoxInfo.style", 
{
{Icon, ICONID_LineStyle0Wide, NOCMD, MCMD, 0, NOMASK, ON, "~0"},
{Icon, ICONID_LineStyle1Wide, NOCMD, MCMD, 1, NOMASK, ON, "~1"},
{Icon, ICONID_LineStyle2Wide, NOCMD, MCMD, 2, NOMASK, ON, "~2"},
{Icon, ICONID_LineStyle3Wide, NOCMD, MCMD, 3, NOMASK, ON, "~3"},
{Icon, ICONID_LineStyle4Wide, NOCMD, MCMD, 4, NOMASK, ON, "~4"},
{Icon, ICONID_LineStyle5Wide, NOCMD, MCMD, 5, NOMASK, ON, "~5"},
{Icon, ICONID_LineStyle6Wide, NOCMD, MCMD, 6, NOMASK, ON, "~6"},
{Icon, ICONID_LineStyle7Wide, NOCMD, MCMD, 7, NOMASK, ON, "~7"},
}
    };

/*----------------------------------------------------------------------+
|                                                                       |
|   Push Button Item Resource Definitions				|
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   The push button item is defined in a resource file with the		|
|   following structure:						|
|									|
|   typedef struct ditem_pushbuttonrsc					|
|      {								|
|      char    isDefault;      # TRUE if this is default button		|
|      ULong   helpInfo;						|
|      ULong   helpSource;						|
|      long    itemHookId;						|
|      long    itemHookArg;						|
|      ULong   commandNumber;						|
|      ULong   commandSource;						|
|   #if defined (resource)						|
|      char    unparsed[];						|
|      char    label[];							|
|   #else								|
|      long    unparsedLength;						|
|   									|
|      char    unparsed[1];						|
|   #endif								|
|   } DItem_PushButtonRsc;						|
|									|
|   Multi-lined push button items can be displayed by placing newline	|
|   characters ("\n") in the push button item label.			|
|									|
+----------------------------------------------------------------------*/
DItem_PushButtonRsc PUSHBUTTONID_MyDone =
   {
    DEFAULT_BUTTON, 
    NOHELP, MHELP,
    HOOKITEMID_MyDone, NOARG, 
    NOCMD, LCMD, "", 
    TXT_ButnItemLabel
   };

#if defined (MSVERSION) && (MSVERSION < 0x550)
/*----------------------------------------------------------------------+
|									|
|   Dialog Box Resource Specification					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   DialogBoxRsc Structure						|
|									|
|   The DialogBoxRsc data structure (defined in "dlogbox.h") defines 	|
|   a dialog box in a resource file. Its header section contains	|
|   information for the entire dialog (like its width and height), 	|
|   and then a list of items.						|
|									|
|   typedef struct dialogboxrsc						|
|    	{								|
|    	ULong		attributes;     # dialog attributes 		|
|    	int		width;          # dialog coords 		|
|    	int		height;         # dialog coords 		|
|    	ULong		helpInfo;       # help for entire dialog 	|
|    	char		*helpSource;    # help task ID			|
|    	long		dialogHookId;   # dialog hook ID 		|
|    	long		parentDialogId; # to inform when destroyed 	|
|   #if defined (resource)						|
|    	char		label[];	# dialog label (title)		|
|	DialogItemRsc   itemList[];     # array of dialog items 	|
|   #else								|
|    	long		labelLength;    # length of label (title)       |
|    	char            label[1];       # dialog label (title)		|
|   #endif								|
|   	} DialogBoxRsc;							|
|									|
|   Icon command frame							|
|									|
|   An icon command frame should be the only item in a dialog box. Icon |
|   command frames should usually be vertically oriented (which means	|
|   that they have more rows than columns).				|
|  									|
|   Item list spec.							|
|									|
|   The DialogItemRsc field extent is unused for icon command frame 	|
|   items. The dialog box that contains an icon command frames is	|
|   resized so the command frame fills the dialog box. The extent	|
|   should be set to {0, 0, 0, 0}.					|
|									|
|   The type field should be IconCmdFrame.				|
|   The ID determines the DItem_IconCmdFrameRsc instance to load.	|
|									|
|   The attributes are ignored for icon command frame items. The icon	|
|   command frame is always enabled and displayed.			|
|									|
|   The itemArg field is unused and should usually be set to 0.		|
|   The label field is unused and should be set to "".			|
|   The auxInfo field is unused and should be set to "".		|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Main Palette Dialog Box                                             |
|									|
+----------------------------------------------------------------------*/
DialogBoxRsc DIALOGID_MyAppFrame =
    {
    DIALOGATTR_DEFAULT | DIALOGATTR_NORIGHTICONS,
    XC, YC, NOHELP, MHELP,
    NOHOOK, NOPARENTID, 
    TXT_CmdFrameTitle,
{
{{0, 0, 0, 0}, IconCmdFrame, ICONCMDFRAMEID_MyApp, ON, 0, "", ""},
}
    };

/*----------------------------------------------------------------------+
|									|
|   Icon Cmd Frame							|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Icon Command Frame Item resource specification			|
|   									|
|   The icon command frame item is defined in a resource file with	|
|   the following structure:						|
|									|
|   typedef struct ditem_iconcmdframersc				|
|      {								|
|      int         nColumns;						|
|      int         nRows; 						|
|      ULong       helpInfo;						|
|      ULong       helpSource;						|
|   #if defined (resource)						|
|      char        label[];						|
|      DialogItemSpec iconPieces[];					|
|   #else								|
|      long        labelLength;						|
|      char        label[1];						|
|   #endif								|
|      } DItem_IconCmdFrameRsc;						|
|									|
+----------------------------------------------------------------------*/
DItem_IconCmdFrameRsc ICONCMDFRAMEID_MyApp =
    {
    1, 3, NOHELP, MHELP, TXT_CmdFrameTitle,
	{
 	{IconCmd, 		ICONCMDID_ChooseElement},
	{IconCmdPaletteX,	ICONCMDPALETTEID_MyCommands},
	{IconCmd, 		ICONCMDID_DeleteElement},
	}
    };


/*----------------------------------------------------------------------+
|									|
|   Line Icon Cmd Palette - Here the definition of the Myapp commands	|
|	IconCmdPalette.  These are the "sub-palettes".  These are 	|
|	usually oriented horizontally.  Again the practical limit	|
|	on the size of these palettes are about 10 wide by 2 tall.	|
|									|
+----------------------------------------------------------------------*/
DItem_IconCmdPaletteXRsc	ICONCMDPALETTEID_MyCommands =
    {
    4, 1, 0, NOHELP, MHELP, NOHOOK, NOARG, 0, TXT_MyappPalette,
	{
	{ICONCMDID_PlaceMyLine,			"MYAPP"},
	{ICONCMDID_PlaceMyDate,			"MYAPP"},
	{ICONCMDID_ChangeElementSymbology,	"MYAPP"},
	{ICONCMDID_50PlaceText,			""},
	}
    };


#else /* Using tool box definitions */

DialogBoxRsc DIALOGID_MyAppFrame =
    {
    DIALOGATTR_DEFAULT  | DIALOGATTR_NORIGHTICONS | DIALOGATTR_AUTOOPEN | 
    DIALOGATTR_DOCKABLE | DIALOGATTR_BOTHVIRTUAL,
    XC, YC, NOHELP, MHELP,
    NOHOOK, NOPARENTID, 
    TXT_CmdFrameTitle,
{
{{0, 0, 0, 0}, IconCmdFrameX, ICONCMDFRAMEID_MyApp, ON, 0, "", ""},
}
    };

DItem_IconCmdFrameXRsc ICONCMDFRAMEID_MyApp =
    {
    1, 3, NOHELP, MHELP, 0, TXT_CmdFrameTitle,
	{
 	{IconCmd, 		ICONCMDID_ChooseElement,	""},
	{ToolBox,		TOOLBOXID_MyCommands,		"MYAPP"},
	{IconCmd, 		ICONCMDID_DeleteElement,	""},
	}
    };

DialogBoxRsc TOOLBOXID_MyCommands =
    {
    DIALOGATTR_TOOLBOXCOMMON,
    0, 0,
    NOHELP, MHELP, 
    NOHOOK, NOPARENTID,
    TXT_MyappPalette,
{
{{0, 0, 0, 0}, ToolBox, TOOLBOXID_MyCommands, ON, 0, "", ""},
}
    };


DItem_ToolBoxRsc	TOOLBOXID_MyCommands =
    {
    NOHELP, MHELP, NOHOOK, NOARG, 0, TXT_MyappPalette,
	{
{{ 0, 0, 0, 0}, IconCmd, ICONCMDID_PlaceMyLine, 	   ON, 0, "", "owner=\"MYAPP\""},
{{ 0, 0, 0, 0}, IconCmd, ICONCMDID_PlaceMyDate,      	   ON, 0, "", "owner=\"MYAPP\""},
{{ 0, 0, 0, 0}, IconCmd, ICONCMDID_ChangeElementSymbology, ON, 0, "", "owner=\"MYAPP\""},
{{ 0, 0, 0, 0}, IconCmd, ICONCMDID_50PlaceText, 	   ON, 1, "", ""},
	}
    };

#endif

/*----------------------------------------------------------------------+
|									|
|  Icon Cmd Resources                                                   |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Icon command							|
|									|
|   Icon command items cannot be directly placed in a dialog box item	|
|   list. They must be referenced by an icon command palette item or an	|
|   icon command frame item. 						|
|									|
|   Each DItem_IconCmdRsc structure has associated IconCmdSmallRsc 	|
|   and IconCmdLargeRsc instances. These resources contain the icon's 	|
|   actual raster bitmaps. These three resources are connected by	|
|   making their resource IDs identical. For example, if an instance	|
|   of DItem_IconCmdRsc has an ID of 5, the IDs of the corresponding	|
|   IconCmdSmallRsc and IconCmdLargeRsc should also be 5.		|
|									|
|   For consistency with MicroStation's icon commands, small icons	|
|   should be 23 pixels wide by 23 pixels high, and large icons should	|
|   be 31 pixels wide by 31 pixels high.				|
|									|
|   Item resource specification						|
|									|
|   The icon command item is defined in a resource file with the	|
|   following structure:						|
|									|
|   typedef struct ditem_iconcmdrsc					|
|      {								|
|      ULong   helpInfo;						|
|   #if defined (resource)						|
|      char    helpTaskId[];						|
|      ULong   attributes;						|
|      ULong   commandNumber;						|
|      char    commandTaskId[];						|
|      char    unparsed[]						|
|      char    enabledAccessStr[];					|
|      DialogItemRsc iconItems[];					|
|   #else								|
|      long    helpTaskLen;						|
|      char    helpTaskId[1];						|
|   #endif								|
|      } DItem_IconCmdRsc;						|
|									|
+----------------------------------------------------------------------*/
DItem_IconCmdRsc ICONCMDID_PlaceMyLine =
    {
    NOHELP, MHELP, 0,
    CMD_PLACE_MYLINE, OTASKID, "", "",
	{
#   	if defined (MSVERSION) && (MSVERSION < 0x551)
	{{7*XC,  GENY(1), -10*XC, 0}, OptionButton, OPTIONBUTTONID_LineStyle, ON, 0, "", ""},
#   	endif
	}
#if defined (MSVERSION) && (MSVERSION >= 0x550)
    }
    extendedAttributes
    {{
    {EXTATTR_FLYTEXT, TXT_Flyover_PlaceMyLine}, 
    {EXTATTR_BALLOON, TXT_Balloon_PlaceMyLine}, 
    }
#endif
    };

#if defined (MSVERSION) && (MSVERSION >= 0x551)
CmdItemListRsc CMD_PLACE_MYLINE =
    {{
    {{7*XC,  GENY(1), -10*XC, 0}, OptionButton, OPTIONBUTTONID_LineStyle, ON, 0, "", ""},
    }};
#endif


DItem_IconCmdRsc ICONCMDID_PlaceMyDate =
    {
    NOHELP, MHELP, 0,
    CMD_PLACE_MYDATE, OTASKID, "", "",
	{
#   	if defined (MSVERSION) && (MSVERSION < 0x551)
	{{7*XC,  GENY(1), 12*XC, 0}, Text, TEXTID_UserInfo, ON, 0, "", ""},
#   	endif
	}
#if defined (MSVERSION) && (MSVERSION >= 0x550)
    }
    extendedAttributes
    {{
    {EXTATTR_FLYTEXT, TXT_Flyover_PlaceMyDate}, 
    {EXTATTR_BALLOON, TXT_Balloon_PlaceMyDate}, 
    }
#endif
    };

#if defined (MSVERSION) && (MSVERSION >= 0x551)
CmdItemListRsc CMD_PLACE_MYDATE =
    {{
    {{7*XC,  GENY(1), 12*XC, 0}, Text, TEXTID_UserInfo, ON, 0, "", ""},
    {{2*XC, GENY(3), 25*XC, 0}, ComboBox, COMBOBOXID_TextFontName, ON, 0, "", "" },

    }};
#endif

DItem_IconCmdRsc ICONCMDID_ChangeElementSymbology =
    {
    NOHELP, MHELP, 0,
    CMD_CHANGE_MYSYMBOLOGY, OTASKID, "", "",
	{
#   	if defined (MSVERSION) && (MSVERSION < 0x551)
	{{7*XC,  GENY(1), 5*XC,0}, Text, TEXTID_ElementColor, ON, 0, "", ""},

	/* reuse the following MicroStation items from ustation.r */
	{{13*XC, GENY(1), 0,   0}, ColorPicker, COLORPICKERID_ElementColor, ON, 0, "", ""},
	{{1*XC,  GENY(2.5), 0, 0}, ToggleButton, TOGGLEID_UseFence, ON, 0, "", ""},
	{{13*XC, GENY(2.5), 0, 0}, OptionButton, OPTIONBUTTONID_LockFenceMode,ON, 0, "\0", ""},
#   	endif
	}
#if defined (MSVERSION) && (MSVERSION >= 0x550)
    }
    extendedAttributes
    {{
    {EXTATTR_FLYTEXT, TXT_Flyover_ChangeMySymbology}, 
    {EXTATTR_BALLOON, TXT_Balloon_ChangeMySymbology}, 
    }
#endif
    };
#if defined (MSVERSION) && (MSVERSION >= 0x551)
CmdItemListRsc CMD_CHANGE_MYSYMBOLOGY =
    {{
    {{7*XC,  GENY(1), 5*XC,0}, Text, TEXTID_ElementColor, ON, 0, "", ""},
    {{13*XC, GENY(1), 0,   0}, ColorPicker, COLORPICKERID_ElementColor, ON, 0, "", ""},
    {{1*XC,  GENY(2.5), 0, 0}, ToggleButton, TOGGLEID_UseFence, ON, 0, "", ""},
    {{13*XC, GENY(2.5), 0, 0}, OptionButton, OPTIONBUTTONID_LockFenceMode,ON, 0, "\0", ""},
    }};
#endif


/*----------------------------------------------------------------------+
|									|
|  Icon Resources                                                       |
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Small and Large Icon Resources are defined as follows:		|
|									|
|   typedef struct iconRsc						|
|       {								|
|       long	    width;						|
|       long	    height;						|
|       long	    format;						|
|       long	    fgColorIndex;					|
|       char	    name[16];						|
|   #if defined (resource)						|
|       byte	    data[];						|
|   #else								|
|       long	    numBytes;						|
|       byte	    data[1];						|
|   #endif								|
|       } IconRsc;  # IconCmdSmallRsc, IconCmdLargeRsc  		|
|									|
+----------------------------------------------------------------------*/
IconCmdSmallRsc ICONCMDID_PlaceMyLine =
    {
    23, 23, FORMAT_MONOBITMAP, BLACK_INDEX, TXT_PlaceMyLine,
	{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x08, 0x00, 0x00, 0x20, 0x00, 0x00, 0x80, 0x00,
	0x02, 0x00, 0x00, 0x08, 0x00, 0x00, 0x20, 0x00,
	0x00, 0x80, 0x00, 0x02, 0x00, 0x00, 0x08, 0x00,
	0x00, 0x20, 0x00, 0x00, 0x80, 0x00, 0x02, 0x00,
	0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00,
	}
    };

IconCmdLargeRsc ICONCMDID_PlaceMyLine =
    {
    31, 31, FORMAT_MONOBITMAP, BLACK_INDEX, TXT_PlaceMyLine,
	{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00,
	0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x80, 0x00,
	0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x20, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
	0x20, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x02,
	0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20,
	0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00,
	0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x20, 0x00,
	0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00,
	}
    };

IconCmdSmallRsc ICONCMDID_PlaceMyDate =
    {
    23, 23, FORMAT_MONOBITMAP, BLACK_INDEX, TXT_PlaceMyDate,
	{
        0xff, 0xf8, 0x01, 0x55, 0x50, 0x03, 0xff, 0xf8,
        0x05, 0x00, 0x10, 0x0e, 0x07, 0xff, 0xd4, 0x78,
        0x00, 0xb9, 0xd0, 0x01, 0x53, 0x20, 0x42, 0xe0,
        0x47, 0x85, 0x40, 0x83, 0x0b, 0x81, 0x06, 0x15,
        0x06, 0x0c, 0x2e, 0x1c, 0x18, 0x54, 0x78, 0x30,
        0xb9, 0xd0, 0x61, 0x53, 0x20, 0xc2, 0xe7, 0xc1,
        0x85, 0xc0, 0x83, 0x08, 0x81, 0x1f, 0x91, 0xfe,
        0x00, 0x20, 0x04, 0x00, 0x40, 0x0f, 0xff, 0x80,
        0x00, 0x00, 0x00, 
  	}
    };

IconCmdLargeRsc ICONCMDID_PlaceMyDate =
    {
    31, 31, FORMAT_MONOBITMAP, BLACK_INDEX, TXT_PlaceMyDate,
	{
        0xff, 0xff, 0x80, 0x01, 0x55, 0x55, 0x00, 0x03,
        0x55, 0x56, 0x00, 0x05, 0xff, 0xff, 0x80, 0x0d,
        0x00, 0x01, 0x00, 0x16, 0x00, 0x02, 0x00, 0x34,
        0x01, 0xff, 0xff, 0x58, 0x1e, 0x00, 0x02, 0xd0,
        0xfc, 0x00, 0x05, 0x63, 0x88, 0x00, 0x0b, 0x47,
        0x10, 0x18, 0x15, 0x80, 0x20, 0x70, 0x2d, 0x00,
        0x43, 0xe0, 0x56, 0x00, 0x81, 0xc0, 0xb4, 0x01,
        0x03, 0x81, 0x58, 0x06, 0x07, 0x02, 0xd0, 0x1c,
        0x0e, 0x05, 0x60, 0x78, 0x1c, 0x0b, 0x41, 0xd0,
        0x38, 0x15, 0x87, 0x20, 0x70, 0x2d, 0x1c, 0x40,
        0xe0, 0x56, 0x3f, 0x81, 0xc0, 0xb4, 0x7f, 0x03,
        0x81, 0x78, 0x02, 0x07, 0x02, 0x10, 0x04, 0x0e,
        0x04, 0x20, 0x08, 0x7f, 0x08, 0x7f, 0xf0, 0x00,
        0x10, 0x00, 0x20, 0x00, 0x20, 0x00, 0x40, 0x00,
        0x40, 0x00, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00,
        0x00, 
      	}
    };


/*----------------------------------------------------------------------+
|                                                                       |
|   Toggle Button Item Resource Definitions				|
|                                                                       |
+----------------------------------------------------------------------*/
DItem_ToggleButtonRsc TOGGLEID_UseFence =
    {
    CMD_CHANGE_MYSYMBOLOGY, LCMD, NOSYNONYM, 
    NOHELP, MHELP, 
    HOOKITEMID_UseFence, NOARG, NOMASK, NOINVERT,
    TXT_UseFenceLabel, 
    "tcb->msToolSettings.general.useFence"
    };
