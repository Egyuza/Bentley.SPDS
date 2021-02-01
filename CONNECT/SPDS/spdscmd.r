/*-----------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/exampletoolcmd.r $
|
+-----------------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include <Mstn\MdlApi\cmdclass.r.h>

#include "spdsids.h"

#define CT_NONE         0
#define CT_MAIN         1
#define CT_MODIFY       2 

#pragma suppressREQCmds

/*-----------------------------------------------------------------------------+
 Main command table
+-----------------------------------------------------------------------------*/
CommandTable CT_MAIN =
{ 
    { 1, CT_MODIFY,     MANIPULATION,   REQ,            "SPDS" },
};

CommandTable CT_MODIFY =
{
    { 1, CT_NONE,       INHERIT,        DEF,            "LEADER",         CMDNAME_LeaderTool },
    { 2, CT_NONE,       INHERIT,        DEF,            "LEADERTOOLSET",         CMDNAME_LeaderToolSet },
};

/*-----------------------------------------------------------------------------+
 Setup for native code only MDL app
+-----------------------------------------------------------------------------*/
#define DLLAPP_PRIMARY 1

DllMdlApp DLLAPP_PRIMARY =
{
    L"SPDS", L"spds" // taskid, dllName
}