/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/scanfile/scanfcmd.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Logfile:   J:/mdl/examples/scanfile/scanfcmd.r_v  $
|   $Workfile:   scanfcmd.r  $
|   $Revision: 1.4 $
|   	$Date: 2013/06/28 20:41:09 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   scancmd.r - Scanfile example source code.				|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <dlogbox.h>
#include <cmdlist.h>
#include <cmdclass.h>

/*----------------------------------------------------------------------+
| 
|  Register Application and DLL
| 
+----------------------------------------------------------------------*/
#define  DLLAPP_SCANFILE        1

DllMdlApp DLLAPP_SCANFILE =
    {
    "SCANFILE", "scanfile"          // taskid, dllName
    }



/* Version 100 */

/*----------------------------------------------------------------------+
|									|
|   Command Table Definitions						|
|									|
+----------------------------------------------------------------------*/
#define  CT_NONE		0
#define  CT_MAIN		1
#define  CT_LINKAGE		2
#define  CT_SCAN		3

Table CT_MAIN =
    {
        {1, CT_LINKAGE, PLACEMENT, REQ, "linkage"},
        {1, CT_SCAN, PLACEMENT, REQ, "scan"}

    };
Table CT_LINKAGE =
    {
        {2, CT_NONE, PLACEMENT, REQ, "extract"},
        {3, CT_NONE, PLACEMENT, REQ, "attach"}
    };
Table CT_SCAN =
    {
        {1, CT_NONE, PLACEMENT, REQ, "linkage"},
    };
