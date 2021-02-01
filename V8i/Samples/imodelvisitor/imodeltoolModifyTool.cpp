/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/imodeltoolModifyTool.cpp,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "imodeltoolModifyTool.h"
WString          imodeltoolModifyTool::GetFragmentString()
    {
    return  m_xmlInstanceData;
    }

void             imodeltoolModifyTool::SetFragmentString (WString data)
    {
    m_xmlInstanceData = data;
    }
/*--------------------------------------------------------------------------**//**
* @description This function consolodates the calls to get the api.    
* @param  if a specific model ref is required.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
Private Bentley::XMLInstanceAPI::Native::XmlInstanceApi  GetAPI(Bentley::XMLInstanceAPI::Native::XmlInstanceStatus instStatus)
    {
    Bentley::XMLInstanceAPI::Native::XmlInstanceSchemaManager  schemaManager;
       
    schemaManager = Bentley::XMLInstanceAPI::Native::XmlInstanceSchemaManager();

    Bentley::XMLInstanceAPI::Native::XmlInstanceApi apiRef( Bentley::XMLInstanceAPI::Native::XmlInstanceApi::CreateApi(instStatus,schemaManager));
   
    return apiRef;
    }


void imodeltoolModifyTool::SetAPIRef (Bentley::XMLInstanceAPI::Native::XmlInstanceApiR api)
    {
    //m_api=api;
    }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            imodeltoolModifyTool::OnPostLocate (HitPathCP path, char *cantAcceptReason)
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
StatusInt       imodeltoolModifyTool::OnElementModify (EditElemHandleR eeh)
    {
    // Element from Fence or Selection Set agenda...could exclude using FilterAgendaEntries...
//return ERROR to not change the element.
      //  return ERROR;
    Bentley::XMLInstanceAPI::Native::XmlInstanceStatus instStatus;
    Bentley::XMLInstanceAPI::Native::XmlInstanceApi apiRef=GetAPI(instStatus);
    apiRef.CreateInstance(instStatus,eeh.GetElemRef(),m_xmlInstanceData);
    
    if (instStatus.status !=  Bentley::XMLInstanceAPI::Native::XML_INSTANCE_SUCCESS )
        return ERROR;
    else
        return SUCCESS;
    }


/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            imodeltoolModifyTool::OnModelNoMotion (MstnButtonEvent const *ev) 
  {
    return true;
  }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
void            imodeltoolModifyTool::OnRestartCommand ()
    {
    imodeltoolModifyTool* newTool = new imodeltoolModifyTool (*this);

    newTool->InstallTool ();
    }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
EditElemHandleP  imodeltoolModifyTool::BuildLocateAgenda (HitPathCP path, MstnButtonEventCP ev)
    {
    // Here we have both the new agenda entry and the current hit path if needed...
    EditElemHandleP elHandle = __super::BuildLocateAgenda (path, ev);

    return  elHandle;
    }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
void            imodeltoolModifyTool::OnPostInstall ()
    {
    __super::OnPostInstall ();
    mdlAccuDraw_setEnabledState (false); // Don't enable AccuDraw w/Dynamics...
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
bool            imodeltoolModifyTool::OnInstall ()
    {
    if (!__super::OnInstall ())
        return false;

    SetCmdNumber (0);      // For toolsettings/undo string...
    SetCmdName (0, 0);  // For command prompt...

    return true;
    }

void    imodeltoolModifyTool::MyInstallTool()
    {
    InstallTool();
    }

