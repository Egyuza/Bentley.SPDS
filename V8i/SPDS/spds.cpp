/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include    <mdl.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <mselemen.fdf>
#include    <mselmdsc.fdf>
#include    <mslinkge.fdf>
#include    <msscancrit.fdf>
#include    <mstagdat.fdf>
#include    <mselems.h>
#include    <mscell.fdf>
#include    <leveltable.fdf>
#include    <mslstyle.fdf>
#include    <msstrlst.h>
#include    <mscnv.fdf>
#include    <msdgnobj.fdf>
#include    <msmodel.fdf>
#include    <msview.fdf>
#include    <msviewinfo.fdf>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msdialog.fdf>

#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>

#include <toolsubs.h>
#include <elementref.h>
#include <msdependency.fdf>
#include <msassoc.fdf>
#include <msmisc.fdf>
#include <mslocate.fdf>
#include <msstate.fdf>
#include <msoutput.fdf>
#include <MicroStationAPI.h>
#include <mstypes.h>
#include <mstmatrx.fdf>
#include <cmdlist.h>

#include <clocale>

#include "spdscmd.h"
#include "spds.h"
#include "TextGrabHelper.h"
#include "LeaderSetTool.h"
#include "LeaderToolModify.h"

USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;

bool isTextGrabRequired = true;
// TODO инкапсул€ци€, если возможно:
TextGrabParam inputParam;

/*---------------------------------------------------------------------------------**//**
* @description  spds_mdlDialogHook
* @param 	dmP      The DialogMessage 
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" Public void	spds_mdlDialogHook (DialogMessage   *dmP) 
{
    dmP->msgUnderstood = TRUE;
    switch (dmP->messageType) {
	case DIALOG_MESSAGE_CREATE:	{
	    dmP->u.create.interests.windowMoving= TRUE;
	    dmP->u.create.interests.resizes     = TRUE;
	    dmP->u.create.interests.updates     = TRUE;
	    dmP->u.create.interests.dialogFocuses = TRUE;

        // initialize Globals
	    break;
	}
	case DIALOG_MESSAGE_WINDOWMOVING: {
	    dmP->u.windowMoving.handled = FALSE;

        /* Don't process if only moving dialog box */
        if (dmP->u.windowMoving.whichCorners == CORNER_ALL ||
	        dmP->u.windowMoving.whichCorners == CORNER_ALL_RESIZED)
            break;
	    dmP->u.windowMoving.handled = TRUE;
	    break;
	}
	case DIALOG_MESSAGE_INIT: {
	    /* Fall through */
    }
	case DIALOG_MESSAGE_UPDATE: {
	    // resize items
	    break;
	}
	case DIALOG_MESSAGE_RESIZE: {
        /* Don't process if only moving dialog box */
        if (dmP->u.resize.whichCorners == CORNER_ALL)
            break;
	       
	    dmP->u.resize.forceCompleteRedraw = TRUE;
	    break;
	}
	case DIALOG_MESSAGE_FOCUSIN: {
	    break;
	}
	case DIALOG_MESSAGE_DESTROY: {
	    // save globals
	    break;
    }
	default:
	    dmP->msgUnderstood = FALSE;
	    break;
	}
}

void grabbingTextCallback(const TextGrabParam& param) {
    inputParam = param;
   // memcpy(inputParam, param, sizeof(inputParam));
    isTextGrabRequired = false;

    //! Ѕ≈«ќѕј—Ќџ… —ѕќ—ќЅ - системный запуск команды собственного тула,  
    // остальные способы приводили к вылету Microstation, т.к. в данный
    // момент времени ещЄ активет тул "Text Editor (Word Processor)"

    //! альтернатива запуску через keyin - строку:
    mdlDialog_cmdNumberQueue(true, CMD_SPDS_LEADER_PLACE, "", FALSE);
    // mdlInput_sendKeyin("spds leader place", 0, INPUTQ_HEAD, 0);

    // TODO можно написать "KeyinHelper" дл€ упрощЄнного запуска keyin
}

