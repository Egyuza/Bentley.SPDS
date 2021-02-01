/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/imodelvisitor.cpp,v $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#define DEBUG 1
#define PRINTLOG_STATE 0
#define COEDGESOLUTION 1
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   imodelvisitor  $
|   $Revision: 1.11 $
|   	$Date: 2015/09/02 06:12:36 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   imodelvisitor - imodelvisitor source code. |
|                                                                       |
+----------------------------------------------------------------------*/

#include "imodelvisitor.h"

//function not in fdf but in the kisolid.lib for getting the underlying entity for the solid.
USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_USTN
USING_NAMESPACE_BENTLEY_USTN_ELEMENT
USING_NAMESPACE_BENTLEY_XMLINSTANCEAPI_NATIVE

namespace BXN       =  Bentley::XMLInstanceAPI::Native;

#define DIM(a)      (  (sizeof a) / sizeof *(a)  )

int     g_debugLevel = PRINTLOG_STATE;
//set this to handle dgn or just i-models  true all false only i-model.
static bool g_bProcessAll = true;

 //static  MSWChar m_graphicXMLFile[MAXFILELENGTH];
/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                    BSI    09/01
+---------------+---------------+---------------+---------------+---------------+------*/
Private void    dumpString
(
char *poo
)
    {
    printf ("%s \n",poo);
    }
#if defined (NO_COMMON_GOOD)
/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                    BSI    09/01
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C"  void     log_printf
(
long  level,
char  *szFormat,
...
)
    {
    va_list     arg;
    char        szBuf[512];
    static int   count;
    
    if (level != g_debugLevel)
        return;

    va_start (arg, szFormat);
    vsprintf (szBuf, szFormat, arg);

    dumpString (szBuf);
    }
// Public static XmlDomRef m_graphicDom;
#endif

/**
* A simple class to collect up all of the xml processing in this sample.
*/
class BusinessDataHandler
    {
    XmlNodeRef      m_rootNodePtr;
    XmlDomRef       m_pXMLDom;
    Bentley::XMLInstanceAPI::Native::XmlInstanceApiR  m_apiRef;
    Bentley::XMLInstanceAPI::Native::XmlInstanceSchemaManager m_schemaManager;

public:

    BusinessDataHandler ( Bentley::XMLInstanceAPI::Native::XmlInstanceApi&  apiRef,MSWChar* rootElementName):m_apiRef(apiRef)
    {
    int status;
    //handle the xml portion.
    XmlHelper::CreateDomInstance (&m_pXMLDom);
    status = mdlXMLDom_createElement (&m_rootNodePtr,m_pXMLDom,rootElementName);
    status = mdlXMLDom_appendChild (m_pXMLDom,m_rootNodePtr);
    status = mdlXMLDom_setRootElement (m_pXMLDom,m_rootNodePtr);
    //get the xmlinstanceapi
    m_schemaManager  = Bentley::XMLInstanceAPI::Native::XmlInstanceSchemaManager ();
    }
/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
XmlNodeRef      GetRootNodePtr ()
    {
    return m_rootNodePtr;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
XmlDomRef      GetDomPtr ()
    {
    return m_pXMLDom;
    }
/*--------------------------------------------------------------------------**//**
* @description Process the XML String that is attached to the element     
* @param  stringRef is the xml string.
* @param  pDom the DOM to append the data to.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
    void ParseXMLStringData (WStringR stringRef)
        {
        //Reporter::log_printf(1,"Printing the Node Information \n");
        XmlNodeRef pNode;
        XmlDomRef  pTempDom;
        XmlNodeRef pRoot;

        mdlXMLDom_createFromText (&pTempDom,FALSE,stringRef.GetMSWCharCP());
        mdlXMLDom_getRootElement(&pNode,pTempDom);

        mdlXMLDom_getRootElement(&pRoot,m_pXMLDom);
        mdlXMLDomElement_appendChild (pRoot,pNode);
        }
/*--------------------------------------------------------------------------**//**
* @description Process the XML String that is attached to the element     
* @param  stringRef is the xml string.
* @param  pDom the DOM to append the data to.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
    void ParseXMLStringData (WStringR stringRef, XmlNodeRef& pNode)
        {
        XmlDomRef  pTempDom;
        mdlXMLDom_createFromText (&pTempDom,FALSE,stringRef.GetMSWCharCP());
        mdlXMLDom_getRootElement(&pNode,pTempDom);
        }
/*--------------------------------------------------------------------------**//**
* @description Read the xml data that is the ECObject associated with this element     
* @param  eh is the element handle for the graphical element.
* @param  pDom is the DOM to append to.
* @param  schemaName the specific schema to work with.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
    void _DataHandler (ElemHandle eh, WStringR schemaName)
        {
        BXN::StringListHandle slHandle;
        XmlInstanceStatus  iStatus;
        ElementRef elRef = eh.GetElemRef ();
 
        slHandle = m_apiRef.ReadInstances(iStatus,elRef);

        int count = slHandle.GetCount();
        int i;
        WString pMember;
        for (i = 0;i<count;++i)
            {
            WString pSchemaName;
            pMember = slHandle.GetString(i);
           
            if (schemaName.length()>0)
                {//if a target name is provided then ask the element if it has data then compare the information.
                pSchemaName = Bentley::XMLInstanceAPI::Native::XmlInstanceApi::GetSchemaNameFromXmlInstance (iStatus,pMember);
                if ((schemaName.CompareToI(WString("*"))== 0)||(schemaName.CompareTo (pSchemaName)==0)) 
                    ParseXMLStringData (pMember);
                }
            else
                ParseXMLStringData(pMember);
            }
        }
/*--------------------------------------------------------------------------**//**
* @description Iterates through all of the schemas in the model buillds a dom of the output.
* @param pModel the model to look through.
* @param showInMsg will show the names of the schema present in the DMSG dialog (for debugging)
* @param pDom is the DOM to build with the schema information.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
    int SchemaInformationHandler (DgnModelRefP pModel,bool showInMsg)
        {
        //look into serializschema information
        //add these to a list model and display.
        BXN::StringListHandle slHandle;
        bool bStatus;
        XmlInstanceStatus iStatus;
        slHandle = m_schemaManager.ReadSchemas(bStatus);
        int schemaCount = slHandle.GetCount();
        int i;
        WString pMember;
        char    msg[512];
        for (i = 0;i<schemaCount;++i)
            {
            pMember = slHandle.GetString(i);
            if (showInMsg)
                {
                sprintf(msg,"Schema Name: %S",pMember.c_str());
                mdlDialog_dmsgsPrint(msg);
                }
            }
        slHandle = m_schemaManager.SerializeSchemas(iStatus);
        schemaCount = slHandle.GetCount();
        for (i = 0;i<schemaCount;++i)
            {
            pMember = slHandle.GetString(i);
            ParseXMLStringData(pMember);
            }
        return schemaCount;
        }
/*--------------------------------------------------------------------------**//**
* @description Check to see if a schema is already in the model
* @param pModel the model to look in.
* @param schemaName the schema name to look for.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
    bool SchemaInModel (DgnModelRefP pModel, WStringCR schemaName)
        {
        BXN::StringListHandle slHandle;
        XmlInstanceStatus     iStatus;
        bool                  bStatus;

        slHandle = m_schemaManager.ReadSchemas(bStatus);
        int schemaCount = slHandle.GetCount();
        int i;
        bool bFound = FALSE;
        WString pMember;
        //loop through the list.
        for (i = 0;i<schemaCount;++i)
            {
            pMember = slHandle.GetString(i);
            if (pMember.CompareTo (schemaName)==0)
                {
                bFound = TRUE;
                break;
                }
            }

        return bFound;
        }

/*--------------------------------------------------------------------------**//**
* @description Check to see if a schema is already in the model if it is then a xml node will be returned
* @param pModel the model to look in.
* @param schemaName the schema name to look for.
* @param nodeP the xml node for the schema defintion.  
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
    bool GetSchemaNode (DgnModelRefP pModel, WStringCR schemaName, XmlNodeRef& nodeP)
        {
        BXN::StringListHandle slsHandle;
        BXN::StringListHandle slHandle;
        bool                  bStatus;
        XmlInstanceStatus     iStatus;
        slHandle = m_schemaManager.ReadSchemas(bStatus);
        slsHandle = m_schemaManager.SerializeSchemas(iStatus);
        int schemaCount = slHandle.GetCount();
        int i;
        bool bFound = FALSE;
        WString pMember;
        WString pFragment;
        //loop through the list.
        for (i = 0;i<schemaCount;++i)
            {
           pMember = slHandle.GetString(i);
            //Reporter::log_printf(4,"schema name: %S \n",pMember.c_str());
            if (pMember.CompareTo (schemaName)==0)
                {
//                pMember = slHandle.GetString(i);
                pFragment = slsHandle.GetString(i);
                //Reporter::log_printf(4,"schema name: %S \n",pFragment.c_str());
                
                ParseXMLStringData(pFragment,nodeP);

                bFound = TRUE;
                break;
                }
            }

        return bFound;

        }
/*--------------------------------------------------------------------------**//**
* @description Call this to add the schema to the model.  This method should only
* be called after checking to see if the schema is in the model. 
* @param pModel the model to add to.
* @param schemaName the full path to the xml file that defines the schema.
* @param MajorVer is the major version number
* @param MinorVer is the minor version number
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
     bool AddSchemaToModel (DgnModelRefP pModel, WStringR schemaName,int MajorVer,int MinorVer,Bentley::XMLInstanceAPI::Native::XmlInstanceSchemaManagerR schemaManager)
        {
        schemaManager.AddSchema(schemaName,MajorVer, MinorVer);
        return true;
        }
/*--------------------------------------------------------------------------**//**
* @description Save the data to an XML file.  This function will allow for the 
* selection of a file.
* @param  pDom is the DOM to write out to a file.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
Public  void SaveXMLFile()
        {
        char	    xmlFilename[MAXFILELENGTH];
        MSWChar     wsXmlFilename[MAXFILELENGTH];
        int         status;
        char*       dataLocationPath;
        
        dataLocationPath = mdlSystem_expandCfgVar ("MS_DATA");

         if ((SUCCESS == mdlDialog_fileCreate (xmlFilename, NULL, 0, "sample.xml", "*.xml", dataLocationPath, "Save As XML File"))
                 && (0 < strlen(xmlFilename)))
                 {
                 mdlCnv_convertMultibyteToUnicode(xmlFilename, -1, wsXmlFilename, MAXFILELENGTH);

                 /* Save the XML Dom out to a FILE   */
                 status = mdlXMLDom_save (m_pXMLDom, FILESPEC_LOCAL, wsXmlFilename, NULL, NULL,  L"Windows-1252", TRUE, TRUE, TRUE);
                 }
         mdlSystem_freeCfgVarBuffer (dataLocationPath);
        }

    };

