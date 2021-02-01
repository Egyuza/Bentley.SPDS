#include "spds.h"

/*------------------------------------------------------------------------------**//**
* @description  Map key-in to function
* Commands
+---------------+---------------+---------------+---------------+---------------+------*/
static MdlCommandNumber s_commandNumbers[] =
{
    { startLeaderTool,  CMD_SPDS_LEADER },
    { startLeaderToolSet,  CMD_SPDS_LEADERTOOLSET },
    // end of list
    0
};

/*---------------------------------------------------------------------------------**//**
* @description  MdlMain
* @param        argc      The number of command line parameters sent to the application.
* @param        argv[]    The array of strings sent to the application on the command line.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" void MdlMain(int argc, WCharCP argv[]) {
    RscFileHandle rfHandle;

    mdlResource_openFile(&rfHandle, NULL, RSC_READONLY);
    mdlState_registerStringIds(STRINGLISTID_Commands, STRINGLISTID_Prompts);
    mdlSystem_registerCommandNumbers(s_commandNumbers);
    mdlParse_loadCommandTable(NULL);
}