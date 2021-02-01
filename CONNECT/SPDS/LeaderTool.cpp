/*--------------------------------------------------------------------------------------+
|
|     $Source: ToolCreateTool.cpp $
|
+--------------------------------------------------------------------------------------*/
#include "spds.h"

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
struct LeaderTool : DgnPrimitiveTool { // DgnElementSetTool
protected:

    bvector<DPoint3d>   m_points;

    LeaderTool (int toolName, int toolPrompt) : DgnPrimitiveTool (toolName, toolPrompt) {}

    virtual void _OnPostInstall () override;
    virtual void _OnRestartTool () override {InstallNewInstance (GetToolId (), GetToolPrompt ());}
    virtual bool _OnDataButton (DgnButtonEventCR ev) override;
    virtual bool _OnResetButton (DgnButtonEventCR ev) override;
    virtual void _OnDynamicFrame (DgnButtonEventCR ev) override;
    virtual void _OnUndoPreviousStep () override;

    bool CreateElement (EditElementHandleR eeh, bvector<DPoint3d> const& points);
    //bool CreateCell(MSElementDescrP eDP, bvector<DPoint3d>& points);
    //bool CreateCell(EditElementHandleR eeh, bvector<DPoint3d>& points);
    //bool CreateCell(bvector<DPoint3d>& points);
    bool CreateCell(EditElementHandleR eeh, bvector<DPoint3d> const& points);
    bool CreateCell(MSElementDescrH edPP, bvector<DPoint3d>& points);

    void CreateAcceptedTransient();
    void SetupAndPromptForNextAction();

public:

    static void InstallNewInstance (int toolId, int toolPrompt);

}; // ExampleCreateLineStringTool


//bool setCell_IsAnnotation(MSElement& e, bool value = true) {
//    bool is3d = e.hdr.dhdr.props.b.is3d;    
//    if (is3d) {
//        e.cell_3d.flags.matchSourceAnnotationSize = 1;
//        return (
//            (e.cell_3d.flags.isAnnotation = value) == value);
//    }
//    else {
//        e.cell_2d.flags.matchSourceAnnotationSize = 1;
//        return (
//            (e.cell_2d.flags.isAnnotation = value) == value);
//    }
//}
//bool setCell_IsAnnotation(EditElementHandleR eeh, bool value = true) {
//    /*
//        Как вариант можно использовать и C-функцию mdlCell_setIsAnnotation,
//        для этого её необходимо объявить след. образом:
//        extern "C" {
//            StatusInt mdlCell_setIsAnnotation(MSElementDescrP cellEdP, bool isAnnotation);
//        }
//    */
//
//    return setCell_IsAnnotation(*eeh.GetElementP(), value);
//}

bool LeaderTool::CreateCell(EditElementHandleR eeh, bvector<DPoint3d> const& points) {
    if (points.size() < 2)
        return false;

    DPoint3d pt0 = points[0];
    RotMatrix rm;
    rm.InitIdentity();

    bool Is3D = ACTIVEMODEL->Is3d();

    ElementPropertiesSetterPtr setter = ElementPropertiesSetter::Create();
    NormalCellHeaderHandler::CreateCellElement(
        eeh, L"LeaderCell", points[0], rm, Is3D, *ACTIVEMODEL);
    EditElementHandle c_eeh;
    NormalCellHeaderHandler::CreateCellElement(
        c_eeh, L"cell", pt0, rm, Is3D, *ACTIVEMODEL);
    // назначаем аннотативным
    //setCell_IsAnnotation(c_eeh);
    
    EditElementHandle line;
    LineStringHandler::CreateLineStringElement(line, NULL, points.data(),
        points.size(), Is3D, *ACTIVEMODEL);
    // текущие настройки линии
    //ElementPropertyUtils::ApplyActiveSettings(line);
    NormalCellHeaderHandler::AddChildElement(c_eeh, line);
    NormalCellHeaderHandler::AddChildComplete(c_eeh); //!*
        
    NormalCellHeaderHandler::AddChildElement(eeh, c_eeh);
    {
        //!* This updates the cell's origin and range diagonal
        //! WITHOUT THIS FUNCTION CELL COULDN'T BE DRAWN IN DYNAMIC!
        NormalCellHeaderHandler::AddChildComplete(eeh);       
    }

    return eeh.IsValid();;
}
 
