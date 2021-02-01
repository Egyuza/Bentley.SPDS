/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/scanfile/scanfile.h,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Logfile:   J:/mdl/examples/scanfile/scanfile.h_v  $
|   $Workfile:   scanfile.h  $
|   $Revision: 1.3 $
|   	$Date: 2013/06/28 20:41:09 $
|									|
+----------------------------------------------------------------------*/
#if !defined  (__scanfileH__)
#define     __scanfileH__

/*----------------------------------------------------------------------+
|									|
|   Include Files   							|
|									|
+----------------------------------------------------------------------*/
#include <mselems.h>
#include <basetype.h>
#include <dlogitem.h>

#define FOUND       1
#define NOTFOUND    0

/*----------------------------------------------------------------------+
|									|
|   Message List IDs							|
|									|
+----------------------------------------------------------------------*/
#define MESSAGELISTID_Messages      1

/*----------------------------------------------------------------------+
|									|
|   Messages								|
|									|
+----------------------------------------------------------------------*/
#define MSGID_ErrorLoadingCmdTable  1
#define MSGID_ErrorAppendingLinkage 2
#define MSGID_LinkageAppended       3
#define MSGID_LinkageFound	    4
#define	MSGID_LinkageNotFound       5



#define  RSCID_DataDef_Link  1

#if !defined (resource)
typedef struct linkinfo
    {
    char	    string[4];
    } LinkInfo;

typedef struct linkdata
    {
    LinkageHeader  linkHdr;
    LinkInfo       linkageInfo;
    } LinkData;
#endif

#endif /* if !defined  (__scanfileH__) */
