/*--------------------------------------------------------------------------------------+
|
|     $Source: /miscdev-root/miscdev/mdl/examples/imodelvisitor/MyElementGraphicsProcessor.h,v $
|
|  $Copyright: (c) 2013 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|									|
|   $Workfile:   imodelvisitor  $
|   $Revision: 1.3 $
|   	$Date: 2013/06/28 20:38:35 $
|									|
+----------------------------------------------------------------------*/
/*----------------------------------------------------------------------+
|                                                                       |
|   Include Files                                                       |
|                                                                       |
+----------------------------------------------------------------------*/

#if !defined(resource)
#include    <mdl.h>
#include    <GPArray.h>
#include    <ElementGraphics.h>
#include    <imodel\xmlinstanceschemamanager.h>
#include    <IModel\xmlinstanceapi.h>
#include    <IModel\stringlisthandle.h>
#include    <IModel\imodelpublisher.h>

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
#include    <toolsubs.h>
#include    <elementref.h>
#include    <msdependency.fdf>
#include    <msassoc.fdf>
#include    <msmisc.fdf>
#include    <mslocate.fdf>
#include    <msstate.fdf>
#include    <msoutput.fdf>
#include    <mskisolid.h>
#include    <mdlxmltools.fdf>
#include    <mskisolid.fdf>
#include    <mdlxmltoolslink.fdf>
#include    <msnamedgroup.fdf>
#include    <msbsplin.fdf>
#include    <mstmatrx.fdf>
#define XMLHANDLER 1
#include "XmlHelper.h"
#include "Reporter.h"
#endif

#if !defined(resource)
USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_USTN
USING_NAMESPACE_BENTLEY_USTN_ELEMENT
USING_NAMESPACE_BENTLEY_XMLINSTANCEAPI_NATIVE

class  MyElementGraphicsProcessor:public Bentley::Ustn::IElementGraphicsProcessor
    {
    Reporter             m_reporter;
    Transform            m_currentTransform;
    XmlNodeRef           m_currentNode;
    ElementFacetOptions *m_FacetOptions;
    
    public :
        MyElementGraphicsProcessor (Reporter reporter, XmlNodeRef currentNode);         
        void  Process (ElemHandle& eh,Bentley::Ustn::IElementGraphicsProcessor processor);

/*=================================================================================
 IElementGraphicsProcessor methods  -- uses the default implemetations of most
 of the methods in this interface
=================================================================================*/

    virtual bool            _ProduceGPAs (bool isCurved) const override{return true;}; //this forces the processing of the GPA
    virtual bool            _ProduceFacets (bool isIndexedPoly) const override{return isIndexedPoly;};
    virtual bool            _ProduceBodies (bool isCurved, int kernelType) const  override{return kernelType== KERNEL_TYPE_PSOLID;};

    virtual StatusInt       _ProcessParasolidBody(KIBodyCP pBody) override; //this handles the Parasolid bodies
    virtual StatusInt       _ProcessAcisBody(KIBodyCP pBody) override;
    virtual StatusInt       _ProcessFacetSet(ElementFacetSet*   pFacets) override;
    virtual BentleyStatus   _ProcessCurve(MSBsplineCurveCR      curve, bool filled) override;
    virtual BentleyStatus   _ProcessCone(DVec3dCR primary, DVec3dCR secondary, DPoint3dCR p0, DPoint3dCR p1, double r0, double r1, bool cap) override;
    virtual BentleyStatus   _ProcessSurface (MSBsplineSurfaceCR surface) override;
    virtual BentleyStatus   _ProcessGPA (GPArrayP gpa, bool isClosed, bool isFilled) override;

    virtual void            _AnnounceTransform(TransformCP trans) override;
    virtual void            _AnnounceElemMatSymb (ElemMatSymbCR matSymb) override;
    virtual void            _AnnounceElemDisplayParams (ElemDisplayParamsCR displayParams) override;

    virtual ElementFacetOptions* _GetFacetOptions() ;
    virtual void WriteReportToFile();

/*=================================================================================
 End of IElementGraphicsProcessor methods
=================================================================================*/
    };
#endif