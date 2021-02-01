/*--------------------------------------------------------------------------------------+
|
|     $Source: MstnExamples/Elements/exampletool/ExampleViewTool.cpp $
|
|  $Copyright: (c) 2015 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------*/
#include "exampletool.h"

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

/*=================================================================================**//**
* Example showing how to use DgnViewTool to write a viewing tool.
* 
* A simple viewing tool example that zooms the view and centers the accept.
* Demonstrates using IndexedViewSet and IViewManager to update the view.
* By default a DgnViewTool will exit on a reset button event.
* 
* @bsiclass                                                               Bentley Systems
+===============+===============+===============+===============+===============+======*/
struct          ExampleViewTool : DgnViewTool
{
protected:

ExampleViewTool (int toolName, int toolPrompt) : DgnViewTool (toolName, toolPrompt) {}

virtual bool _OnDataButton (DgnButtonEventCR ev) override;

public:

static void InstallNewInstance (int toolId, int toolPrompt);

}; // ExampleViewTool

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool ExampleViewTool::_OnDataButton (DgnButtonEventCR ev)
    {
    IndexedViewportP  vp = ev.GetViewport ();

    if (!vp)
        return false; // Sub-classes may ascribe special meaning to this status, it's not checked by DgnViewTool.

    DPoint3d  centerPt = *ev.GetPoint (); // The DgnButtonEvent point is always ACTIVE coords.

    vp->ActiveToRoot (&centerPt, &centerPt, 1); // Need point in ROOT coords for new zoom center (ACTIVE != ROOT when a reference is activated).
    vp->Zoom (&centerPt, 0.5, true); // Change the frustum for this viewport by tje supplied scale factor, does not update the view.
    vp->SynchWithViewInfo (true, true); // Add entry to view undo stack so that view previous can nr used to get back to the current view.

    IndexedViewSet::FullUpdateInfo  info;

    info.SetStartEndMsg (true); // View tools should output progress/display complete messages...
    IViewManager::GetActiveViewSet ().UpdateView (vp, DRAW_MODE_Normal, DrawPurpose::Update, info); // Update the display by doing a full update.

    return true;
    }

/*---------------------------------------------------------------------------------**//**
* Method to create and install a new instance of the tool. If InstallTool returns ERROR,
* the new tool instance will be freed/invalid. Never call delete on RefCounted classes.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void ExampleViewTool::InstallNewInstance (int toolId, int toolPrompt)
    {
    ExampleViewTool* tool = new ExampleViewTool (toolId, toolPrompt);

    tool->InstallTool ();
    }

/*---------------------------------------------------------------------------------**//**
* Function that was associated with the command number for starting the tool.
*
* @param[in] unparsed Additional input supplied after command string.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
Public void startExampleViewTool (WCharCP unparsed)
    {
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    ExampleViewTool::InstallNewInstance (CMDNAME_ExampleViewTool, PROMPT_SelectView);
    }

