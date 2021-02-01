/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/english/imodelvisitormsg.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   imodelvisitor  $
|   $Revision: 1.2 $
|   	$Date: 2013/06/28 20:38:36 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   imodelvisitor - imodelvisitor source code. |
|                                                                       |
+----------------------------------------------------------------------*/

/*----------------------------------------------------------------------+
|																		|
|   Include Files														|
|																		|
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <dlogids.h>

#include "..\imodelvisitor.h"

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
