/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/ToolMsg.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   toolExample  $
|   $Revision: 1.2 $
|   	$Date: 2013/06/28 20:30:24 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   toolExample - toolExample source code. |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
|																		|
|   Include Files														|
|																		|
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <dlogids.h>

#include "..\[!output SAFE_PROJECT_NAME]Tool.h"

/*----------------------------------------------------------------------+
|																		|
|   Messages List Resource Definition									|
|																		|
+----------------------------------------------------------------------*/

MessageList MESSAGELISTID_Commands =
{
    {
    {MESSAGEID_ToolSettings,	"Native Tool Settings Content"},
    {COMMANDID_PlaceDate,		"Place My Date"},
    }
};


MessageList MESSAGELISTID_Messages =
{
    {
    {MESSAGEID_abc,					""},
	{MESSAGEID_ResourceLoadError,	"Unable to open resource file"},
    }
};


MessageList MESSAGELISTID_Prompts =
{
    {
    {PROMPTMESSAGEID_abc,	""},
    {PROMPTID_EnterPoint,	"Enter data point"},
    }
};

MessageList MESSAGELISTID_Misc =
{
    {
    {MISCMESSAGEID_abc,		""},
    }
};
