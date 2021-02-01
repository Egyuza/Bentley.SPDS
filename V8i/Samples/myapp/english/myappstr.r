/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/myapp/english/myappstr.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   myappstr.r  $
|   $Revision: 1.4 $
|   	$Date: 2013/06/28 20:40:50 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   myappstr.r - MyApp Message list resource   	    	    		|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <rscdefs.h>

#include "myapp.h"

/*----------------------------------------------------------------------+
|									|
|   Messages List Resource Definition					|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|								        |
|   The Message List resource structure is defined as follows:		|
|									|
|   #if defined (resource)						|
|   	typedef struct __messagelist__					|
|   	    {								|
|   	    ULong	1;   # No. expected infoFields per string.	|
|   	    struct messages						|
|   	    	{							|
|   	    	ULong   infoFields[];					|
|       	char    msg[];						|
|       	} Messages [];						|
|	    } MessageList;						|
|									|
+----------------------------------------------------------------------*/
MessageList MESSAGELISTID_Commands =
{
    {
    {COMMANDID_PlaceMyLine, 		"Place My Line"},
    {COMMANDID_PlaceDate, 		"Place My Date"},
    {COMMANDID_ChangeSymbology,		"Change My Symbology"},
    {COMMANDID_ChangeSymbologyFence,	"Change My Symbology of Fence Contents"},
    }
};

MessageList MESSAGELISTID_Prompts =
{
    {
    {PROMPTID_EnterFirst, 		"Enter first point"},
    {PROMPTID_EnterNext, 		"Enter next point"},
    {PROMPTID_EnterPoint, 		"Enter data point"},
    {PROMPTID_AcceptFence, 		"Accept/Reject Fence Contents"},
    {PROMPTID_IdentifyElt,		"Identify element"},
    }

};

MessageList MESSAGELISTID_Msgs =
{
    {
    {MSGID_LoadCmdTbl, 		"Unable to load command table"},
    {MSGID_DialogOpen, 		"Unable to open main dialog"},
    {MSGID_DateString, 		"MM:DD:YYYY"},
    {MSGID_RscOpenErr, 		"Unable to open resource file"},
    {MSGID_SureString,		"Are you sure?"},
    {MSGID_PaletteOpen,		"Unable to open main palette"},
    {MSGID_NoFence,		"No fence defined"},
    {MSGID_InvalidLineStyle,	"Invalid line style value"},
    }

};

