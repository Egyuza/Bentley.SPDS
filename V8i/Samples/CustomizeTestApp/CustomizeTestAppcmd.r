/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/CustomizeTestApp/CustomizeTestAppcmd.r,v $
|    $RCSfile: CustomizeTestAppcmd.r,v $
|   $Revision: 1.1 $
|       $Date: 2008/06/02 15:09:34 $
|     $Author: Mark.Anderson $
|
|  $Copyright: (c) 2008 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "rscdefs.h"
#include "cmdclass.h"
#include <CustomizeTestAppids.h>

#define CT_NONE             0
#define CT_MAIN             1
#define CT_CUSTOMIZE        2
#define CT_TEST             3

#pragma suppressREQCmds

/*----------------------------------------------------------------------+
|                                                                       |
| Main command table                                                    |
|                                                                       |
+----------------------------------------------------------------------*/
Table CT_MAIN =
{
    { 1, CT_CUSTOMIZE,  SHOW,      REQ,        "CUSTOMIZE"},
};

Table CT_CUSTOMIZE =
{
    { 1, CT_TEST,  SHOW,      REQ,        "TEST"},
};

Table CT_TEST =
{
    { 1, CT_NONE,  INHERIT,      NONE,        "TEMPLATES"},
    { 2, CT_NONE,  INHERIT,      NONE,        "EXPRESSIONS"},
};
                                                                                
/*-----------------------------------------------------------------------
 Setup for native code only MDL app
-----------------------------------------------------------------------*/
#define  DLLAPP_PRIMARY     1

DllMdlApp   DLLAPP_PRIMARY =
    {
    "CustomizeTestApp", "CustomizeTestApp"                        // taskid, dllName
    }