class PackageHandler
    {
    Bentley::MicroStation::IModel::IModelPublisher* pEngine;
    public:
/*--------------------------------------------------------------------------**//**
* @description Sets the options for the packaging of the IModel.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/    
    PackageHandler(char* unparsed)
        {
        pEngine = Bentley::MicroStation::IModel::IModelPublisher::Create(); 
        }
/*--------------------------------------------------------------------------**//**
* @description Wratps the create package method of the publishing engine.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/    
 void CreatePackage(Bentley::Ustn::MSDocumentPtr packageDocument, MSWCharP name)
     {
     pEngine->PublishAndPackage(*packageDocument,name);
     }
/*--------------------------------------------------------------------------**//**
* @description Sets the options for the packaging of the IModel.
* @param packageOpts the global package options
* @param strOptionName the string name for the option to be manipulated
* @param onOff the flag to turn on or off the boolean settings.
* @param options the void pointer to carry in the variable option parameters.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/    
   bool SetCreatePackageOption 
        (
        WString strOptionName,
        bool onOff,
        void* options
        )
        {
        PackageOptionsEnum po = PackageOptionFromString (strOptionName);
        StatusInt iStatus=ERROR;
        Bentley::MicroStation::IModel::PackageOptionsR packageOpts = pEngine->PackageOptions();
        switch (po)
            {
        case GetRemoveModel:
                {
                packageOpts.SetRemoveIModelsAfterPackaging (onOff);
                iStatus = 0;
                break;
                }
            case CanExport:
                {
                packageOpts.SetCanExport(onOff);
                iStatus = 0;
                break;
                }
            case CanPrint:
                {
                packageOpts.SetCanPrint(onOff);
                iStatus = 0;
                break;
                }
            case Password:
                {
                MSCharP pwd = (MSCharP)options;
                iStatus = packageOpts.SetPassword((const MSCharP)pwd);
                break;
                }
            case Certificate:
                {
                CertInfo *pInfo = (CertInfo*)options;
                iStatus = packageOpts.SetCertificate(pInfo->certName,pInfo->recipient);
                break;
                }
            case Expiration:
                {
                MSWCharP expiration= (MSWCharP)options;
                iStatus = packageOpts.SetExpiration(expiration);
                break;
                }
            case License:
                {
                Bentley::MicroStation::IModel::LicenseType license = Bentley::MicroStation::IModel::LICENSE_None_ProtectionDisabled;
                char*  licenseString = (char*)options;

                if (strcmpi(licenseString,"user"))
                    license = Bentley::MicroStation::IModel::LICENSE_Everyone;
                if (strcmpi(licenseString,"pass"))
                    license = Bentley::MicroStation::IModel::LICENSE_Password;
                if (strcmpi(licenseString,"cert"))
                    license = Bentley::MicroStation::IModel::LICENSE_Certificate;

                iStatus = packageOpts.SetLicenseType (license);
                break;
                }
            default:
                return false;
            }

        if (ERROR != iStatus)
            return true;
        else
            return false;
        }
    private:
/*--------------------------------------------------------------------------**//**
* @description Converts from a string to the enumeration
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/   
   PackageOptionsEnum PackageOptionFromString (WString name)
        {
        if (0 == name.CompareToI( L"RemoveModel"))
            return GetRemoveModel;
        if (0 == name.CompareToI(L"Export"))
            return CanExport;
        if (0 == name.CompareToI(L"Print"))
            return CanPrint;
        if (0 == name.CompareToI(L"Password"))
            return Password;
        if (0 == name.CompareToI(L"Certificate"))
            return Certificate;
        if (0 == name.CompareToI(L"Expiration"))
            return Expiration;
        if (0 == name.CompareToI(L"License"))
            return License;
        return PkgOptUnknown;
        }

    };

class PublishHandler
    {
    Bentley::MicroStation::IModel::IModelPublisher* pEngine;

    public:
/*--------------------------------------------------------------------------**//**
* @description Constructs the publishing handler.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/        
    PublishHandler(char* unparsed)
        {
         pEngine = Bentley::MicroStation::IModel::IModelPublisher::Create();
        }
    
   void SetProvenanceSourceCallback (Bentley::MicroStation::IModel::OverrideProvenanceSourceEvent evP)
        {
        pEngine->SetOverrideProvenanceSourceEvent (evP);
        }
Bentley::MicroStation::IModel::IModelPublisher* GetEngine()
    {
    return  pEngine;
    }
/*--------------------------------------------------------------------------**//**
* @description Publishes the IModel.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/    
    void PublishIModel()
        {
        pEngine->Publish();
        }
        
/*--------------------------------------------------------------------------**//**
* @description Publishes the IModel.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
    bool EndsWith (Bentley::WString fullString, Bentley::WString ending)
        {
        if (fullString.length() >= ending.length())
            return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
        else
            return false;
        }

/*--------------------------------------------------------------------------**//**
* @description Creates a DgnDb i-model based on the packaged V8i i-model
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/        
     void PublishDgnDbIModel()
        {
        // detect if current file has extension .i.dgn since we will be converting the current file to DgnDb i-model and it needs to be a V8i package
        DgnFileP    masterDgnFile = mdlDgnFileObj_getMasterFile();
        Bentley::WString currentFileName = masterDgnFile->GetName();
        WString packagedFileExtension = L".i.dgn";
        WString publishedFileExtension = L".dgn.i.dgn";
        bool isPackagedIModel = false;
        if (EndsWith(currentFileName, packagedFileExtension) && !EndsWith(currentFileName, publishedFileExtension))
            isPackagedIModel = true;

        if (!isPackagedIModel)
            {
            mdlOutput_messageCenter(MESSAGE_ERROR, "Please open a packaged V8i i-model with .i.dgn extension in order to proceed", NULL, true);
            return;
            }
          
        // get filename without extension and add .imodel to it
        WString iModelFileName = currentFileName.substr(0, currentFileName.length() - packagedFileExtension.length());
        iModelFileName.Append(".imodel");

        // create an MSDocument based on the destination filename specified by the user
        MSDocumentManagerR  documentManager            = Bentley::Ustn::MSDocumentManager::GetManager();
        Bentley::Ustn::MSDocumentPtr imodelDocument    = NULL;
        Bentley::Ustn::MSDocumentCreateOptions options = DOC_CREATE_SilentReplace;
        DgnOpenStatus status                           = DGNOPEN_STATUS_SUCCESS;
        imodelDocument = documentManager.CreateNewDocument(status, iModelFileName.c_str(), "MS_DEF", DEFPUBLISHDGNFILE_ID, L".imodel", options);
       
        // Attempt to package the file, display success message as message or error in dialog box
        Bentley::MicroStation::IModel::IModelStatus iModelStatus;
        iModelStatus = pEngine->PublishDgnDbIModel (currentFileName.c_str(), NULL /*specify password for package file here if it is protected*/, *imodelDocument);      
        switch (iModelStatus)
            {
            case Bentley::MicroStation::IModel::IMODEL_Success:
                {
                mdlOutput_messageCenter(MESSAGE_INFO, "Successfully published file. If an issues.log file exists check it for any problems encountered during conversion.", NULL, false);
                break;
                }
            case Bentley::MicroStation::IModel::IMODEL_ImporterNotFound:
                {
                mdlOutput_messageCenter(MESSAGE_ERROR, "The importer program to create i-model could not be found", NULL, true);
                break;
                }
            case Bentley::MicroStation::IModel::IMODEL_ImporterNotSupported:
                {
                mdlOutput_messageCenter(MESSAGE_ERROR, "Bentley DgnDb Importer requires Windows 7, Windows Server 2008 R2, or higher", NULL, true);
                break;
                }
            case Bentley::MicroStation::IModel::IMODEL_PackageFileNotFound:
                {
                mdlOutput_messageCenter(MESSAGE_ERROR, "Package file doesn't exist", NULL, true);
                break;
                }
            case Bentley::MicroStation::IModel::IMODEL_MobileIModelFileExtension:
                {
                mdlOutput_messageCenter(MESSAGE_ERROR, "The output file name must end with .imodel", NULL, true);
                break;
                }
            case Bentley::MicroStation::IModel::IMODEL_Error:
                {
                mdlOutput_messageCenter(MESSAGE_ERROR, "Error creating file. Make sure that you have access to the destination file or folder. If the source package is protected by a password, it must be supplied. Check issues.log file for any problems encountered during conversion.", NULL, true);
                break;
                }
            default:
                return;
            }
        }     
        
