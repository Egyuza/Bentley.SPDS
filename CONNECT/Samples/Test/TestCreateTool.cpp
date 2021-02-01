/*--------------------------------------------------------------------------------------+
|
|     $Source: ToolCreateTool.cpp $
|
+--------------------------------------------------------------------------------------*/
#include "testtool.h"

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

/*=================================================================================**//**
* Example showing how to use DgnPrimitiveTool to write a place line string tool.
*
* Demonstrates using transients for snapping to the previously accepted segments 
* during placement dynamics as well as using Ctrl+Z to undo the previous data point.
* 
* @bsiclass                                                               Bentley Systems
+===============+===============+===============+===============+===============+======*/
struct          TestCreateLineStringTool : DgnPrimitiveTool
{
protected:

bvector<DPoint3d>   m_points;

TestCreateLineStringTool (int toolName, int toolPrompt) : DgnPrimitiveTool (toolName, toolPrompt) {}

virtual void _OnPostInstall () override;
virtual void _OnRestartTool () override {InstallNewInstance (GetToolId (), GetToolPrompt ());}
virtual bool _OnDataButton (DgnButtonEventCR ev) override;
virtual bool _OnResetButton (DgnButtonEventCR ev) override;
virtual void _OnDynamicFrame (DgnButtonEventCR ev) override;
virtual void _OnUndoPreviousStep () override;

bool CreateElement (EditElementHandleR eeh, bvector<DPoint3d> const& points);
void CreateAcceptedSegmentsTransient ();
void SetupAndPromptForNextAction ();

public:

static void InstallNewInstance (int toolId, int toolPrompt);

}; // ExampleCreateLineStringTool

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool TestCreateLineStringTool::CreateElement (EditElementHandleR eeh, bvector<DPoint3d> const& points)
    {
    if (points.size () < 2)
        return false;

    // NOTE: Create a line, linestring, or complex string based on point count.
    if (SUCCESS != DraftingElementSchema::ToElement (eeh, *ICurvePrimitive::CreateLineString (points), NULL, ACTIVEMODEL->Is3d (), *ACTIVEMODEL))
        return false;

    ElementPropertyUtils::ApplyActiveSettings (eeh);

    return true;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void TestCreateLineStringTool::CreateAcceptedSegmentsTransient ()
    {
    EditElementHandle   eeh;

    mdlTransient_free (&msTransientElmP, true); // Use general purpose tool transient (automatically cleared when tool exits).

    if (CreateElement (eeh, m_points))
        msTransientElmP = mdlTransient_addElemDescr (msTransientElmP, eeh.GetElementDescrP (), true, 0xffff, DRAW_MODE_Normal, false, false, true);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void TestCreateLineStringTool::SetupAndPromptForNextAction ()
    {
    WString     msgStr;

    RmgrResource::LoadWString (msgStr, (RscFileHandle) 0, STRINGLISTID_Prompts, 1 == m_points.size () ? PROMPT_NextPoint : PROMPT_NextPointOrReset, GetMdlDesc ());
    NotificationManager::OutputPrompt (msgStr.c_str ());

    if (m_points.size () < 1)
        return;

    DVec3d          xVec   = DVec3d::From (0.0, 0.0, 0.0); 
    DPoint3d        origin = m_points.back (); // Move AccuDraw to last point in case of undo previous step.
    AccuDrawFlags   flags  = ACCUDRAW_SetOrigin;

    if (m_points.size () > 1)
        {
        xVec = DVec3d::FromStartEndNormalize (m_points.back (), *(&m_points.back ()-1)); // Rotate AccuDraw to last segment.
        flags = (AccuDrawFlags) (flags | ACCUDRAW_SetXAxis);
        }

    AccuDraw::GetInstance ().SetContext (flags, &origin, &xVec);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void TestCreateLineStringTool::_OnDynamicFrame (DgnButtonEventCR ev)
    {
    if (m_points.size () < 1)
        return;

    bvector<DPoint3d>   tmpPts;
    EditElementHandle   eeh;

    tmpPts.push_back (m_points.back ()); // Only draw the current segment in dynamics, the accepted segments drawn as transients.
    tmpPts.push_back (*ev.GetPoint ());

    if (!CreateElement (eeh, tmpPts))
        return;

    RedrawElems redrawElems;

    redrawElems.SetDynamicsViews (IViewManager::GetActiveViewSet (), ev.GetViewport ()); // Display in all views, draws to cursor view first...
    redrawElems.SetDrawMode (DRAW_MODE_TempDraw);
    redrawElems.SetDrawPurpose (DrawPurpose::Dynamics);

    redrawElems.DoRedraw (eeh);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void TestCreateLineStringTool::_OnUndoPreviousStep ()
    {
    if (m_points.size () < 2)
        {
        _OnRestartTool (); // Start over from a clean initial state (i.e. Dynamics/AccuDraw not started) by installing a new tool instance.
        return;
        }

    m_points.pop_back ();
    SetupAndPromptForNextAction ();
    CreateAcceptedSegmentsTransient ();
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool TestCreateLineStringTool::_OnDataButton (DgnButtonEventCR ev)
    {
    if (0 == m_points.size ())
        {
        _BeginDynamics (); // Start dynamics on first point. Enables AccuDraw and triggers _OnDynamicFrame being called.
        EnableUndoPreviousStep (); // Enable Ctrl+Z notification to undo the previous data point.
        }

    m_points.push_back (*ev.GetPoint ());
    SetupAndPromptForNextAction ();
    CreateAcceptedSegmentsTransient ();

    return false;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool TestCreateLineStringTool::_OnResetButton (DgnButtonEventCR ev)
    {
    EditElementHandle   eeh;

    if (CreateElement (eeh, m_points))
        eeh.AddToModel (); // Add new line string element to active model. The place linestring tool uses a reset to accept.

    _OnReinitialize (); // Exits tool if started in single-shot mode, otherwise restarts it.

    return true;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void TestCreateLineStringTool::_OnPostInstall ()
    {
    AccuSnap::GetInstance ().EnableSnap (true); // Enable snapping for create tools.

    __super::_OnPostInstall ();
    }

/*---------------------------------------------------------------------------------**//**
* Method to create and install a new instance of the tool. If InstallTool returns ERROR,
* the new tool instance will be freed/invalid. Never call delete on RefCounted classes.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void TestCreateLineStringTool::InstallNewInstance (int toolId, int toolPrompt)
    {
    TestCreateLineStringTool* tool = new TestCreateLineStringTool (toolId, toolPrompt);

    tool->InstallTool ();
    }

/*---------------------------------------------------------------------------------**//**
* Function that was associated with the command number for starting the tool.
*
* @param[in] unparsed Additional input supplied after command string.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
Public void startTestCreateLineStringTool(WCharCP unparsed)
{
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    TestCreateLineStringTool::InstallNewInstance (CMDNAME_TestLStringTool, PROMPT_FirstPoint);
}