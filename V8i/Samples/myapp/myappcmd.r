/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/myapp/myappcmd.r,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#pragma suppressREQCmds
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   myappcmd.r  $
|   $Revision: 1.4 $
|   	$Date: 2013/06/28 20:40:47 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   myappcmd.r - MyApp Command Table hierarchy 	    	    	    	|
|									|
+----------------------------------------------------------------------*/
#include <rscdefs.h>
#include <cmdclass.h>

/*----------------------------------------------------------------------+
| 
|  Register Application and DLL
| 
+----------------------------------------------------------------------*/
#define  DLLAPP_MYAPP        1

DllMdlApp DLLAPP_MYAPP =
    {
    "MYAPP", "myapp"          // taskid, dllName
    }


/*----------------------------------------------------------------------+
|									|
|   Local Defines							|
|									|
+----------------------------------------------------------------------*/
#define	CT_NONE	    0
#define	CT_MAIN	    1
#define	CT_PLACE    2
#define	CT_CHANGE   3
#define	CT_OPEN     4
#define	CT_SETTING  5
#define CT_ONOFF    6

/*----------------------------------------------------------------------+
|									|
|   Table <tableId> =    						|
|   {									|
|      	{ <no.>, <subTableId>, <cmdClass>, <options>, "<cmdWord>" };	|
|   };									|
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   MyApp commands							|
|                                                                       |
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Main (Root) command word table					|
|                                                                       |
+----------------------------------------------------------------------*/
Table	CT_MAIN =
{ 
    { 1,  CT_PLACE,	PLACEMENT,	REQ,		"Place" }, 
    { 2,  CT_CHANGE,	MANIPULATION,	REQ,		"Change" }, 
    { 3,  CT_OPEN,	DIALOGOPEN,	REQ | HID,	"Open" }, 
    { 4,  CT_SETTING,	PARAMETERS,	REQ | HID,	"MyappSetting" }, 
};

/*----------------------------------------------------------------------+
|                                                                       |
|   CMD_PLACE subtable							|
|                                                                       |
+----------------------------------------------------------------------*/
Table	CT_PLACE =
{
    { 1,  CT_NONE, 	INHERIT, 	NONE, 		"MyLine" },
    { 2,  CT_NONE, 	INHERIT, 	NONE, 		"MyDate" }, 
};

/*----------------------------------------------------------------------+
|                                                                       |
|   CMD_CHANGE subtable							|
|                                                                       |
+----------------------------------------------------------------------*/
Table	CT_CHANGE =
{ 
    { 1,  CT_NONE, 	INHERIT, 	NONE, 		"MySymbology" }, 
};
	
/*----------------------------------------------------------------------+
|                                                                       |
|   CT_OPEN subtable							|
|                                                                       |
+----------------------------------------------------------------------*/
Table	CT_OPEN =
{ 
    { 1,  CT_NONE, 	INHERIT, 	NONE, 		"MyToolFrame" }, 
    { 2,  CT_NONE, 	INHERIT, 	NONE, 		"MyMainDialog" }, 
};

/*----------------------------------------------------------------------+
|                                                                       |
|   CT_SETTING subtable							|
|                                                                       |
+----------------------------------------------------------------------*/
Table	CT_SETTING =
{ 
    { 1,  CT_NONE, 	INHERIT, 	NONE, 		"LineStyle" }, 
    { 2,  CT_NONE, 	INHERIT, 	NONE, 		"Date" }, 
    { 3,  CT_ONOFF, 	INHERIT, 	NONE, 		"UseFence" }, 
};

/*----------------------------------------------------------------------+
|                                                                       |
|   CT_ONOFF subtable							|
|                                                                       |
+----------------------------------------------------------------------*/
Table	CT_ONOFF =
{ 
    { 1,  CT_NONE, 	INHERIT, 	NONE, 		"On" }, 
    { 2,  CT_NONE, 	INHERIT, 	NONE, 		"Off" }, 
    { 3,  CT_NONE, 	INHERIT, 	DEF, 		"Toggle" }, 
};
	    	    


