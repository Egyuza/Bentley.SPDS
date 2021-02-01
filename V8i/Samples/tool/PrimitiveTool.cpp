/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/PrimitiveTool.cpp,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "[!output SAFE_PROJECT_NAME]PrimitiveTool.h"
//clean up when this instance is done (another command has been fired)
        void [!output SAFE_PROJECT_NAME]PrimitiveTool::OnCleanup ()
            {
            dlmSystem_mdlFree (m_str);
            }
//what to do when the command is restarted
        void [!output SAFE_PROJECT_NAME]PrimitiveTool::OnRestartCommand ()
            {
            [!output SAFE_PROJECT_NAME]PrimitiveTool    *pTool = new [!output SAFE_PROJECT_NAME]PrimitiveTool(GetCmdNumber(),GetCmdName());
            pTool->m_str = dlmSystem_strdup("test");
            pTool->InstallTool();
            }
//creates the element.  This is used in the dynamics
// so let is manage the descriptor
        bool [!output SAFE_PROJECT_NAME]PrimitiveTool::CreateElement (EditElemHandleR eeh, DPoint3dR origin,MstnButtonEventCP ev)
            {
            MSElement       textEl;
            MSElementDescr  *pText;

            mdlText_create (&textEl,NULL,m_str,&origin,NULL,NULL,NULL,NULL);
            mdlElmdscr_new (&pText,NULL,&textEl);
            pText->h.dgnModelRef = ACTIVEMODEL;
            eeh.SetElemDescr (pText,true,false);//the second parameter lets the destructor free the descriptor (way cool I don't have to remember to free this now).
            return eeh.IsValid();
            }
//what to do on the data point
        bool [!output SAFE_PROJECT_NAME]PrimitiveTool::OnDataButton (MstnButtonEventCP ev)
            {
            EditElemHandle  eeh;
            m_origin = *ev->GetPoint();
            m_haveOrigin = true;

            if (CreateElement(eeh,m_origin,ev))
                eeh.AddToModel(ACTIVEMODEL);

            OnReinitialize();

            return true;
            }
//handle the dynamics (for XM and beyond complex are the default way to go)
        void [!output SAFE_PROJECT_NAME]PrimitiveTool::OnComplexDynamics (MstnButtonEventCP ev)
            {
            EditElemHandle eeh;
            DPoint3d    pt;

            pt = *ev->GetPoint();
            if (!CreateElement (eeh,pt,ev))
                return;

            //printf ("should be in dynamics %s \n",m_str);
            RedrawElems redrawTool;
            redrawTool.SetDrawMode (DRAW_MODE_TempDraw);
            redrawTool.SetDrawPurpose(DRAW_PURPOSE_Dynamics);
            redrawTool.SetViews (0xffff);
            redrawTool.DoRedraw (eeh);

            }
//after the tool is installed 
        void [!output SAFE_PROJECT_NAME]PrimitiveTool::OnPostInstall ()
            {
            //I can start dynamics since only one point is necessary for text placement.
             BeginComplexDynamics ();
            __super::OnPostInstall();
            }
//the tool is being installed are there any tasks to do?
        bool [!output SAFE_PROJECT_NAME]PrimitiveTool::OnInstall ()
            {
            m_haveOrigin = true;
            return __super::OnInstall();
            }
//a public method I made to install this tool
        void [!output SAFE_PROJECT_NAME]PrimitiveTool::MyInstallTool()
            {
            InstallTool();
            }