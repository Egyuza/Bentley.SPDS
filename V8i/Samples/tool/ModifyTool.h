/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/ModifyTool.h,v $
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
#include <microstationapi.h>
#include <mstypes.h>

//#pragma once
#include "[!output SAFE_PROJECT_NAME]ToolCmd.h"

USING_NAMESPACE_BENTLEY;
USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;

struct [!output SAFE_PROJECT_NAME]ModifyTool :MstnElementSetTool
    {
    private:

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
    };
