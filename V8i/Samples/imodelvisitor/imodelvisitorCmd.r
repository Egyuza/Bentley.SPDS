/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/imodelvisitorCmd.r,v $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   imodelvisitor  $
|   $Revision: 1.6 $
|   	$Date: 2015/09/02 06:12:36 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   imodelvisitor - imodelvisitor source code. |
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
#define     CT_SETACT       5
#define     CT_EDIT         6
#define     CT_FORWARDCMD   7

/*----------------------------------------------------------------------+
|                                                                       |
|   Application command syntax   					|
|                                                                       |
+----------------------------------------------------------------------*/
Table CT_MAIN =
{ 
    { 1, CT_ACTION,       REPORTCMD,  REQ,	"IMODELVISITOR" }, 
    { 2, CT_FORWARDCMD,       REPORTCMD, REQ,      "ImodelStart"   },
}

Table CT_ACTION =
{ 
    { 1, CT_ACTYPE,       INHERIT,	NONE,      "ACTION" },
    { 2, CT_SETACT,       INHERIT,  NONE,      "SET"},
    { 3, CT_EDIT,         INHERIT,   NONE,     "ADD"},
}

Table CT_ACTYPE =
{ 
    { 1, CT_NONE,       INHERIT,	NONE,      "PROCESS" },
    { 2, CT_NONE,       INHERIT,    NONE,      "PUBLISH"},
    { 3, CT_NONE,       INHERIT,    NONE,      "PACKAGE"},
    { 4, CT_NONE,       INHERIT,    NONE,      "ADDSCHEMA"},
    { 5, CT_NONE,       INHERIT,    NONE,      "REPORT"},
    { 6, CT_NONE,       INHERIT,    NONE,      "OFFLINE"},
    { 7, CT_NONE,       INHERIT,    NONE,      "PUBLISHDGNDB"},
}

Table CT_SETACT = 
{
    {1,  CT_NONE,    INHERIT,   NONE,   "PACKAGEOPTS"},
    {2,  CT_NONE,    INHERIT,   NONE,   "PUBOPTS"},
    {3,  CT_NONE,    INHERIT, NONE, "PROCALL"},
}

Table CT_EDIT = 
{
    { 1, CT_NONE,   INHERIT,   NONE,  "ATTRIB"},
}

Table  CT_FORWARDCMD = 
{
    {1, CT_NONE,    INHERIT, NONE, "CMD" },
}