/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/english/exampletoolmsg.r $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include "exampletoolids.h"

/*----------------------------------------------------------------------+
|    Command Names                                                      |
+----------------------------------------------------------------------*/
MessageList STRINGLISTID_Commands =
{
    {
    { CMDNAME_ExampleMoveTool,      "Move Element Example" },
    { CMDNAME_ExampleModifyTool,    "Modify Element Example" },
    { CMDNAME_ExampleFloodTool,     "Flood Element Example" },
    { CMDNAME_ExampleViewTool,      "Un-rotate View Example" },
    { CMDNAME_ExampleLineTool,      "Place Line Example" },
    { CMDNAME_ExampleLStringTool,   "Place Line String Example" },
    }
};

MessageList STRINGLISTID_Prompts =
{
    {
    { PROMPT_SelectView,            "Select view" },
    { PROMPT_FirstPoint,            "Enter first point" },
    { PROMPT_NextPoint,             "Enter next point" },
    { PROMPT_NextPointOrReset,      "Enter next point or reset to complete" },
    }
};



