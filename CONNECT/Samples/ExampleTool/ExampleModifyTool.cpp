/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/ExampleModifyTool.cpp $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "exampletool.h"

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

/*=================================================================================**//**
* Example showing how to use DgnElementSetTool to write a MODIFY ELEMENT tool.
*
* This tool will move the closest vertex on a line or linestring.
* 
* @bsiclass                                                               Bentley Systems
+===============+===============+===============+===============+===============+======*/
struct          ExampleModifyElementTool : DgnElementSetTool
{
private:

bool            m_isDynamics;
DgnButtonEvent  m_ev;

protected:

//! @param[in] toolId Set tool command name identifier.
ExampleModifyElementTool (int toolId) : DgnElementSetTool (toolId) {}

virtual bool            _DoGroups () override {return false;} // Pick single element, don't include graphic and named group members...
virtual UsesSelection   _AllowSelection () override {return USES_SS_None;} // Don't support selection sets...

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
size_t GetCloseVertex (CurveLocationDetailCR location) const
    {
    size_t  nSegments = 1; // If it's not a linestring (ex. line) there's only a single segment.

    if (ICurvePrimitive::CURVE_PRIMITIVE_TYPE_LineString == location.curve->GetCurvePrimitiveType ())
        {
        bvector<DPoint3d> const* points = location.curve->GetLineStringCP ();

        nSegments = ((points->size () < 3) ? 1 : (points->size ()-1));
        }

    double  uSegRange = (1.0 / nSegments);
    size_t  closeVertex = (size_t) ((location.fraction + (uSegRange*.5)) / uSegRange);

    return closeVertex;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
virtual bool _SetupForModify (DgnButtonEventCR ev, bool isDynamics) override
    {
    m_isDynamics = isDynamics;
    m_ev = ev;

    return true;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
virtual StatusInt _OnElementModify (EditElementHandleR eeh) override
    {
    // NOTE: Since we've already validated the element in OnPostLocate and don't support groups/selection sets we don't have to check it again here.
    DPoint3d            locatePoint;
    CurveVectorPtr      curve = ICurvePathQuery::ElementToCurveVector (eeh);
    CurveLocationDetail location;

    _GetAnchorPoint (&locatePoint);

    // Get curve location detail from accept point.
    if (!curve->ClosestPointBounded (locatePoint, location))
        return ERROR;

    // Get the closest vertex from the curve location detail.
    size_t              closeVertex = GetCloseVertex (location);
    ICurvePrimitivePtr& primitive = curve->front ();

    // Update closest vertex on line/line string with current button location.
    switch (primitive->GetCurvePrimitiveType ())
        {
        case ICurvePrimitive::CURVE_PRIMITIVE_TYPE_Line:
            {
            DSegment3d  segment = *primitive->GetLineCP ();
            
            segment.point[closeVertex] = *m_ev.GetPoint ();
            primitive = ICurvePrimitive::CreateLine (segment); // Replace line primitive with a new primitive with modified point.
            break;
            }

        case ICurvePrimitive::CURVE_PRIMITIVE_TYPE_LineString:
            {
            bvector<DPoint3d>  points = *primitive->GetLineStringCP ();

            points[closeVertex] = *m_ev.GetPoint ();
            primitive = ICurvePrimitive::CreateLineString (points); // Replace linestring primitive with a new primitive with modified point.
            break;
            }
        }
    
    // Give the element's handler a chance to update itself from the modified curve vector.
    ICurvePathEdit* pathEdit;

    if (NULL != (pathEdit = dynamic_cast <ICurvePathEdit*> (&eeh.GetHandler ())) && SUCCESS == pathEdit->SetCurveVector (eeh, *curve.get ()))
        return SUCCESS;

    // Handler didn't choose to update itself, create a new element to represent the modified curve vector.
    return DraftingElementSchema::ToElement (eeh, *curve, &eeh, eeh.GetModelRef ()->Is3d (), *eeh.GetModelRef ());
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
virtual bool _OnPostLocate (HitPathCP path, WStringR cantAcceptReason) override
    {
    if (!DgnElementSetTool::_OnPostLocate (path, cantAcceptReason))
        return false;

    ElementHandle   eh (path->GetHeadElem (), path->GetRoot ());
    CurveVectorPtr  curve = ICurvePathQuery::ElementToCurveVector (eh);

    // Accept elements that are open paths with one or more linear segments (ex. line or linestring).
    if (!curve.IsValid () || !curve->IsOpenPath ())
        return false;

    switch (curve->HasSingleCurvePrimitive ())
        {
        case ICurvePrimitive::CURVE_PRIMITIVE_TYPE_Line:
        case ICurvePrimitive::CURVE_PRIMITIVE_TYPE_LineString:
            return true;

        default:
            return false;
        }
    }

/*---------------------------------------------------------------------------------**//**
* Install a new instance of the tool. Will be called in response to external events
* such as undo or by the base class from _OnReinitialize when the tool needs to be
* reset to it's initial state.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
virtual void _OnRestartTool () override
    {
    InstallNewInstance (GetToolId ());
    }

public:

/*---------------------------------------------------------------------------------**//**
* Method to create and install a new instance of the tool. If InstallTool returns ERROR,
* the new tool instance will be freed/invalid. Never call delete on RefCounted classes.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
static void InstallNewInstance (int toolId)
    {
    ExampleModifyElementTool* tool = new ExampleModifyElementTool (toolId);

    tool->InstallTool ();
    }

}; // ExampleModifyElementTool

/*---------------------------------------------------------------------------------**//**
* Function that was associated with the command number for starting the tool.
*
* @param[in] unparsed Additional input supplied after command string.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
Public void startExampleModifyElementTool (WCharCP unparsed)
    {
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    ExampleModifyElementTool::InstallNewInstance (CMDNAME_ExampleModifyTool);
    }

