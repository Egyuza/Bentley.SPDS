/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/ExampleMoveTool.cpp $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "exampletool.h"

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

/*=================================================================================**//**
* Example showing how to use DgnElementSetTool to write a basic MOVE ELEMENT tool.
* 
* The base class populates an ElementAgenda from a pick or selection set on the 1st
* data button and starts element dynamics. The 2nd data button accepts the modification.
*
* The base class provides generic user prompts such as Identify Element and
* Accept/Reject Selection. Override _SetupAndPromptForNextAction to supply more detailed
* direction to the user.
* 
* This sub-class is responsible for applying the desired modification to the element(s). 
* The _OnElementModify method is called for each entry in the ElementAgenda both during
* element dynamics as well as for the final accept.
* 
* @bsiclass                                                               Bentley Systems
+===============+===============+===============+===============+===============+======*/
struct          ExampleMoveElementTool : DgnElementSetTool
{
protected:

Transform       m_transform; // Transform to apply to element(s), initialized in _SetupForModify.

//! @param[in] toolId Set tool command name identifier.
ExampleMoveElementTool (int toolId) : DgnElementSetTool (toolId) {}

virtual bool        _SetupForModify (DgnButtonEventCR, bool isDynamics) override;
virtual StatusInt   _OnElementModify (EditElementHandleR) override; // pure virtual method, sub-class must override!
virtual void        _OnRestartTool () override;                     // pure virtual method, sub-class must override!

public:

static void InstallNewInstance (int toolId);

}; // ExampleMoveElementTool

/*---------------------------------------------------------------------------------**//**
* Tools that modify elements relative to the current cursor location in dynamics, or
* the data button location to accept, should pre-compute the necessary information needed
* for their _OnElementModify method in _SetupForModify. Unless this method returns
* true, _OnElementModify won't be called.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool ExampleMoveElementTool::_SetupForModify (DgnButtonEventCR ev, bool isDynamics)
    {
    DPoint3d    anchorPt;

    // Base class saves the location that was used to accept the element, selection set, or fence.
    if (!_GetAnchorPoint (&anchorPt))
        return false;

    DVec3d      vec;

    // Setup translation from accept point to this button event point.
    vec.DifferenceOf (*ev.GetPoint (), anchorPt);
    m_transform.InitFrom (vec);

    return true;
    }

/*---------------------------------------------------------------------------------**//**
* Apply the transform previously computed in _SetupForModify to the supplied element.
* Will be called both for element dynamics and on the data button to accept the modification.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
StatusInt ExampleMoveElementTool::_OnElementModify (EditElementHandleR eeh)
    {
    TransformInfo tInfo (m_transform);

    return eeh.GetHandler (MISSING_HANDLER_PERMISSION_Transform).ApplyTransform (eeh, tInfo);
    }

/*---------------------------------------------------------------------------------**//**
* Install a new instance of the tool. Will be called in response to external events
* such as undo or by the base class from _OnReinitialize when the tool needs to be
* reset to it's initial state.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void ExampleMoveElementTool::_OnRestartTool ()
    {
    InstallNewInstance (GetToolId ());
    }

/*---------------------------------------------------------------------------------**//**
* Method to create and install a new instance of the tool. If InstallTool returns ERROR,
* the new tool instance will be freed/invalid. Never call delete on RefCounted classes.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void ExampleMoveElementTool::InstallNewInstance (int toolId)
    {
    ExampleMoveElementTool* tool = new ExampleMoveElementTool (toolId);

    tool->InstallTool ();
    }

/*---------------------------------------------------------------------------------**//**
* Function that was associated with the command number for starting the tool.
*
* @param[in] unparsed Additional input supplied after command string.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
Public void startExampleMoveElementTool (WCharCP unparsed)
    {
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    ExampleMoveElementTool::InstallNewInstance (CMDNAME_ExampleMoveTool);
    }

