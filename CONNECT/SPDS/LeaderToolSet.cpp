#include "LeaderToolSet.h"
#include "TextHelper.h"

bool setCell_IsAnnotation(MSElement& e, bool value = true);
bool setCell_IsAnnotation(EditElementHandleR eeh, bool value = true);

TextBlockPtr CreateTextBlock(WCharCP styleName, DPoint3dCR origin)
{
    TextBlockPtr textBlock = NULL;

    DgnModelP activeDgnModel = TextHelper::GetActiveDgnModelP();
    DgnTextStylePtr textStyle = TextHelper::GetTextStyle(styleName);

    if (NULL != activeDgnModel && textStyle.IsValid())
    {
        textBlock = TextBlock::Create(*textStyle, *activeDgnModel);
        if (textBlock.IsValid())
        {
            textBlock->SetUserOrigin(origin);
        }
    }

    return textBlock;
}

bool LeaderToolSet::CreateCell(
    EditElementHandleR eeh, 
    bvector<DPoint3d> const& points) 
{
    if (points.size() < 2)
        return false;

    DPoint3d pt0 = points[0];
    RotMatrix rm;
    rm.InitIdentity();

    DgnModelRefR modelRef = *ACTIVEMODEL;
    // DgnModelR model = *modelRef.GetDgnModelP();
    // DgnFileR file = *modelRef.GetDgnFileP();
    
    bool Is3D = modelRef.Is3d(); 

    ElementPropertiesSetterPtr setter = ElementPropertiesSetter::Create();
    NormalCellHeaderHandler::CreateCellElement(
        eeh, L"LeaderCell", pt0, rm, Is3D, modelRef);
    // назначаем аннотативным
    setCell_IsAnnotation(eeh);
    
    {// LINE:
        EditElementHandle line;
        LineStringHandler::CreateLineStringElement(line, NULL, points.data(),
            points.size(), Is3D, modelRef);
        // текущие настройки линии
        //ElementPropertyUtils::ApplyActiveSettings(line);
        NormalCellHeaderHandler::AddChildElement(eeh, line);
    }        
    {// TEXT:
        TextBlockR textBlock = *CreateTextBlock(L"", pt0);
        textBlock.AppendText(L"#,###");            
           
    /*     TextField field;
        field.CreateForElement*/

        EditElementHandle text;
        TextHandlerBase::CreateElement(text, NULL, textBlock);

        text.GetITextEdit()->DoesSupportFields   

        NormalCellHeaderHandler::AddChildElement(eeh, text);
    }

    {
    //!* This updates the cell's origin and range diagonal
    //! WITHOUT THIS FUNCTION CELL COULDN'T BE DRAWN IN DYNAMIC!
    NormalCellHeaderHandler::AddChildComplete(eeh);
    }

    return eeh.IsValid();
}

void            ModifyTextStyle(EditElementHandleP elementToModify) {
    ITextEditP textEdit = elementToModify->GetITextEdit();           //We have made sure (in _OnPostLocate) in this case that only text elements get selected.

                                                                     // Extract the TextBlock from the Text element.
    ITextPartIdPtr textPart;
    TextBlockPtr textBlock = textEdit->GetTextPart(*elementToModify, *textPart);
    if (!textBlock.IsNull() && !textBlock->IsEmpty()) {
        CaretPtr startCaret = textBlock->CreateStartCaret();
        CaretPtr endCaret = textBlock->CreateEndCaret();
        textBlock->ApplyTextStyle(*m_textStyleObj, true, *startCaret, *endCaret);
        textBlock->PerformLayout();
        textEdit->ReplaceTextPart(*elementToModify, *textPart, *textBlock);
    }
}


void LeaderToolSet::CreateAcceptedSegmentsTransient() {
    mdlTransient_free(&msTransientElmP, true); // Use general purpose tool transient (automatically cleared when tool exits).
    EditElementHandle eeh;
    if (CreateCell(eeh, m_points)) {
        msTransientElmP = mdlTransient_addElemDescr(msTransientElmP, eeh.GetElementDescrCP(), true, 0xffff, DRAW_MODE_Normal, false, false, true);
    }
}

void LeaderToolSet::SetupAndPromptForNextAction() {
    WString     msgStr;

    RmgrResource::LoadWString(msgStr, (RscFileHandle)0, STRINGLISTID_Prompts, 1 == m_points.size() ? PROMPT_NextPoint : PROMPT_NextPointOrReset, GetMdlDesc());
    NotificationManager::OutputPrompt(msgStr.c_str());

    if (m_points.size() < 1)
        return;

    DVec3d          xVec = DVec3d::From(0.0, 0.0, 0.0);
    DPoint3d        origin = m_points.back(); // Move AccuDraw to last point in case of undo previous step.
    AccuDrawFlags   flags = ACCUDRAW_SetOrigin;

    if (m_points.size() > 1) {
        xVec = DVec3d::FromStartEndNormalize(m_points.back(), *(&m_points.back() - 1)); // Rotate AccuDraw to last segment.
        flags = (AccuDrawFlags)(flags | ACCUDRAW_SetXAxis);
    }

    AccuDraw::GetInstance().SetContext(flags, &origin, &xVec);
}

