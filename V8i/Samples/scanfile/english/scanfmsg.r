/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/scanfile/english/scanfmsg.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   scanfmsg.r  - Application's language specific resource definitions.	|
|									|
|   $Logfile:   J:/mdl/examples/scanfile/english/scanfmsg.r_v  $
|   $Workfile:   scanfmsg.r  $
|   $Revision: 1.3 $
|      	$Date: 2013/06/28 20:41:10 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <rscdefs.h>

#include "scanfile.h"

/*----------------------------------------------------------------------+
|									|
|   Messages								|
|									|
+----------------------------------------------------------------------*/
MessageList  MESSAGELISTID_Messages =
    {
{

{ MSGID_ErrorLoadingCmdTable,	"Error loading command table" },
{ MSGID_ErrorAppendingLinkage,	"Error appending linkage" },
{ MSGID_LinkageAppended,	"Linkage successfully appended" },
{ MSGID_LinkageFound,       	"Linkage found" },
{ MSGID_LinkageNotFound,	"Linkage not found" },

}
    };