/*--------------------------------------------------------------------------**//**
* @description Sets the options for the publishing of the IModel.
* @param optionName the string name for the option to be manipulated
* @param value the flag to turn on or off the boolean settings.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/    
    bool SetPublishOption (WStringR optionName, bool value)
        {
        int status = ERROR;
        Bentley::MicroStation::IModel::PublishOptionsR publishOptions = pEngine->PublishOptions();

        PublishOptionsEnum option = GetEnumFromString (optionName);

        switch (option)
            {
        case CacheVisibleEdges:
            {
            status = publishOptions.SetCacheVisibleEdges(value);
            break;
            }
        case FiterDesignHistory:
            {
            status = publishOptions.SetFilterDesignHistory(value);
            break;
            }
        case PublishDesignLinks:
            {
            status = publishOptions.SetPublishDesignLinks(value);
            break;
            }
        case RepublishAllFiles:
            {
            status = publishOptions.SetRepublishAllFiles(value);
            break;
            }
        default:
            return false;
            }

        if (ERROR != status)
            return true;
        else
            return false;
        }

    private:
/*--------------------------------------------------------------------------**//**
* @description converts from the string passed in to the enumeration.
* @param name the string name for the option to be manipulated
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/    
    PublishOptionsEnum GetEnumFromString (WStringR name)
        {
        if (0 == name.CompareToI (L"FilterHistory"))
            return FiterDesignHistory;
        
        if (0 == name.CompareToI(L"PublishDesignHistory"))
            return PublishDesignLinks;
        
        if (0 == name.CompareToI(L"RepublishAll"))
            return RepublishAllFiles;
        
        if (0== name.CompareToI(L"CacheVisibleEdges"))
            return CacheVisibleEdges;
        
        return PubOptUnknown;
        }
    };

/*--------------------------------------------------------------------------**//**
* @description This function consolodates the calls to get the api.    
* @param  if a specific model ref is required.
* @bsimethod                       @author BSI
+---------------------------------------------------------------------------*/
XmlInstanceApi  GetAPI(Bentley::XMLInstanceAPI::Native::XmlInstanceStatus instStatus)
    {
    XmlInstanceSchemaManager  schemaManager;
       
    schemaManager = XmlInstanceSchemaManager();

    XmlInstanceApi apiRef( BXN::XmlInstanceApi::CreateApi(instStatus,schemaManager));
   
    return apiRef;
    }

