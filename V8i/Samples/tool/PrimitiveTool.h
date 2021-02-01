/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/PrimitiveTool.h,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/
#include    <mdl.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <mselemen.fdf>
#include    <mselmdsc.fdf>
#include    <mslinkge.fdf>
#include    <mselems.h>
#include    <msdgnobj.fdf>
#include    <msmodel.fdf>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>
#include <toolsubs.h>

//scan code
#include <elementref.h>
//locate code
#include <msstate.fdf>
#include <msoutput.fdf>
//added for new interfaces in XM/Athens
#include <microstationapi.h>
#include <mstypes.h>

//place command
#include <mstmatrx.fdf>

#include "[!output SAFE_PROJECT_NAME]ToolCmd.h"
//#include "toolExample.h"
USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;

/*=================================================================================**//**
* Simple placement tool to create a text element by 1 point.
* @bsiclass
+===============+===============+===============+===============+===============+======*/
struct [!output SAFE_PROJECT_NAME]PrimitiveTool:MstnPrimitiveTool
    {
    protected:
        DPoint3d m_origin;
        bool     m_haveOrigin;
    public:
        char     *m_str;

        [!output SAFE_PROJECT_NAME]PrimitiveTool (int cmdNumber, int cmdName):MstnPrimitiveTool ( cmdNumber,  cmdName,0){m_haveOrigin=true;}
        virtual bool OnInstall () override;
        virtual void OnPostInstall() override;
        virtual void OnCleanup() override;
        virtual void OnRestartCommand() override;

        virtual void OnComplexDynamics(MstnButtonEventCP ev);
        virtual bool OnResetButton (MstnButtonEventCP ev) override {OnReinitialize (); return true;}
        virtual bool OnDataButton (MstnButtonEventCP ev) override;

        virtual bool CreateElement (EditElemHandleR eeh, DPoint3dR origin,MstnButtonEventCP ev);
        //created this method since the InstallTool is protected and only accessable with in the class.
        virtual void MyInstallTool();
    };