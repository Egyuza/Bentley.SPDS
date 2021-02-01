/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/adrwdemo/adrwdcmd.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|    Include Files							|
|									|
+----------------------------------------------------------------------*/
#pragma		suppressREQCmds

#include 	<rscdefs.h>
#include 	<cmdclass.h>

#include	"adrwdemo.h"
/*----------------------------------------------------------------------+
| 
|  Register Application and DLL
| 
+----------------------------------------------------------------------*/
#define  DLLAPP_ADRWDEMO        1

DllMdlApp DLLAPP_ADRWDEMO =
    {
    "ADRWDEMO", "adrwdemo"          // taskid, dllName
    }


/*----------------------------------------------------------------------+
|									|
|   Immediate Defines							|
|									|
+----------------------------------------------------------------------*/
#define CT_NONE 		    0
#define CT_MAIN 		    1
#define CT_DEMO			    2

/*----------------------------------------------------------------------+
|									|
|   Command Tables							|
|									|
+----------------------------------------------------------------------*/
Table	CT_MAIN =
{
{ 1, CT_DEMO,     PLACEMENT,	    REQ,		    "DEMO"	}
};

Table CT_DEMO =
{
 { 1, CT_NONE,	    INHERIT, NONE|CMDSTR(MSG_cmdLine),      "LINE"      },
 { 2, CT_NONE,	    INHERIT, NONE|CMDSTR(MSG_cmdCircle),    "CIRCLE"    },
 { 3, CT_NONE,	    INHERIT, NONE|CMDSTR(MSG_cmdRect),      "RECTANGLE" },
 { 4, CT_NONE,	    INHERIT, NONE|CMDSTR(MSG_cmdRect2),     "RECTANGLE2"},
 { 5, CT_NONE,	    INHERIT, NONE|CMDSTR(MSG_cmdChange),    "CHANGE"    },
};


