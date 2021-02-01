/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/exampletoolcmd.r $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include <Mstn\MdlApi\cmdclass.r.h>

#include "testtoolids.h"

#define CT_NONE         0
#define CT_MAIN         1
#define CT_MODIFY       2 

#pragma suppressREQCmds

/*----------------------------------------------------------------------+
 Main command table
+----------------------------------------------------------------------*/
CommandTable CT_MAIN =
    { 
    { 1, CT_MODIFY,     MANIPULATION,   REQ,            "TEST" },
    };

CommandTable CT_MODIFY =
    {
    { 1, CT_NONE,       INHERIT,        DEF,            "LSTRINGTOOL",         CMDNAME_TestLStringTool },
    };

/*-----------------------------------------------------------------------
 Setup for native code only MDL app
-----------------------------------------------------------------------*/
#define DLLAPP_PRIMARY 1

DllMdlApp DLLAPP_PRIMARY =
    {
    L"TESTTOOL", L"testtool" // taskid, dllName
    }