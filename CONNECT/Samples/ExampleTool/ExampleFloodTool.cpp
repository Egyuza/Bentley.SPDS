/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/ExampleFloodTool.cpp $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "exampletool.h"

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

/*=================================================================================**//**
* Example showing how to use DgnRegionElementTool to write a CREATE REGION by FLOOD tool.
* 
* The base class handles collecting the region boundary from the elements in the view
* or selection set. The sub-class is responsible for doing something with the new region
* and optionally processing the original elements that comprise the boundary.
*
* @bsiclass                                                               Bentley Systems
+===============+===============+===============+===============+===============+======*/
struct          ExampleFloodElementTool : DgnRegionElementTool
{
private:

RegionParams    m_regionParams;

protected:

//! @param[in] toolId Set tool command name identifier.
ExampleFloodElementTool (int toolId) : DgnRegionElementTool () {SetCmdName (toolId, 0);}

virtual bool                _WantFloodDynamicArea () override {return true;} // Show valid flood region under the cursor without waiting for accept point.
virtual RegionCreateMode    _GetRegionCreateMode () override {return REGION_CREATE_ByParams;} // Create region using _GetRegionParams.
virtual RegionParams const& _GetRegionParams () override; // Setup region params to create region by flood (or boolean).
virtual BentleyStatus       _OnProcessRegionResult (EditElementHandleR eeh) override; // Process a region element created by this tool.

virtual void _OnRestartTool () override; // pure virtual method, sub-class must override!

public:

static void InstallNewInstance (int toolId);

}; // ExampleFloodElementTool

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
RegionParams const& ExampleFloodElementTool::_GetRegionParams ()
    {
    m_regionParams.SetType (RegionType::Flood); // Create region by flood using seed point.
    m_regionParams.SetFloodParams (RegionLoops::Outer, 0.0); // Allow regions to be created with holes.

    return m_regionParams;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
BentleyStatus ExampleFloodElementTool::_OnProcessRegionResult (EditElementHandleR eeh)
    {
    ElementPropertyUtils::ApplyActiveSettings (eeh); // Apply active symbology to new region element.
    ElementPropertyUtils::ApplyActiveAreaSettings (eeh, -1); // Apply active area fill to new region element.
    eeh.AddToModel ();

    return SUCCESS;
    }

/*---------------------------------------------------------------------------------**//**
* Install a new instance of the tool. Will be called in response to external events
* such as undo or by the base class from _OnReinitialize when the tool needs to be
* reset to it's initial state.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void ExampleFloodElementTool::_OnRestartTool ()
    {
    InstallNewInstance (GetToolId ());
    }

/*---------------------------------------------------------------------------------**//**
* Method to create and install a new instance of the tool. If InstallTool returns ERROR,
* the new tool instance will be freed/invalid. Never call delete on RefCounted classes.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void ExampleFloodElementTool::InstallNewInstance (int toolId)
    {
    ExampleFloodElementTool* tool = new ExampleFloodElementTool (toolId);

    tool->InstallTool ();
    }

/*---------------------------------------------------------------------------------**//**
* Function that was associated with the command number for starting the tool.
*
* @param[in] unparsed Additional input supplied after command string.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
Public void startExampleFloodElementTool (WCharCP unparsed)
    {
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    ExampleFloodElementTool::InstallNewInstance (CMDNAME_ExampleFloodTool);
    }