/*---------------------------------------------------------------------------------**//**
* @description  imodelvisitor_processElements
* @param 	unparsed      The schema to work with * is for all.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelvisitor_processElements (char * unparsed)
    {
     Reporter::log_printf (0,"imodelvisitor_mdlCommand\n");
     ISessionMgrR    pSessionMgr=ISessionMgr::GetManager();
     DgnModelRefP pModel = pSessionMgr.GetActiveModel(); 
     DgnCacheP pCache = pModel->GetDgnCache();
     MSDgnFileP      pFile;
     pFile = pModel->GetDgnFile();
     UInt32 count = pCache->GetElementCount (DGNCACHE_SECTION_GRAPHIC_ELMS);
     DgnElmListIterator iter;
     int elCounter=0;
     WString   rootName("_");
     WString schemaName;
     Reporter  reporter(const_cast<wchar_t*>(rootName.c_str()),pFile,true,false);
     
     Bentley::XMLInstanceAPI::Native::XmlInstanceStatus iStatus;
     Bentley::XMLInstanceAPI::Native::XmlInstanceApi apiRef(GetAPI(iStatus));
     
     BusinessDataHandler myBusiness (apiRef,const_cast<wchar_t*>(rootName.c_str()));
    
     MyElementGraphicsProcessor pMyProcessor (reporter,reporter.GetRootNodePtr());

     DgnFile::CacheIterator* modelCache = DgnFile::CacheIterator::Create(*pFile);
     //this should be changed to pass the top level models to the next function which will then 
     //traverse the modelref chain.
     if ((unparsed) && (strlen (unparsed)>0))
         schemaName = WString (unparsed);
     else if (g_bProcessAll)
        schemaName = WString ("*");

     ModelRefIteratorP  mrIterator;
     DgnModelRefP       modelP;
     mdlModelRefIterator_create (&mrIterator,pModel,MRITERATE_Root|MRITERATE_PrimaryChildRefs,-1);

     if ((schemaName.CompareToI(WString("*"))==0)||(myBusiness.SchemaInModel(pModel,schemaName)))
         {
        
         if (g_bProcessAll || pFile->IsIModel())
             while(NULL!=(modelP= mdlModelRefIterator_getNext(mrIterator)))
                 {
                 pCache = mdlModelRef_getCache(modelP);
                 reporter.SetCurrentModelRef (modelP);

                 if (pCache && (!pCache->IsFilled(DGNCACHE_SECTION_GRAPHIC_ELMS)))
                     pCache->FillSections (DGNCACHE_SECTION_GRAPHIC_ELMS);
                 if ((mdlModelRef_isActiveModel(modelP)) || ((mdlModelRef_isReference(modelP)) && (mdlModelRef_isDisplayedInView(modelP,ANY_VIEW))))
                 for (CacheElemRef elRef = iter.FirstCacheElm(pCache->GetGraphicElms()); NULL != elRef; elRef = iter.NextCacheElm (false, false))
                    {
                    Reporter::log_printf (0,"element of type %ld, and ID = %I64u modelid = %S\n ",elRef->GetElemType(),elRef->GetElemID(), pCache->GetModelName());
                    char dbgBuffer [512];
                    sprintf (dbgBuffer,"element of type %ld, and ID = %I64u modelid = %S\n ",elRef->GetElemType(),elRef->GetElemID(), pCache->GetModelName());
                    WString debugInfo = WString (dbgBuffer);
                    reporter.DebugNode(debugInfo);
                    ElemHandle eh = ElemHandle(elRef,pModel);
                    
                    //this method will enable us to process either DGN or XGraphics independent of the formatting.
                    myBusiness._DataHandler(eh,schemaName);
                    //we could look at the business data and use that as a key to how to handle the graphics.
                    Bentley::Ustn::ElementGraphicsOutput::Process(eh,pMyProcessor);
                    elCounter++;
                    }
                 }

             mdlModelRefIterator_free(&mrIterator);
             pMyProcessor.WriteReportToFile();
             myBusiness.SaveXMLFile ();
             }
//	mdlDialog_dmsgsPrint("imodelvisitor_mdlCommand\n");  
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_processModel
* @param 	pModel      The model to process.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
int imodelVisitor_processModel (DgnModelRefP pModel)
     {
     Reporter::log_printf (0,"imodelvisitor_mdlCommand\n");
     //mdlDialog_dmsgsPrint("imodelvisitor processing offline ...starting\n");
     ISessionMgrR    pSessionMgr=ISessionMgr::GetManager();
     DgnCacheP pCache = pModel->GetDgnCache();
     MSDgnFileP      pFile;
     pFile = pModel->GetDgnFile();
     UInt32 count = pCache->GetElementCount (DGNCACHE_SECTION_GRAPHIC_ELMS);
     DgnElmListIterator iter;
     int elCounter=0;
     WString   rootName("_");
     WString schemaName;
     Reporter  reporter(const_cast<wchar_t*>(rootName.c_str()),pFile,true,false);
    
     Bentley::XMLInstanceAPI::Native::XmlInstanceStatus iStatus;
     Bentley::XMLInstanceAPI::Native::XmlInstanceApi apiRef(GetAPI(iStatus));
     
     BusinessDataHandler myBusiness (apiRef,const_cast<wchar_t*>(rootName.c_str()));
    
     MyElementGraphicsProcessor pMyProcessor (reporter,reporter.GetRootNodePtr());

     DgnFile::CacheIterator* modelCache = DgnFile::CacheIterator::Create(*pFile);
     
     schemaName = WString ("*");
     
     ModelRefIteratorP  mrIterator;
     DgnModelRefP       modelP;
     mdlModelRefIterator_create (&mrIterator,pModel,MRITERATE_Root|MRITERATE_PrimaryChildRefs,-1);

     if ((schemaName.CompareToI(WString("*"))==0)||(myBusiness.SchemaInModel(pModel,schemaName)))
         {        
         if (pFile->IsIModel()||g_bProcessAll)
             while(NULL!=(modelP= mdlModelRefIterator_getNext(mrIterator)))
                 {
                  pCache = mdlModelRef_getCache(modelP);
                  reporter.SetCurrentModelRef(modelP);
                 if (!pCache->IsFilled(DGNCACHE_SECTION_GRAPHIC_ELMS))
                     pCache->FillSections (DGNCACHE_SECTION_GRAPHIC_ELMS);

                 for (CacheElemRef elRef = iter.FirstCacheElm(pCache->GetGraphicElms()); NULL != elRef; elRef = iter.NextCacheElm (false, false))
                    {
                    Reporter::log_printf (0,"element of type %ld, and ID = %I64u modelid = %S\n ",elRef->GetElemType(),elRef->GetElemID(), pCache->GetModelName());
        
                    ElemHandle eh = ElemHandle(elRef,pModel);
                    //this method will enable us to process either DGN or XGraphics independent of the formatting.
                    Bentley::Ustn::ElementGraphicsOutput::Process(eh,pMyProcessor);
                    myBusiness._DataHandler(eh,schemaName);
                    elCounter++;
                    }
                 }

             mdlModelRefIterator_free(&mrIterator);
             pMyProcessor.WriteReportToFile();
             myBusiness.SaveXMLFile ();
             }
//	mdlDialog_dmsgsPrint("imodelvisitor processing offline ...done\n");  
    return elCounter;
    }
/*---------------------------------------------------------------------------------**//**
* @description  GetFileToProcess gets a file to process by showing the standard file open dialog.
* @param 	fileName      The file name to process <= =>
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
bool GetFileToProcess(char* fileName)
    {
    int status;
    status = mdlDialog_fileOpen(fileName,NULL,0,"","*.i.dgn","","Select i model to process");
    return (SUCCESS == status);
    }

/*---------------------------------------------------------------------------------**//**
* @description  offLineModelProcessor the iterator callback function to get all of 
* the models for processing.
* @param 	continueFlag      Flag to set to stop processing
* @param    pModel            The model to process
* @param    isDefault         Flag that tells if this is the default model in file.
* @param    args              user arguement passed in to this function.
*
* @return non zero for a failed process.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
int offLineModelProcessor
    (
    int* continueFlag,
    DgnModelRefP pModel,
    BoolInt      isDefault,
    void*        args
    )
    {
    int status;
    status = imodelVisitor_processModel(pModel);
    *continueFlag = TRUE;
    return 0;
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_processOffLine The command entry point
* @param 	unparsed      The unparsed information sent to the command
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_processOffLine(char* unparsed)
    {
    DgnModelRefP pModel;
    int threed;
    char fileName[MAXFILELENGTH];
    MSWCharP modelName=L"Default";
    int fmt;
    int status;
    bool bGotFile = true;

    if ((*unparsed)&&(strlen(unparsed) >0))
        {
        //strcpy (fileName,unparsed);
        status = mdlFile_find (fileName,unparsed,"","");
        bGotFile = (SUCCESS == status);
        }
    else
        bGotFile = GetFileToProcess(fileName);
    
    if (!bGotFile)
        return;
    DgnFileP pFile; 
   
    status = mdlWorkDgn_openFile (&pModel,&fmt,&threed,fileName,NULL,TRUE);
    if (SUCCESS == status)
        {
        pFile = mdlModelRef_getDgnFile (pModel);
        DgnCacheP pCache = mdlModelRef_getCache(pModel);

        if (!pCache->IsFilled(DGNCACHE_SECTION_GRAPHIC_ELMS))
             pCache->FillSections (DGNCACHE_SECTION_GRAPHIC_ELMS);

        mdlDgnFileObj_traverseRootModelRefs(offLineModelProcessor,NULL,pFile);
        }
    status = mdlWorkDgn_closeFile (pModel);
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_traverseMembers - this is the callback for group member traversal.
* the grphic is processed and business data added to the dom that is passed in as a user arg.
* @param 	displayPathP
* @param 	namedGroupP
* @param 	memberIndex
* @param 	userArg
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
StatusInt imodelVisitor_traverseMembers 
    (
    DisplayPathP    displayPathP,
    NamedGroupP     namedGroupP,
    int             memberIndex,
    void*           userArg
    )
    {
    int numEntries = mdlDisplayPath_getCount (displayPathP);
    ElementRef  refP = mdlDisplayPath_getElem (displayPathP,0); //just get the first for the moment...?

    DgnModelRefP pModel =  mdlDisplayPath_getPathRoot(displayPathP);
    DgnFileP pFile = pModel->GetDgnFile();
    WString   rootName("_");
    WString schemaName;
    Reporter  reporter(const_cast<wchar_t*>(rootName.c_str()),pFile,true,false);
    
    Bentley::XMLInstanceAPI::Native::XmlInstanceStatus iStatus;
    Bentley::XMLInstanceAPI::Native::XmlInstanceApi apiRef(GetAPI(iStatus));

    BusinessDataHandler myBusiness (apiRef,const_cast<wchar_t*>(rootName.c_str()));
    
    MyElementGraphicsProcessor pMyProcessor (reporter,reporter.GetRootNodePtr());
    reporter.SetCurrentModelRef(pModel);
    ElemHandle eh(refP, pModel);

    Bentley::Ustn::ElementGraphicsOutput::Process (eh,pMyProcessor);

    myBusiness._DataHandler(eh,schemaName);
  
    return 0;
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_scanForItemSet - this is the callback that will loop through
* the namedgroups(ItemSets) and do a compare to find the one in question.  Once found the group
* is then sent to the callback for detail processing.
* @param 	namedGrpEdP      The named group (Itemset)
* @param    userArg          a void pointer for any user args to send to the callback.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
int imodelVisitor_scanForItemSet
    (
    MSElementDescrP namedGrpEdP,
    void*           userArg
    )
    {
    MSWCharP pName = (MSWCharP)userArg;
    int   status;

    //if the group is not a componenet set then return.
    if (NULL==mdlLinkage_extractFromElement(NULL,&namedGrpEdP->el,LINKAGEID_ComponentSet,0,NULL,NULLFUNC,NULL))
        return SUCCESS;

     
    NamedGroupP sourceGrp = NULL;
    MSWChar     sourceName[512];

    status = mdlNamedGroup_fromElementDescr (&sourceGrp,namedGrpEdP, namedGrpEdP->h.dgnModelRef,namedGrpEdP->h.dgnModelRef);

    if (SUCCESS == status)
        if (SUCCESS == mdlNamedGroup_getName(sourceGrp,sourceName,512))
            {
            
            WString SrcName(sourceName);
            WString targName(pName);
            if (0==(targName.CompareToI(SrcName)))
                {
                //this should be traversing the elements in the named group....
                mdlNamedGroup_traverseMembers(sourceGrp,imodelVisitor_traverseMembers,NAMEDGROUP_Traverse_Simple,NULL,FALSE,FALSE);
                }
            }
        return status;
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_visitItemSet - this will look through the model to find
* the itemset and then visit the graphics and business data.
* @param 	unparsed      The name of  the itemset to look for.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_visitItemSet(char* unparsed)
    {
    //still in progress...
    //get model ref iterator
    //in navigator the references are the important item.
    ModelRefIteratorP  mrIterator;
    DgnModelRefP       modelP;
    DgnCacheP pCache;
    mdlModelRefIterator_create (&mrIterator,MASTERFILE,MRITERATE_Root|MRITERATE_PrimaryChildRefs,-1);
    while(NULL!=(modelP= mdlModelRefIterator_getNext(mrIterator)))
         {
         pCache = mdlModelRef_getCache(modelP);
         if (!pCache->IsFilled(DGNCACHE_SECTION_GRAPHIC_ELMS))
              pCache->FillSections (DGNCACHE_SECTION_GRAPHIC_ELMS);
         mdlNamedGroup_traverseGroups (modelP,imodelVisitor_scanForItemSet,unparsed);
         }
    mdlModelRefIterator_free (&mrIterator);
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_reportSchema - This will publish the schema stored in the 
* file to an xml file.
* @param 	unparsed      unused.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_reportSchema (char* unparsed)
    {
     WString   rootName("_");
     
     Bentley::XMLInstanceAPI::Native::XmlInstanceStatus iStatus;
     Bentley::XMLInstanceAPI::Native::XmlInstanceApiR apiRef(GetAPI(iStatus));
     BusinessDataHandler myBusiness (apiRef,const_cast<wchar_t*>(rootName.c_str()));
    //early dump out the schema names?
     myBusiness.SchemaInformationHandler(MASTERFILE,true);
     myBusiness.SaveXMLFile ();
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_addSchema
* @param 	unparsed      The unparsed information sent to the command
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_addSchema (char* unparsed)
    {
    WString schemaName;
    int major,minor;
    int status;
    major = 0;
    minor = 0;
    char    schemaFileName[MAXFILELENGTH];
    char*   filterString="*.xml";
    char*   titleString ="Schema File";
    if ((unparsed) && (strlen (unparsed)>0))
        schemaName = WString(unparsed);
    else
        {
        status = mdlDialog_fileOpen (schemaFileName,NULL,NULL,NULL,filterString,NULL,titleString);
        if (SUCCESS!=status)
            return;
        char dev[255];
        char dir[255];
        char name[255];
        char exten[255];
        mdlFile_parseName (schemaFileName,dev,dir,name,exten);
    //take .eschema off the file name.
        char localName[512];
        memset (localName,'\0',sizeof localName);
        strncpy (localName,name,strlen(name) - 9);
        schemaName =WString(localName);
        }
     WString   rootName("_");
     
     XmlInstanceStatus iStatus;
     XmlInstanceApiR apiRef(GetAPI(iStatus));

     BusinessDataHandler myBusiness (apiRef,const_cast<wchar_t*>(rootName.c_str()));

    if (!myBusiness.SchemaInModel (ACTIVEMODEL,schemaName))
        {
        WString fullName = WString(schemaFileName);
        Bentley::XMLInstanceAPI::Native::XmlInstanceSchemaManager schemaManager;
        myBusiness.AddSchemaToModel(ACTIVEMODEL,fullName,major,minor,schemaManager);
        }
    }

 static void  OverRideSourceCallback(Bentley::WString &newName,MSWCharCP oldName)
        {
        newName = WString ("marka");
        //Bentley::MicroStation::IModel::OverrideProvenanceSourceEvent
        return;
        }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_publish
* @param 	unparsed      The unparsed information sent to the command
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_publish (char* unparsed)
    {
    PublishHandler myPublish (unparsed);

    //experiment with the provenance callbacks
    myPublish.GetEngine()->SetOverrideProvenanceSourceEvent (OverRideSourceCallback );
    myPublish.PublishIModel();
    }

/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_publish
* @param 	unparsed      The unparsed information sent to the command
* @bsimethod 							BSI             09/15
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_publishdgndb(char* unparsed)
    {
    PublishHandler myPublish(unparsed);
    myPublish.PublishDgnDbIModel();
    }

/*---------------------------------------------------------------------------------**//**
* @description  sets the publish options.  It uses the format optionname:value to pass the information on a single commandline.
* @param 	unparsed      The unparsed information sent to the command.  Each word on the commandline is : separated!
* In this application this is the command options:
* Option: FilterHistory - true/false
* Option: PublishDesignHistory - true/false
* Option: RepublishAll - true/false
* Option: CacheVisibleEdges - true/false
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_setPublishOptions(char* unparsed)
    {
    int pos;
    char OptionName[512];
    bool onOff;
    PublishHandler myPublish(unparsed);
    memset (OptionName,'\0',sizeof OptionName );
    pos = strcspn (unparsed,":");
    strncpy (OptionName,unparsed,pos);
    if (0 == strcmpi("true",unparsed+pos+1))
        onOff = true;
    else
        onOff = false;

    myPublish.SetPublishOption(WString(OptionName),onOff);
    }
/*---------------------------------------------------------------------------------**//**
* @description  sets the package options.  It uses the format optionname:value:value to pass the information on a single commandline.
* @param 	unparsed      The unparsed information sent to the command.  Each word on the commandline is : separated!
* In this application this is the command options:
* Option: removemodel - On/Off
* Option: print - true/false
* Option: export - true/false
* Option: license - Type: user/cert/pass for a user, certificate, or password.
* the certificate or password are set using the setPackageOption for password or certificate.
* Option: password - the password to set.
* Option: certificate - the certName:certRecipient pair
* Option: expiration - the date this expires
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_setPackageOption (char* unparsed)
    {
    PackageHandler myPackage(unparsed);
    bool onOff;
    char OptionName[512];
    int pos;
    void* options = NULL;
    CertInfo* infoP=NULL;
    memset (OptionName,'\0',sizeof OptionName);
    pos = strcspn (unparsed,":");
    strncpy (OptionName,unparsed,pos);

    if (0 == strcmpi (OptionName,"removemodel"))
        {
        int totalLen = strlen (unparsed);
        if ((totalLen - (pos+1))>4)
            onOff = false;
        else
            onOff = true;
        }

    if (0 == strcmpi(OptionName,"print"))
        {
        int totalLen = strlen (unparsed);
        if ((totalLen - (pos+1))> 4)
            onOff = false;
        else
            onOff = true;
        }

    if (0 == strcmpi(OptionName,"export"))
        {
        int totalLen = strlen(unparsed);
            if ((totalLen - (pos+1))>4)
                onOff = false;
            else
                onOff = true;
        }

    if (0 == strcmpi(OptionName,"license"))
        {
        char LicType[5];
        memset (LicType,'\0',sizeof LicType);
        strcpy (LicType,unparsed+pos);
        options = LicType;
        }

    if (0 == strcmpi(OptionName,"password"))
        {
        char passwrd[512];
        strcpy (passwrd,unparsed+pos);
        options = passwrd;
        }
    if (0 == strcmpi(OptionName,"certificate"))
        { 
        char certName[512];
        char certRecipient[512];
        int pos2 =strcspn (unparsed+pos,":");
        int pos3 = strcspn (unparsed + pos + pos2,":");
       
        infoP = (CertInfo*)malloc (sizeof *infoP);
        mdlCnv_convertMultibyteToUnicode (certName,-1,infoP->certName,-1);
        mdlCnv_convertMultibyteToUnicode (certRecipient,-1,infoP->recipient,-1);
        strncpy(certName,unparsed+pos+pos2,pos3 - (pos2+pos));
        strcpy(certRecipient,unparsed+pos+pos2+pos3);
        options = infoP;
        }
    if (0 == strcmpi(OptionName,"expiration"))
        {
        char expiry[512];
        strcpy (expiry,unparsed+pos);
        options = expiry;
        }

    myPackage.SetCreatePackageOption (WString(OptionName),onOff,options);
    
    if (infoP)
        free (infoP);
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_package
* @param 	unparsed      The unparsed information sent to the command
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_package (char* unparsed)
    {
    PackageHandler myPackage(unparsed);
    MSDocumentManagerR  documentManager            = Bentley::Ustn::MSDocumentManager::GetManager();
    Bentley::Ustn::MSDocumentPtr packageDocument   = NULL;
    MSWCharP  wFileName                            = L"YOUR_FILE_URI";
    Bentley::Ustn::MSDocumentCreateOptions options = DOC_CREATE_Default;
    DgnOpenStatus status                           = DGNOPEN_STATUS_SUCCESS;
    char fileName[MAXFILELENGTH];
    mdlDialog_fileOpen (fileName,NULL,0,"","","","Select Package Name");
    WString wsFileName = WString(fileName);
    
    packageDocument= documentManager.CreateNewDocument (status, wsFileName.GetMSWCharCP(), "MS_DEF", DEFPUBLISHDGNFILE_ID, L".dgn", options);
    
    myPackage.CreatePackage(packageDocument,L"");

    }

/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_package
* @param 	pNode      The xml node to hold the entire schema
* @param        className   The name of the class to build
* @param        pClass     The instance data class as held in xml
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
void imodelVisitor_ReportData(XmlNodeRef pNode,WString className, XmlNodeRef&  pClass)
    {
    XmlNamedNodeMapRef  nameMap;
    if (SUCCESS == mdlXMLDomElement_getAllAttributes (&nameMap,pNode))
     {    
        XmlNodeRef  childAttr = NULL;
        mdlXMLDomAttrList_getFirstChild (&childAttr, nameMap);
        
        while (childAttr != NULL)
            {
            MSWChar attrName  [256];
            MSWChar attrValue [256];
            //this is where you would do the update
            int size = DIM (attrValue);
            mdlXMLDomAttr_getValue (attrValue, &size, childAttr);
            size = DIM (attrName);
            mdlXMLDomAttr_getName (attrName, &size, childAttr);
            
            printf ("attr name = %S, value = %S\n", attrName, attrValue);
            
            mdlXMLDomNode_free (childAttr);
            childAttr = NULL;
            mdlXMLDomAttrList_getNextChild  (&childAttr, nameMap);
            }
        
        mdlXMLDomAttrList_free (nameMap);
        }

    //in here you will be handed the attributes that you can eventually update
     //  Report on sub-elements
    XmlNodeListRef  nodeListRef = NULL;
    
    if (mdlXMLDomNode_getChildNodes (&nodeListRef, pNode) == SUCCESS)
        {
        int     numChildren = mdlXMLDomNodeList_getNumChildren (nodeListRef);
        
        for (int i = 0; i < numChildren; i++)
            {
            XmlNodeRef  childNodeRef;
            mdlXMLDomNodeList_getChild (&childNodeRef, nodeListRef, i);

            //I am just cloning the first node for the moment.
            if (pClass==NULL)
                mdlXMLDomNode_cloneNode (&pClass,childNodeRef,false);
            else
                {
                XmlNodeRef pChildClone;
                mdlXMLDomNode_cloneNode (&pChildClone,childNodeRef,false);
                mdlXMLDomElement_appendChild (pClass,pChildClone);
                }
            imodelVisitor_ReportData (childNodeRef,className,pClass);
            mdlXMLDomNode_free (childNodeRef);
            }
                    
        mdlXMLDomNodeList_free (nodeListRef);
        }

    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_package
* @param 	pNode      The XMLNode to build the schema
* @param        className  The name of the class to create from the schema
* @param        pClass     The instance class to create as an xml fragment
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
void imodelVisitor_buildInstanceData (XmlNodeRef pNode, WString className, XmlNodeRef& pClass)
    {
    imodelVisitor_ReportData(pNode,className,pClass);
    }

void imodelVisitor_populateSingleInstance(WString className, XmlNodeRef& pNode, XmlNodeRef pClass)
    {
    int        status;
    XmlDomRef  pDom;
    XmlNodeRef pData;

    XmlHelper::CreateDomInstance(&pDom,false);
    //add a root node...
    mdlXMLDom_createElement (&pNode,pDom,className.c_str());
    mdlXMLDom_setRootElement(pDom,pNode);
    // All of the field names should be sorted out from the schema.
    status = mdlXMLDomElement_addAttribute (pNode,XMLDATATYPE_STRING,L"xmlns","_TestSteelcase.00.00");

    status = mdlXMLDomElement_addAttribute (pNode, XMLDATATYPE_STRING,L"instanceID","");
    XmlNodeListRef nodeListRef;
    
    if (mdlXMLDomNode_getChildNodes (&nodeListRef, pClass) == SUCCESS)
        {
        int     numChildren = mdlXMLDomNodeList_getNumChildren (nodeListRef);
        
        for (int i = 0; i < numChildren; i++)
            {
            XmlNodeRef  childNodeRef;
            mdlXMLDomNodeList_getChild (&childNodeRef, nodeListRef, i);
            XmlNamedNodeMapRef  nameMap;
            if (SUCCESS == mdlXMLDomElement_getAllAttributes (&nameMap,childNodeRef))
                {    
                XmlNodeRef  childAttr = NULL;
                MSWChar nodeName[255];
                int lenNodeName=255;
                WString nodeValue;

                status = mdlXMLDomAttrList_getChildByName( &childAttr,nameMap,L"propertyName");
                
                status = mdlXMLDomAttr_getValue (nodeName,&lenNodeName,childAttr);

                pData = XmlHelper::AddElementStringValue (pNode,pDom, nodeName,L"");  
                mdlXMLDomElement_removeAttribute (pData,L"xmlns");  
                }
            }
     }
   
    int maxLen;

     //once done with the data update this has a complete fragment.  
    mdlXMLDomNode_getXmlText (NULL,&maxLen,pNode);

    MSWChar*  xmlFragment = (MSWChar *)_alloca (++maxLen*sizeof MSWChar);

    mdlXMLDomNode_getXmlText (xmlFragment,&maxLen,pNode);


    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_addAttribute
* @param 	unparsed      The unparsed information sent to the command this is the schema name
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C"  DLLEXPORT void imodelVisitor_addAttribute(char* unparsed)
{
    //get the schema as a dom node?
    WString rootName(unparsed);
    Bentley::XMLInstanceAPI::Native::XmlInstanceStatus iStatus;
    Bentley::XMLInstanceAPI::Native::XmlInstanceApiR apiRef(GetAPI(iStatus));

    BusinessDataHandler myBusiness (apiRef,const_cast<wchar_t*>(rootName.c_str()));
    
    bool hasDataClassDef;
    XmlNodeRef pNode=NULL;
    XmlNodeRef pChildNode=NULL;
    int maxLen;
    WString  className=L"_FSLData";
    XmlNodeRef pClass=NULL;
    //create an instance of the class use the node
    hasDataClassDef = myBusiness.GetSchemaNode (MASTERFILE,rootName,pNode);
    //if you get the schema back then you can recurse through it to find the fields to update
    if (hasDataClassDef)
        imodelVisitor_buildInstanceData(pNode,className,pClass);
    
    imodelVisitor_populateSingleInstance (className, pNode, pClass);

    //once done with the data update this has a complete fragment.  
    mdlXMLDomNode_getXmlText (NULL,&maxLen,pNode);

    MSWChar*  xmlFragment = (MSWChar *)_alloca (++maxLen*sizeof MSWChar);

    mdlXMLDomNode_getXmlText (xmlFragment,&maxLen,pNode);
    
    //need to just get the part of the xml and then use the xmlinstance api to put to the element.
    imodeltoolModifyTool* imodelTool = new imodeltoolModifyTool();
    imodelTool->SetFragmentString(WString (xmlFragment));
    imodelTool->MyInstallTool();

}
#define BUFSIZE 255
//#if defined (RPC_WORKINPROGRESS)
int SendMessagePipe(WString  unparsed)
{ 
   HANDLE hPipe; 
    LPCWSTR lpvMessage=L"Default message from client."; 
    wchar_t* chBuf[BUFSIZE] = {'\0'}; 
    BOOL   fSuccess = FALSE; 
    DWORD  cbToWrite, cbWritten; 
    LPWSTR lpszPipename = L"\\\\.\\pipe\\mynamedpipe"; 
   // if ( unparsed )
    WString message=WString(L"echo : " + unparsed);
        lpvMessage = message.GetMSWCharCP();

    // Try to open a named pipe; wait for it, if necessary. 

    while (1) 
        { 
        hPipe = CreateFileW( 
            lpszPipename,   // pipe name 
            GENERIC_READ |  // read and write access 
            GENERIC_WRITE, 
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file 

        // Break if the pipe handle is valid. 

        if (hPipe != INVALID_HANDLE_VALUE) 
            break; 

        // Exit if an error other than ERROR_PIPE_BUSY occurs. 

        if (GetLastError() != ERROR_PIPE_BUSY) 
            {
            //_tprintf( TEXT("Could not open pipe. GLE=%d\n"), GetLastError() ); 
            return -1;
            }

        // All pipe instances are busy, so wait for 2 seconds. 

        if ( ! WaitNamedPipeW(lpszPipename, 2000)) 
            { 
            //printf("Could not open pipe: 2 second wait timed out."); 
            return -1;
            } 
        } 


    // Send a message to the pipe server. 

    cbToWrite = (lstrlenW(lpvMessage)+1)*sizeof(wchar_t);
    //_tprintf( TEXT("Sending %d byte message: \"%s\"\n"), cbToWrite, lpvMessage); 

    fSuccess = WriteFile( 
        hPipe,                  // pipe handle 
        lpvMessage,             // message 
        cbToWrite,              // message length 
        &cbWritten,             // bytes written 
        NULL);                  // not overlapped 

    if ( ! fSuccess) 
        {
        // _tprintf( TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError() ); 
        return -1;
        }

    CloseHandle(hPipe); 

    return 0; 
   }

//#endif

/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_ipcCommands
* @param 	unparsed      The unparsed information sent to the command this is the schema name
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_ipcCommands (char* unparsed)
    {
    //this demonstrates the ability to pass one command on from the initial command.
    //StatusInt status = SendMymodelMessage (WString(unparsed),WString(L"Testing"));
    StatusInt status = SendMessagePipe(WString(unparsed));
    if (SUCCESS == status)
        mdlInput_sendKeyin (unparsed, 0, INPUTQ_EOQ,"imodelvisitor");
    }
/*---------------------------------------------------------------------------------**//**
* @description  imodelVisitor_setProcessOptions
* @param 	unparsed      The unparsed information sent to the command this is All to process 
*                             dgn models and all schema.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT void imodelVisitor_setProcessOptions (char* unparsedP)
    {
    //this allows the application to have a flag for processing all the schema with out adding the command line *
    //this will also allow processing of dgn models not just i-models.
    if ((unparsedP) && (0==strcmpi ("ALL",unparsedP)))
        g_bProcessAll = true;
    else
        g_bProcessAll = false;
    }
/*---------------------------------------------------------------------------------**//**
* @description  MdlMain this sample reflects Section 31 i-model Development in programmers guide
* @param 	argc      The number of command line parameters sent to the application.
* @param 	argv[]    The array of strings sent to the application on the command line.
* @bsimethod 							BSI             06/03
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C" DLLEXPORT  int MdlMain
(
int         argc,
char        *argv[]
)
    {
	RscFileHandle   rfHandle;
    g_bProcessAll = true;
    mdlResource_openFile (&rfHandle,NULL,RSC_READONLY);

   // Map key-in to function
    static MdlCommandNumber cmdNumbers[] =
    {
	{imodelvisitor_processElements,  CMD_IMODELVISITOR_ACTION_PROCESS },
        {imodelVisitor_processOffLine, CMD_IMODELVISITOR_ACTION_OFFLINE },
        {imodelVisitor_publish, CMD_IMODELVISITOR_ACTION_PUBLISH },
        {imodelVisitor_package, CMD_IMODELVISITOR_ACTION_PACKAGE },
        {imodelVisitor_reportSchema, CMD_IMODELVISITOR_ACTION_REPORT},
        {imodelVisitor_addSchema, CMD_IMODELVISITOR_ACTION_ADDSCHEMA},
        {imodelVisitor_setPublishOptions, CMD_IMODELVISITOR_SET_PUBOPTS},
        {imodelVisitor_setPackageOption, CMD_IMODELVISITOR_SET_PACKAGEOPTS},
        {imodelVisitor_addAttribute, CMD_IMODELVISITOR_ADD_ATTRIB},
        {imodelVisitor_ipcCommands,CMD_IMODELSTART_CMD},
        {imodelVisitor_setProcessOptions, CMD_IMODELVISITOR_SET_PROCALL},
        {imodelVisitor_publishdgndb, CMD_IMODELVISITOR_ACTION_PUBLISHDGNDB},
        0,
    };

    mdlSystem_registerCommandNumbers (cmdNumbers);

    mdlParse_loadCommandTable (NULL);

 	return SUCCESS;
    }