bool CreateCell__By_Descr(EditElementHandleR eeh, bvector<DPoint3d>& points) {
    if (points.size() < 2)
        return false;

    MSElement el;
    MSElementDescrP edP = NULL;

    mdlCell_create(&el, L"LeaderCell", &points[0], false);
    //mdlLineString_create(&el, NULL, points.data(), static_cast<int>(points.size()));
    mdlElmdscr_new(&edP, NULL, &el);

    {
        //MSElementDescrP child_edP = NULL;
        //mdlCell_create(&el, L"Cell", &points[0], false);
        //(*child_edP).el.cell_3d.flags.isAnnotation = 1;

        //mdlElmdscr_new(&child_edP, NULL, &el);
        //mdlElmdscr_appendDscr(edP, child_edP);

        mdlLineString_create(&el, NULL, points.data(), static_cast<int>(points.size()));
        mdlElmdscr_appendElement(edP, &el);
    }
    edP->h.isHeader = 1;
    edP->h.dgnModelRef = ACTIVEMODEL; // This line is IMPORTANT !
    eeh.SetElementDescr(edP, true, false);

    {
        //! This updates the cell's origin and range diagonal
        //! WITHOUT THIS FUNCTION CELL COULDN'T BE DRAWN IN DYNAMIC!
        NormalCellHeaderHandler::AddChildComplete(eeh);
    }
   
    //mdlElmdscr_freeAll(&edP);
    ElementPropertyUtils::ApplyActiveSettings(eeh);
    return eeh.IsValid();;
}

