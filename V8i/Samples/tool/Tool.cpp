/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/Tool.cpp,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   toolExample  $
|   $Revision: 1.2 $
|   	$Date: 2013/06/28 20:30:23 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   toolExample - toolExample source code. |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include    <mdl.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msdialog.fdf>
#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>
#include <toolsubs.h>
#include <msoutput.fdf>
//added for new interfaces in XM/Athens
#include <microstationapi.h>
#include <mstypes.h>

//place command
char    *g_str;

#include "[!output SAFE_PROJECT_NAME]ToolCmd.h"
#include "[!output SAFE_PROJECT_NAME]Tool.h"
#include "[!output SAFE_PROJECT_NAME]PrimitiveTool.h"
#include "[!output SAFE_PROJECT_NAME]ModifyTool.h"

USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;


/*---------------------------------------------------------------------------------**//**
* @description  toolExample_mdlDialogHook
* @param 	dmP      The DialogMessage 
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" Public void	toolExample_mdlDialogHook
(
DialogMessage   *dmP
)
    {
    dmP->msgUnderstood = TRUE;
    switch (dmP->messageType)
	    {
	    case DIALOG_MESSAGE_CREATE:
	        {
	        dmP->u.create.interests.windowMoving= TRUE;
	        dmP->u.create.interests.resizes     = TRUE;
	        dmP->u.create.interests.updates     = TRUE;
	        dmP->u.create.interests.dialogFocuses = TRUE;

	    // initialize Globals
	    
	        break;
	        }
	    case DIALOG_MESSAGE_WINDOWMOVING:
	        {
	        dmP->u.windowMoving.handled = FALSE;

            /* Don't process if only moving dialog box */
            if (dmP->u.windowMoving.whichCorners == CORNER_ALL ||
	    	    dmP->u.windowMoving.whichCorners == CORNER_ALL_RESIZED)
                break;
	        dmP->u.windowMoving.handled = TRUE;
	        break;
	        }
	    case DIALOG_MESSAGE_INIT:
	        {
	    /* Fall through */
	        }
	    case DIALOG_MESSAGE_UPDATE:
	        {
	    // resize items
	        break;
	        }

	    case DIALOG_MESSAGE_RESIZE:
	        {
            /* Don't process if only moving dialog box */
            if (dmP->u.resize.whichCorners == CORNER_ALL)
                break;
	       
	        dmP->u.resize.forceCompleteRedraw = TRUE;
	        break;
	        }
	    case DIALOG_MESSAGE_FOCUSIN:
	        {
	        break;
	        }
	    case DIALOG_MESSAGE_DESTROY:
	        {
	        // save globals
	        break;
	        }
	    default:
	        dmP->msgUnderstood = FALSE;
	        break;
	    }
    }


[!if OPTION_PLACEMENT] 
/*----------------------------------------------------------------------+*//**
* 							      
* @bsimethod toolExamplePlaceCmd   				      
* @param 	unparsedP     Contination string that started this command
*							      
* Author:   BSI 				06/03 	      
*							      
+----------------------------------------------------------------------*/
extern "C" DLLEXPORT void [!output SAFE_PROJECT_NAME]PrimitiveToolCmd
(
char    *unparsed
)
    {

    [!output SAFE_PROJECT_NAME]PrimitiveTool *pTool = new [!output SAFE_PROJECT_NAME]PrimitiveTool(CMD_[!output SAFE_PROJECT_NAME_UPPERCASE]_ACTION_PLACE,0);

    if (unparsed)
        pTool->m_str = dlmSystem_strdup (unparsed);
    else
        pTool->m_str = dlmSystem_strdup("test");

    pTool->MyInstallTool();
   
    /*-----------------------------------------------------------------
    | Start dynamics to display elements when one more datapoint will 
    | give enough information to write an element to the design file.
    | For our text element, we can start dynamics right away.
    +-----------------------------------------------------------------*/
    //mdlState_setFunction (STATE_COMPLEX_DYNAMICS, generateText);
    }
[!endif]
[!if OPTION_MODIFY]
/*---------------------------------------------------------------------------------**//**
* @description  toolExample_mdlCommand
* @param 	unparsed      The unparsed information sent to the command
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void [!output SAFE_PROJECT_NAME]ModifyToolCmd 
(
char * unparsed
)
    {
	[!output SAFE_PROJECT_NAME]ModifyTool *pTool = new [!output SAFE_PROJECT_NAME]ModifyTool ();

    pTool->MyInstallTool();
    }
[!endif]

/*---------------------------------------------------------------------------------**//**
* @description  MdlMain
* @param 	argc      The number of command line parameters sent to the application.
* @param 	argv[]    The array of strings sent to the application on the command line.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT  int MdlMain
(
int         argc,
char        *argv[]
)
    {
	RscFileHandle   rfHandle;

    mdlResource_openFile (&rfHandle,NULL,RSC_READONLY);

    // Map command name to function (usage: MDL COMMAND COMPEXPORT)
    static  MdlCommandName cmdNames[] = 
    {
[!if OPTION_MODIFY]
        {[!output SAFE_PROJECT_NAME]ModifyToolCmd, "[!output SAFE_PROJECT_NAME]ModifyToolCmd"  },
[!endif]
[!if OPTION_PLACEMENT]
        {[!output SAFE_PROJECT_NAME]PrimitiveToolCmd, "[!output SAFE_PROJECT_NAME]PrimitiveToolCmd"  },
[!endif]
        0,
    };

    mdlSystem_registerCommandNames (cmdNames);

    // Map key-in to function
    static MdlCommandNumber cmdNumbers[] =
    {
[!if OPTION_MODIFY]
		{[!output SAFE_PROJECT_NAME]ModifyToolCmd,  CMD_[!output SAFE_PROJECT_NAME_UPPERCASE]_ACTION_MODIFY },
[!endif]
[!if OPTION_PLACEMENT]
		{[!output SAFE_PROJECT_NAME]PrimitiveToolCmd,  CMD_[!output SAFE_PROJECT_NAME_UPPERCASE]_ACTION_PLACE },
[!endif]
        0,
    };

    mdlSystem_registerCommandNumbers (cmdNumbers);

    mdlParse_loadCommandTable (NULL);

 	return SUCCESS;
    }
