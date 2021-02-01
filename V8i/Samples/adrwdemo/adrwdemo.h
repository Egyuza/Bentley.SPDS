/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/adrwdemo/adrwdemo.h,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   adrwdemo.h	 Adrwdemo header file					|
|									|
+----------------------------------------------------------------------*/
#if !defined (__adrwdH__)
#define __adrwdH__

/*----------------------------------------------------------------------+
|									|
|   Include Files							|
|									|
+----------------------------------------------------------------------*/
#define     DIALOGID_Pal		1

/*----------------------------------------------------------------------+
|									|
|   Item IDs								|
|									|
+----------------------------------------------------------------------*/
#define     TOGGLEID_ShowAxes		1
#define     TOGGLEID_UseCPlane		2

/*----------------------------------------------------------------------+
|									|
|   Message List ID's							|
|									|
+----------------------------------------------------------------------*/
#define     MSGLIST_commands		0
#define     MSG_cmdLine 		1
#define     MSG_cmdCircle		2
#define     MSG_cmdRect 		3
#define     MSG_cmdRect2		4
#define     MSG_cmdChange		5

#define     MSGLIST_prompts		100
#define     MSG_promptEnterPoint	101
#define     MSG_promptAcceptReject	102

/*----------------------------------------------------------------------+
|									|
|   Palette Definitions 						|
|									|
+----------------------------------------------------------------------*/
#define     ICONCMDFRAMEID_Frame	1

#define     ICONCMDID_Line		1
#define     ICONCMDID_Circle		2
#define     ICONCMDID_Rectangle 	3
#define     ICONCMDID_Rectangle2	4
#define     ICONCMDID_ChangeCircle	5

/*----------------------------------------------------------------------+
|									|
|   Local Structure Definitions 					|
|									|
+----------------------------------------------------------------------*/
typedef struct adrwdemoinfo
    {
    int     showAxes;
    int     useCPlane;
    } AdrwdemoInfo;

#endif /* !defined (__adrwdH__) */
