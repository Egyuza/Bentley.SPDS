/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/adrwdemo/english/adrwdmsg.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|    $Logfile:   J:/mdl/examples/adrwdemo/english/adrwdmsg.r_v  $
|   $Workfile:   adrwdmsg.r  $
|   $Revision: 1.3 $
|   	$Date: 2013/06/28 20:33:45 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|	    Message List for Adrwdemo Application			|
+----------------------------------------------------------------------*/
#include "rscdefs.h"
#include "cmdclass.h"
#include "adrwdemo.h"

MessageList MSGLIST_commands =
{
    {
    {MSG_cmdLine,	    "Line"			},
    {MSG_cmdCircle,	    "Circle"			},
    {MSG_cmdRect,	    "Rectangle"			},
    {MSG_cmdRect2,	    "Rotated Rectangle"		},
    {MSG_cmdChange,	    "Change Circle"		},
    }
};

MessageList MSGLIST_prompts =
{
    {
    {MSG_promptEnterPoint, 	"Enter point"		},
    {MSG_promptAcceptReject, 	"Accept or Reject"	},
    }
};
