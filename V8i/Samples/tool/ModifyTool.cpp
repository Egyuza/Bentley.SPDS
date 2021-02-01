/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/vault/VisualStudioWizards/MSTNToolWizard/Templates/1033/ModifyTool.cpp,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "[!output SAFE_PROJECT_NAME]ModifyTool.h"

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            [!output SAFE_PROJECT_NAME]ModifyTool::OnPostLocate (HitPathCP path, char *cantAcceptReason)
    {
    if (!__super::OnPostLocate (path, cantAcceptReason))
        return false;

    // Only allow elements of our type to be located...
    ElemHandle  eh (mdlDisplayPath_getElem ((DisplayPathP) path, 0), mdlDisplayPath_getPathRoot ((DisplayPathP) path));

    return true;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
StatusInt       [!output SAFE_PROJECT_NAME]ModifyTool::OnElementModify (EditElemHandleR eeh)
    {
    // Element from Fence or Selection Set agenda...could exclude using FilterAgendaEntries...
//return ERROR to not change the element.
      //  return ERROR;

    return SUCCESS;
    }


/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            [!output SAFE_PROJECT_NAME]ModifyTool::OnModelNoMotion (MstnButtonEvent const *ev) 
  {
    return true;
  }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
void            [!output SAFE_PROJECT_NAME]ModifyTool::OnRestartCommand ()
    {
    [!output SAFE_PROJECT_NAME]ModifyTool* newTool = new [!output SAFE_PROJECT_NAME]ModifyTool ();

    newTool->InstallTool ();
    }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
EditElemHandleP  [!output SAFE_PROJECT_NAME]ModifyTool::BuildLocateAgenda (HitPathCP path, MstnButtonEventCP ev)
    {
    // Here we have both the new agenda entry and the current hit path if needed...
    EditElemHandleP elHandle = __super::BuildLocateAgenda (path, ev);

    return  elHandle;
    }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
void            [!output SAFE_PROJECT_NAME]ModifyTool::OnPostInstall ()
    {
    __super::OnPostInstall ();
    mdlAccuDraw_setEnabledState (false); // Don't enable AccuDraw w/Dynamics...
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            [!output SAFE_PROJECT_NAME]ModifyTool::OnInstall ()
    {
    if (!__super::OnInstall ())
        return false;

    SetCmdNumber (0);      // For toolsettings/undo string...
    SetCmdName (0, 0);  // For command prompt...

    return true;
    }

void    [!output SAFE_PROJECT_NAME]ModifyTool::MyInstallTool()
    {
    InstallTool();
    }

