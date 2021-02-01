/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/exampletoolcmd.r $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include <Mstn\MdlApi\cmdclass.r.h>

#include "exampletoolids.h"

#define CT_NONE         0
#define CT_MAIN         1
#define CT_MODIFY       2 

#pragma suppressREQCmds

/*----------------------------------------------------------------------+
 Main command table
+----------------------------------------------------------------------*/
CommandTable CT_MAIN =
    { 
    { 1, CT_MODIFY,     MANIPULATION,   REQ,            "EXAMPLE" },
    };

CommandTable CT_MODIFY =
    {
    { 1, CT_NONE,       INHERIT,        DEF,            "MOVETOOL",         CMDNAME_ExampleMoveTool },
    { 2, CT_NONE,       INHERIT,        DEF,            "MODIFYTOOL",       CMDNAME_ExampleModifyTool },
    { 3, CT_NONE,       INHERIT,        DEF,            "FLOODTOOL",        CMDNAME_ExampleFloodTool },
    { 4, CT_NONE,       INHERIT,        DEF,            "VIEWTOOL",         CMDNAME_ExampleViewTool },
    { 5, CT_NONE,       INHERIT,        DEF,            "LINETOOL",         CMDNAME_ExampleLineTool },
    { 6, CT_NONE,       INHERIT,        DEF,            "LSTRINGTOOL",      CMDNAME_ExampleLStringTool },
    };

/*-----------------------------------------------------------------------
 Setup for native code only MDL app
-----------------------------------------------------------------------*/
#define DLLAPP_PRIMARY 1

DllMdlApp DLLAPP_PRIMARY =
    {
    L"EXAMPLETOOL", L"exampletool" // taskid, dllName
    }