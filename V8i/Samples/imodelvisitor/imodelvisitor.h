/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/imodelvisitor.h,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   imodelvisitor  $
|   $Revision: 1.4 $
|   	$Date: 2013/06/28 20:38:35 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/

#if !defined(resource)
#include <Rpc.h>
#include    <mdl.h>
#include <GPArray.h>
#include <ElementGraphics.h>
#include <imodel\xmlinstanceschemamanager.h>
#include <IModel\xmlinstanceapi.h>
#include <IModel\stringlisthandle.h>
#include <IModel\imodelpublisher.h>
#include <IModel\imodelmessage.h>

#include    <MicroStationAPI.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <mselemen.fdf>
#include    <mselmdsc.fdf>
#include    <mslinkge.fdf>
#include    <msscancrit.fdf>
#include    <mstagdat.fdf>
#include    <mselems.h>
#include    <mscell.fdf>
#include    <leveltable.fdf>
#include    <mslstyle.fdf>
#include    <msstrlst.h>
#include    <mscnv.fdf>
#include    <msdgnobj.fdf>
#include    <msmodel.fdf>
#include    <msview.fdf>
#include    <msviewinfo.fdf>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msdialog.fdf>
#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>
#include	<toolsubs.h>
//scan code
#include	<elementref.h>
//locate code
#include	<msdependency.fdf>
#include	<msassoc.fdf>
#include	<msmisc.fdf>
#include	<mslocate.fdf>
#include	<msstate.fdf>
#include	<msoutput.fdf>
#include    <mskisolid.h>
#include    <mdlxmltools.fdf>
#include    <mskisolid.fdf>
#include    <mdlxmltoolslink.fdf>
#include    <msnamedgroup.fdf>
#include <msbsplin.fdf>
//place command
#include	<mstmatrx.fdf>


#define XMLHANDLER 1
#include "imodelvisitorCmd.h"
#include "MyElementGraphicsProcessor.h"
#include "XmlHelper.h"
#include "imodeltoolModifyTool.h"
#endif
/*----------------------------------------------------------------------+
|                                                                       |
|   imodelvisitor - imodelvisitor source code. |
|                                                                       |
+----------------------------------------------------------------------*/
#define DIALOGID_MDLDialog	    1
#define HOOKDIALOGID_MDLDialog	    (DIALOGID_MDLDialog * 100) + 1
#define DIALOGID_HostedDialog	    2
#define HOOKDIALOGID_HostedDialog   (DIALOGID_HostedDialog * 100) + 1



#define PUSHBUTTONID_ModalDialog	2
#define PUSHBUTTONID_ModelessDialog	3
#define PUSHBUTTONID_HostedDialog	4
#define PUSHBUTTONID_MDLButton		10
#define PUSHBUTTONID_DockableDialog	5
#define PUSHBUTTONID_ToolSettingsDialog	6

#define MESSAGELISTID_Commands	    1
#define MESSAGELISTID_Messages	    2
#define MESSAGELISTID_Prompts	    3
#define MESSAGELISTID_Misc	    4
#define	MESSAGEID_ResourceLoadError 5

#define MESSAGEID_ToolSettings  1
#define MESSAGEID_abc		2
#define COMMANDID_PlaceDate	3

#define PROMPTMESSAGEID_abc	1
#define PROMPTID_EnterPoint	2

#define MISCMESSAGEID_abc	1

 typedef struct certInfo
        {
        MSWChar   certName[512];
        MSWChar   recipient[512];
        }CertInfo;

  enum PackageOptionsEnum
       {
        GetRemoveModel = 1,
        CanExport      = 2,
        CanPrint       = 3,
        Password       = 4,
        Certificate    = 5,
        Expiration     = 6,
        License        = 7,
        PkgOptUnknown  = -1,
       };
  enum PublishOptionsEnum
      {
      FiterDesignHistory = 1,
      PublishDesignLinks = 2,
      RepublishAllFiles  = 3,
      CacheVisibleEdges  = 4,
      PubOptUnknown      = -1
      };

#if !defined(resource)
USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_USTN
USING_NAMESPACE_BENTLEY_USTN_ELEMENT
USING_NAMESPACE_BENTLEY_XMLINSTANCEAPI_NATIVE

#endif