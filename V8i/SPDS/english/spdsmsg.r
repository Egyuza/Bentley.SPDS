/*----------------------------------------------------------------------+
|																		|
|   Include Files														|
|																		|
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <dlogids.h>

#include "..\spds.h"

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