/*------------------------------------------------------------------------------
KEYIN: "SPDS LEADER PLACE"
----------------------------------------------------------------------------- */
extern "C" DLLEXPORT void LeaderPlaceCmd (char *unparsed) 
{
    if (isTextGrabRequired) {
        // запускаем текстовый редактор дл€ ввода текста:
        TextGrabHelper::Instance().startGrabbing(&grabbingTextCallback);
    }
    else {
        mdlState_clear();
        //mdlState_startDefaultCommand();

        isTextGrabRequired = true; // восстанавливаем значение флага
        LeaderSetTool *pTool = new LeaderSetTool();//(CMD_SPDS_LEADER_PLACE, 0);
        pTool->InstallTool();
        pTool->setInputParams(inputParam);
    }
   
    /*-----------------------------------------------------------------
    | Start dynamics to display elements when one more datapoint will 
    | give enough information to write an element to the design file.
    | For our text element, we can start dynamics right away.
    +-----------------------------------------------------------------*/
    //mdlState_setFunction (STATE_COMPLEX_DYNAMICS, generateText);
}


/*------------------------------------------------------------------------------
KEYIN: "SPDS LEADER MODIFY"
----------------------------------------------------------------------------- */
extern "C" DLLEXPORT void LeaderModifyCmd_Add (char *unparsed)
{
    LeaderToolModify *pTool = new LeaderToolModify();
    pTool->InstallTool(MOD_ADD);

}
extern "C" DLLEXPORT void LeaderModifyCmd_Delete(char *unparsed) {
    LeaderToolModify *pTool = new LeaderToolModify();
    pTool->InstallTool(MOD_DELETE);

}
extern "C" DLLEXPORT void LeaderModifyCmd_Replace(char *unparsed) {
    LeaderToolModify *pTool = new LeaderToolModify();
    pTool->InstallTool(MOD_REPLACE);
}


/*---------------------------------------------------------------------------------**//**
* @description  MdlMain
* @param 	argc      The number of command line parameters sent to the application.
* @param 	argv[]    The array of strings sent to the application on the command line.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT  int MdlMain (int argc, char *argv[])
{    
    std::setlocale(LC_ALL, "");
    
    RscFileHandle   rfHandle;
    mdlResource_openFile (&rfHandle,NULL,RSC_READONLY);


    // Map command name to function (usage: MDL COMMAND COMPEXPORT)
    static  MdlCommandName cmdNames[] = 
    {
        { LeaderPlaceCmd,  "LeaderPlaceCmd" },
        //{ LeaderModifyCmd_Add, "LeaderModifyCmd_Add" },
        { LeaderModifyCmd_Delete, "LeaderModifyCmd_Delete" },
        //{ LeaderModifyCmd_Replace, "LeaderModifyCmd_Replace" },
        0,
    };

    mdlSystem_registerCommandNames (cmdNames);

    // Map key-in to function
    static MdlCommandNumber cmdNumbers[] =
    {
        { LeaderPlaceCmd,   CMD_SPDS_LEADER_PLACE },
        //{ LeaderModifyCmd_Add,  CMD_SPDS_LEADER_MODIFY_ADD },
        { LeaderModifyCmd_Delete,  CMD_SPDS_LEADER_MODIFY_DELETE },
        //{ LeaderModifyCmd_Replace,  CMD_SPDS_LEADER_MODIFY_REPLACE },
        0,
    };

    mdlSystem_registerCommandNumbers(cmdNumbers);
    mdlParse_loadCommandTable(NULL);

    // регистрируем необходимые обработчики событий:
    mdlDependency_registerCallback(
        APPID_LEADER,
        mdlSystem_getCurrMdlDesc(),
        &Leader_TextDependencyCallback
    );

 	return SUCCESS;
}
