/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/ToolCmd.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   toolExample  $
|   $Revision: 1.2 $
|   	$Date: 2013/06/28 20:30:23 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   toolExample - toolExample source code. |
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <cmdclass.h>

/*----------------------------------------------------------------------+
|									|
|   Local Defines							|
|									|
+----------------------------------------------------------------------*/
#define	    CT_NONE	    0
#define	    CT_MAIN	    1
#define     CT_ACTION	    2
#define     CT_ACTYPE        3
#define     CT_DIALOG       4

/*----------------------------------------------------------------------+
|                                                                       |
|   Application command syntax   					|
|                                                                       |
+----------------------------------------------------------------------*/
Table CT_MAIN =
{ 
    { 1, CT_ACTION, PLACEMENT, REQ,	"[!output SAFE_PROJECT_NAME]" }, 
}

Table CT_ACTION =
{ 
    { 1, CT_ACTYPE,       INHERIT,	NONE,      "ACTION" },
}

Table CT_ACTYPE =
{ 
    { 1, CT_NONE,       INHERIT,	NONE,      "MODIFY" },
    { 2, CT_NONE,       INHERIT,	NONE,      "PLACE" },
}