void LeaderToolSet::_OnDynamicFrame(DgnButtonEventCR ev) {

    DisplayPathCP hitPath = _DoLocate(ev, true, ComponentMode::Innermost);
    //mdlLocate_getCurrPath();

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

void LeaderToolSet::_OnUndoPreviousStep() {
    if (m_points.size() < 2)
    {
        _OnRestartTool(); // Start over from a clean initial state (i.e. Dynamics/AccuDraw not started) by installing a new tool instance.
        return;
    }

    m_points.pop_back();
    SetupAndPromptForNextAction();
    CreateAcceptedSegmentsTransient();
}

bool LeaderToolSet::_OnDataButton(DgnButtonEventCR ev) {
    if (0 == m_points.size()) {

        _BeginDynamics();
        _EndDynamics();

        AccuSnap::GetInstance().EnableLocate(true);        
        _DoLocate(ev, true, ComponentMode::Innermost);

        _BeginDynamics(); // Start dynamics on first point. Enables AccuDraw and triggers _OnDynamicFrame being called.
        EnableUndoPreviousStep(); // Enable Ctrl+Z notification to undo the previous data point.
    }
    else {
        //_EndDynamics();
        //_DoLocate(ev, true, ComponentMode::Innermost);
        //// mdlLocate_getCurrPath();
        //_BeginDynamics();
    }

    m_points.push_back(*ev.GetPoint());
    SetupAndPromptForNextAction();
    CreateAcceptedSegmentsTransient();



    return false;
}

bool LeaderToolSet::_OnResetButton(DgnButtonEventCR ev) {
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

            // итерация по дочерним элементам:
            for (ChildElemIter child(eeh); child.IsValid(); child = child.ToNext()) {
                if (child.GetElementType() == MSElementTypes::TEXT_ELM ||
                    child.GetElementType() == MSElementTypes::TEXT_NODE_ELM) {
                    DgnModelRefP modelRefP = ev.GetViewport()->GetTargetModel();
                    
                    mdlInput_sendKeyin(L"texteditor modify", false,
                        MSInputQueuePos::INPUTQ_HEAD, mdlSystem_getCurrTaskID());
                    // mdlDialog_cmdNumberQueue(FALSE, CMD_MDL_UNLOAD, mdlSystem_getCurrTaskID(), INPUTQ_EOQ);

             /*       int mdlSelect_addElement(filePos, modelRefP);
                    )*/
                    //child.GetElementCP()->text_3d.origin.get

                    //// выделение текстового поля:
                    //SelectionSetManager::GetManager().AddElement(
                    //    child.GetElementRef(), modelRefP);

                    break;                    
                }
            }

        }
    }


    _OnReinitialize(); // Exits tool if started in single-shot mode, otherwise restarts it.

    return true;
}

void LeaderToolSet::_OnPostInstall() {
    AccuSnap::GetInstance().EnableSnap(true); // Enable snapping for create tools.

    CVMonitor = CfgVarMonitor();
    BentleyStatus res = 
        ConfigurationManager::MonitorVariable(L"DgnApps", CVMonitor);
    if (res) {

    }

    __super::_OnPostInstall();
}

void LeaderToolSet::InstallNewInstance(int toolId) {
    LeaderToolSet* tool = new LeaderToolSet(toolId);

    tool->InstallTool();
}


Public void startLeaderToolSet(WCharCP unparsed)
{
    // NOTE: Call the method to create/install the tool, RefCounted classes don't have public constructors...
    LeaderToolSet::InstallNewInstance(CMDNAME_LeaderToolSet);
}



bool setCell_IsAnnotation(MSElement& e, bool value) {
    bool is3d = e.hdr.dhdr.props.b.is3d;
    if (is3d) {
        e.cell_3d.flags.matchSourceAnnotationSize = 1;
        return (
            (e.cell_3d.flags.isAnnotation = value) == value);
    }
    else {
        e.cell_2d.flags.matchSourceAnnotationSize = 1;
        return (
            (e.cell_2d.flags.isAnnotation = value) == value);
    }
}

bool setCell_IsAnnotation(EditElementHandleR eeh, bool value) {
    /*
    Как вариант можно использовать и C-функцию mdlCell_setIsAnnotation,
    для этого её необходимо объявить след. образом:
    extern "C" {
    StatusInt mdlCell_setIsAnnotation(MSElementDescrP cellEdP, bool isAnnotation);
    }
    */

    return setCell_IsAnnotation(*eeh.GetElementP(), value);
}
