/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/english/exampletoolmsg.r $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include <Mstn\MdlApi\rscdefs.r.h>
#include "testtoolids.h"

/*----------------------------------------------------------------------+
|    Command Names                                                      |
+----------------------------------------------------------------------*/
MessageList STRINGLISTID_Commands =
{
    {
    { CMDNAME_TestLStringTool,   "Пример рисования полилинии" },
    }
};

MessageList STRINGLISTID_Prompts =
{
    {
    { PROMPT_SelectView,            "Выбрать вид" },
    { PROMPT_FirstPoint,            "Укажите начальную точку" },
    { PROMPT_NextPoint,             "Укажите следующую точку" },
    { PROMPT_NextPointOrReset,      "Укажите следующую точку либо сброс для завершения" },
    }
};



