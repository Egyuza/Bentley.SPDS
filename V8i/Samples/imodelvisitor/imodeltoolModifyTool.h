/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/imodeltoolModifyTool.h,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include    <WString.h>
#include    <mdl.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msdialog.fdf>

#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>
#include    <RefCounted.h>
#include    <toolsubs.h>

//scan code
#include <elementref.h>
//locate code
#include <msdependency.fdf>
#include <msassoc.fdf>
#include <msmisc.fdf>
#include <mslocate.fdf>
#include <msstate.fdf>
#include <msoutput.fdf>
//added for new interfaces in XM/Athens
#include <MicrostationAPI.h>
#include <mstypes.h>
#include <WString.h>
#include <imodel\xmlinstanceschemamanager.h>
#include <IModel\xmlinstanceapi.h>
#include <IModel\stringlisthandle.h>
#include <IModel\imodelpublisher.h>

//#pragma once
#include "imodelvisitorcmd.h"

USING_NAMESPACE_BENTLEY;
USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;

struct imodeltoolModifyTool :MstnElementSetTool
    {
    private:
    WString m_xmlInstanceData;
    //Bentley::XMLInstanceAPI::Native::XmlInstanceApiR m_api;
      
    public:
        virtual bool        WantAccuSnap () {return false;}
        virtual bool        NeedPointForDynamics () {return SOURCE_Pick == GetElemSource ();} // Can check GetHitSource to detect EditAction
        virtual bool        OnPostLocate (HitPathCP path, char *cantAcceptReason) override;
        virtual bool        OnModelNoMotion (MstnButtonEventCP) override;
        virtual StatusInt   OnElementModify (EditElemHandleR elHandle) override;
        virtual void        OnRestartCommand () override;
        virtual void        OnPostInstall () override;
        virtual EditElemHandleP BuildLocateAgenda (HitPathCP path, MstnButtonEventCP ev) override;
        virtual bool        OnInstall () override;
        virtual void        MyInstallTool();
         WString     GetFragmentString ();
                void        SetFragmentString (WString data);    
                void        SetAPIRef(Bentley::XMLInstanceAPI::Native::XmlInstanceApiR api);

    };