bool CreateCell_By_DescrH(MSElementDescrH edPP, bvector<DPoint3d>& points) {
    if (points.size() < 2)
        return false;

    //MSElementDescrP outerElem = NULL;
    //memset(outerElem, 0, sizeof(outerElem));
  
    ////eeh.GetElementDescrP(); // Cause the handle to create and hold a descriptor.
    // MSElementDescrP edP = eeh.ExtractElementDescr(); // take ownership of descriptor away from eh
    MSElement _el;
    mdlCell_create(&_el, L"LeaderCell", &points[0], false);
    mdlElmdscr_new(edPP, NULL, &_el);
    
    {
        MSElementDescrP child_edP = NULL;
        mdlCell_create(&_el, L"Cell", &points[0], false);
        (*child_edP).el.cell_3d.flags.isAnnotation = 1;

        mdlElmdscr_new(&child_edP, NULL, &_el);
        mdlElmdscr_appendDscr(*edPP, child_edP);

        mdlLineString_create(&_el, NULL, points.data(), static_cast<int>(points.size()));
        mdlElmdscr_appendElement(child_edP, &_el);
    }    

    //MSElementDescrH edPPchild = NULL;
    //mdlElmdscr_new(edPPchild, NULL, &_el);

    // mdlLineString_create(&_el, NULL, points.data(), static_cast<int>(points.size()));
    // mdlElmdscr_appendElement(*edPP, &_el);

    //mdlElmdscr_appendDscr(*edPP, *edPPchild);
    //mdlElmdscr_freeAll(edPPchild);
    
    return true;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool LeaderTool::CreateElement (EditElementHandleR eeh, bvector<DPoint3d> const& points) {
    if (points.size() < 2)
        return false;

    // NOTE: Create a line, linestring, or complex string based on point count.
    if (SUCCESS != DraftingElementSchema::ToElement(eeh, *ICurvePrimitive::CreateLineString(points), NULL, ACTIVEMODEL->Is3d(), *ACTIVEMODEL))
        return false;
    
    //ElementPropertyUtils::ApplyActiveSettings(eeh);

    return true;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void LeaderTool::CreateAcceptedTransient () {
    //EditElementHandle   eeh;

    mdlTransient_free (&msTransientElmP, true); // Use general purpose tool transient (automatically cleared when tool exits).
    EditElementHandle eeh;
    if (CreateCell(eeh, m_points)) {
        msTransientElmP = mdlTransient_addElemDescr(msTransientElmP, eeh.GetElementDescrCP(), true, 0xffff, DRAW_MODE_Normal, false, false, true);
    }
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void LeaderTool::SetupAndPromptForNextAction () {
    WString     msgStr;

    RmgrResource::LoadWString (msgStr, (RscFileHandle) 0, STRINGLISTID_Prompts, 1 == m_points.size () ? PROMPT_NextPoint : PROMPT_NextPointOrReset, GetMdlDesc ());
    NotificationManager::OutputPrompt (msgStr.c_str ());

    if (m_points.size () < 1)
        return;

    DVec3d          xVec   = DVec3d::From (0.0, 0.0, 0.0); 
    DPoint3d        origin = m_points.back (); // Move AccuDraw to last point in case of undo previous step.
    AccuDrawFlags   flags  = ACCUDRAW_SetOrigin;

    if (m_points.size () > 1) {
        xVec = DVec3d::FromStartEndNormalize (m_points.back(), *(&m_points.back() - 1)); // Rotate AccuDraw to last segment.
        flags = (AccuDrawFlags) (flags | ACCUDRAW_SetXAxis);
    }

    AccuDraw::GetInstance().SetContext (flags, &origin, &xVec);
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void LeaderTool::_OnDynamicFrame (DgnButtonEventCR ev) {

    DisplayPathCP hitPath = mdlLocate_getCurrPath();
    if (NULL != hitPath) {
        ElementHandle elem(hitPath->GetComponentElem(), hitPath->GetRoot());
        FindInstancesScopePtr scope = FindInstancesScope::CreateScope(elem, FindInstancesScopeOption(DgnECHostType::Element));
        ECQueryPtr query = ECQuery::CreateQuery(ECQUERY_PROCESS_SearchAllClasses);

        WString msg(L"Cursor is on ElementId = ");
        msg += elem.GetElementId();
        NotificationManager::OutputPrompt(msg.c_str());

        //NotifyMessageDetails details(OutputMessagePriority::Info, str.c_str(), L"");
        //NotificationManager::OutputMessage(details);
    }
    else {
        NotificationManager::OutputPrompt(WString(L"do action").c_str());
    }

    if (m_points.size() < 1)
        return;


    bvector<DPoint3d> tmpPts;
    tmpPts.push_back(m_points.back()); // Only draw the current segment in dynamics, the accepted segments drawn as transients.
    tmpPts.push_back(*ev.GetPoint());

    EditElementHandle eeh;
    if (CreateCell(eeh, tmpPts)) {
        RedrawElems redrawElems;
        redrawElems.SetDynamicsViews(IViewManager::GetActiveViewSet(), ev.GetViewport());
        redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
        redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);
        redrawElems.DoRedraw(eeh);
    }
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void LeaderTool::_OnUndoPreviousStep () {
    if (m_points.size () < 2)
        {
        _OnRestartTool (); // Start over from a clean initial state (i.e. Dynamics/AccuDraw not started) by installing a new tool instance.
        return;
        }

    m_points.pop_back ();
    SetupAndPromptForNextAction ();
    CreateAcceptedTransient ();
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool LeaderTool::_OnDataButton (DgnButtonEventCR ev) {
    if (0 == m_points.size ()) {       
        _BeginDynamics (); // Start dynamics on first point. Enables AccuDraw and triggers _OnDynamicFrame being called.

        EnableUndoPreviousStep (); // Enable Ctrl+Z notification to undo the previous data point.
    }
    else {
        _EndDynamics();
        mdlLocate_getCurrPath();
        _BeginDynamics();
    }

    m_points.push_back(*ev.GetPoint ());
    SetupAndPromptForNextAction();
    CreateAcceptedTransient();

    return false;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool LeaderTool::_OnResetButton (DgnButtonEventCR ev) {
    //MSElementDescr* cellDscrP = NULL;

    EditElementHandle eeh;
    if (CreateCell(eeh, m_points)) {
            //MSElementDescrP edP = eeh.ExtractElementDescr();
            //mdlElmdscr_display(edP, ACTIVEMODEL, DRAW_MODE_Normal);
            //mdlElmdscr_add(edP);
            //eeh.SetElementDescr(edP, true, false);

        StatusInt res = eeh.AddToModel();
        //if (res == 0x11013) {
        //}
        //
        if (SUCCESS != res) { // Add new line string element to active model. The place linestring tool uses a reset to accept.
            // error
            BeAssert(eeh.GetElementRef() == NULL);
            BeAssert(eeh.GetElementRef() != NULL);
        }            
        else
        {
            BeAssert(eeh.GetElementRef() != NULL);
            BeAssert(eeh.IsPersistent());
            BeAssert(eeh.PeekElementDescrCP() == NULL); // The original descriptor has been discarded and freed!
        }
    }

    _OnReinitialize (); // Exits tool if started in single-shot mode, otherwise restarts it.

    return true;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void LeaderTool::_OnPostInstall () {
    AccuSnap::GetInstance ().EnableSnap (true); // Enable snapping for create tools.

    __super::_OnPostInstall ();
}

/*---------------------------------------------------------------------------------**//**
* Method to create and install a new instance of the tool. If InstallTool returns ERROR,
* the new tool instance will be freed/invalid. Never call delete on RefCounted classes.
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void LeaderTool::InstallNewInstance (int toolId, int toolPrompt) {
    LeaderTool* tool = new LeaderTool (toolId, toolPrompt);

    tool->InstallTool ();
}

/*---------------------------------------------------------------------------------**//**
* Function that was associated with the command number for starting the tool.
*
* @param[in] unparsed Additional input supplied after command string.
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
Public void startLeaderTool(WCharCP unparsed)
{
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    LeaderTool::InstallNewInstance (CMDNAME_LeaderTool, PROMPT_FirstPoint);
}