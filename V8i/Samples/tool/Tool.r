/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/Tool.r,v $
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
#include <rscdefs.h>


#define     DLLAPPID 1

/* associate app with dll */
DllMdlApp DLLAPPID = 
    {
    "[!output SAFE_PROJECT_NAME]Tool", "[!output SAFE_PROJECT_NAME]Tool"
    }

