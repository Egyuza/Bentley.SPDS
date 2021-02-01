#include "LeaderSetTool.h"

STEP operator ++(STEP& step) {
    step = static_cast<STEP>(step + 1);
    return step;
}
STEP operator --(STEP& step) {
    step = static_cast<STEP>(step - 1);
    return step;
}

// TODO разобраться до конца с инициализацией LeaderSetTool
LeaderSetTool::LeaderSetTool() // (int cmdNumber, int cmdName)
    : //: MstnPrimitiveTool(cmdNumber, cmdName, 0),
    step_(STEP_START),
    arrowRadius_(0.5),
    textOffset_(1.0), // TODO отступ текста 1.0
    isNextArrowFreeAngle_(false),
    isReadyToPublish_(false),
    shouldLeadersFromOnePoint_(true),
    nArrows_(0)
{
    { // определение id стиля текста:
        MdlTextStyle* styleP = NULL;
        
        char styleNameP[MAX_STR_LEN];
        wchar_t styleNameWP[MAX_STR_LEN];
        int res = mdlSystem_getCfgVar(styleNameP, CFGV_TEXTSTYLE, MAX_STR_LEN);
        if (SUCCESS == res) {
            // mdlOutput_messageCenter(MESSAGE_INFO, styleNameP, styleNameP, TRUE);
            mbstowcs(styleNameWP, styleNameP, MAX_STR_LEN); // конвертация строки
            res = mdlTextStyle_getByName(&styleP, &textStyleId_, styleNameWP, ACTIVEMODEL, TRUE);
        }
        if (SUCCESS != res) {
            textStyleId_ = getDefaultTextStylePtr()->GetID();
            mdlTextStyle_getById(&styleP, styleNameWP, MAX_STR_LEN, textStyleId_, ACTIVEMODEL);
            

            //char styleIdStr[(((sizeof textStyleId) * CHAR_BIT) + 2) / 3 + 2];
            //sprintf(styleIdStr, "%d", textStyleId);
            //mdlOutput_messageCenter(MESSAGE_INFO, styleIdStr, styleIdStr, TRUE);
        }
        textStyleHeight_ = mdlTextStyle_getHeight(styleP);
        textStyleLineSpacing_ = mdlTextStyle_getLineSpacing(styleP);
    }
}

void LeaderSetTool::setInputParams(const TextGrabParam& params) {
    grabParams_ = params;

    // TODO setInputParams

    //origin_ = grabParams_.origin;
    //SetAccudrawContext(ACCUDRAW_SetOrigin | ACCUDRAW_SetModePolar, &origin_);
    //BeginComplexDynamics();
    //++currentStep_ ;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
//clean up when this instance is done (another command has been fired)
void LeaderSetTool::OnCleanup () {
    // TODO все действия по очистке памяти, т.к. устанавливается новый тул
    elements_.clear();   
}

/*------------------------------------------------------------------------------
Перезапуск тула
----------------------------------------------------------------------------- */
void LeaderSetTool::OnRestartCommand () {
    mdlDialog_cmdNumberQueue(true, CMD_SPDS_LEADER_PLACE, "", FALSE);
}

StatusInt LeaderSetTool::OnElementModify(EditElemHandleR elHandle) {
    return ERROR;
}

bool LeaderSetTool::OnModifierKeyTransition(bool wentDown, int key) {
    switch (key) {
    case ALTKEY:
        // TODO on ctrl key
        break;
    case CTRLKEY:
        // TODO on alt key
        break;
    default:
        break;
    }
    return true;
}

/*------------------------------------------------------------------------------
Реализация динамики процесса построения
----------------------------------------------------------------------------- */
void LeaderSetTool::OnComplexDynamics (MstnButtonEventCP ev) {   

    ElementList elems;
    
    createElementsOnStep(elems, ev);
    if (step_ < STEP_FIRST_ARROW_POINT2) {
        createTextGroupElems(elems, *ev->GetPoint());
    }
    else if (step_ < STEP_TEXT) {
        createTextGroupElems(elems, textLineOrigin_);
    }

    if (elems.size() > 0) {
        //printf ("should be in dynamics %s \n",m_str);
        RedrawElems redrawTool;
        redrawTool.SetDrawMode(DRAW_MODE_TempDraw);
        redrawTool.SetDrawPurpose(DRAW_PURPOSE_Dynamics);
        redrawTool.SetViews(0xffff);

        for (ElementList::iterator it = elems.begin();
            it != elems.end(); ++it)
        {
            redrawTool.DoRedraw(*it);
        }
    }
}

/*------------------------------------------------------------------------------
Ввод точки построения пользователем - ЛКМ
----------------------------------------------------------------------------- */
//what to do on the data point
bool LeaderSetTool::OnDataButton(MstnButtonEventCP ev) {
    DPoint3dCR dataPoint = *ev->GetPoint();

    if (step_)
    { // запоминаем уже построенные элементы, и публикуем их как временные:
        ElementList elems;
        if (createElementsOnStep(elems, ev)) {
            if (step_ == STEP_FIRST_ARROW_POINT2) {
                // удаляем предыдущую линию:
                elements_.pop_back();
            }
            elements_.insert(elements_.end(), elems.begin(), elems.end());
            CreateAcceptedTransient();
            // return true;
        }
    }
        
    switch (step_) {
    case STEP_START:
        origin_ = dataPoint;
        SetAccudrawContext(ACCUDRAW_SetOrigin | ACCUDRAW_SetModePolar, &origin_);
        BeginComplexDynamics();
    case STEP_FIRST_ARROW_POINT1:
        curArrowPoints_[step_] = dataPoint;
        SetAccudrawContext(ACCUDRAW_SetModeRect);
        break;
    case STEP_FIRST_ARROW_POINT2:
    {
        textLineOrigin_ = getHorOrVertProjectionPoint(
            curArrowPoints_[step_ - 1], dataPoint);
        curArrowPoints_[step_] = textLineOrigin_;
        // запоминаем координаты первой линии-указателя
        memcpy(firstArrowPoints_, curArrowPoints_, sizeof(firstArrowPoints_));
        SetAccudrawContext(
            ACCUDRAW_SetModeRect | ACCUDRAW_SetOrigin, 
            &textLineOrigin_
        );
        break;
    }
    case STEP_TEXT:
        // mdlOutput_keyin("edit text");
    case STEP_NEXT_ARROW:
        mdlSelect_freeAll();
        mdlSelect_addElementByRef(textEeh_.GetElemRef(), ACTIVEMODEL, 2, SELECT_NEW, TRUE);

        // mdlInput_sendKeyin("WORD EDIT TEXT", 0, INPUTQ_EOQ, 0);
        // keyin:
        // Place DIALOGTEXT ICON

        ++nArrows_;
        isNextArrowFreeAngle_ = false; // сброс флага произвольного угла линии указателя

        if (shouldLeadersFromOnePoint_) {
            SetAccudrawContext(
                ACCUDRAW_SetModePolar | ACCUDRAW_SetOrigin,
                &firstArrowPoints_[1]
            );
        }
        else {
            SetAccudrawContext(
                ACCUDRAW_SetModeRect | ACCUDRAW_SetOrigin, 
                &firstArrowPoints_[0]
            );
        }
        break;
    default:
        break;
    }
    
    if (step_ != STEP_NEXT_ARROW) {
        // переход на след. шаг построения
        ++step_;
    }
    return true;
}

/*------------------------------------------------------------------------------
событе нажатия ПКМ
----------------------------------------------------------------------------- */
bool LeaderSetTool::OnResetButton(MstnButtonEventCP ev) {
   
    if (step_ == STEP_NEXT_ARROW) {
        if (shouldLeadersFromOnePoint_) {
            shouldLeadersFromOnePoint_ = false;
            SetAccudrawContext(
                ACCUDRAW_SetModeRect | ACCUDRAW_SetOrigin, 
                &firstArrowPoints_[0]
            );
        }
        else if (!isNextArrowFreeAngle_) {
            // сбрасывается установка параллельности очедной линии указателя  
            // относительно первой, и включаем флаг произвольного угла:
            isNextArrowFreeAngle_ = true;
            SetAccudrawContext(
                ACCUDRAW_SetModePolar | ACCUDRAW_SetOrigin, &curArrowPoints_[1]);
        }
        else {
        // Публикация в модель:
            if (SUCCESS == CreateCellAndPublish()) {
                OnReinitialize();
            }
        }
    }
    else {
        OnReinitialize();
    }
    return true;
}

/*------------------------------------------------------------------------------
Построение элементов на текущем шаге
----------------------------------------------------------------------------- */
bool LeaderSetTool::
createElementsOnStep(ElementList& elems, MstnButtonEventCP ev) {
    
    DPoint3dCR dataPoint = *ev->GetPoint(); // координаты курсора
    
    switch (step_) {
    case STEP_START:
        break;
    case STEP_FIRST_ARROW_POINT1:
    {
        EditElemHandle cirle;
        if (CreateCircle(cirle, curArrowPoints_[0], arrowRadius_, TRUE)) {
            elems.push_back(cirle);
        }
        EditElemHandle line;
        curArrowPoints_[1] = dataPoint;
        if (CreateStringLine(line, curArrowPoints_, 2)) {
            elems.push_back(line);
        }
        break;
    }
    case STEP_FIRST_ARROW_POINT2:
    {
        // заканчиваем рисование первой линии-указателя
        // выводим ориентацию базовой коллекторной линии указателей
        // ориентация задаётся либо горизонтально либо вертикально

        EditElemHandle line;
        curArrowPoints_[2] = getHorOrVertProjectionPoint(curArrowPoints_[1], dataPoint);
        textLineOrigin_ = curArrowPoints_[2];
        CreateStringLine(line, curArrowPoints_, 3);
        elems.push_back(line);
        break;
    }
    case STEP_TEXT:
    { 
        // полка с текстом        
        createTextGroupElems(elems, textLineOrigin_, ev->GetPoint(), textOrigin_);
        break;
    }
    case STEP_NEXT_ARROW:
    {
        curArrowPoints_[0] = dataPoint;

        if (shouldLeadersFromOnePoint_) {
        // все указатели изходят из одной точки:
            curArrowPoints_[1] = firstArrowPoints_[1];
        }
        else if (isNextArrowFreeAngle_) {
        // произвольный угол:
            // вторая точка фиксируется в OnComplexDynamics           
        }
        else {
        // ПАРАЛЛЕЛЬНО первой линии-указателю:
            DPoint3d& interceptPoint = curArrowPoints_[1];
            {
                DVector3d v_base, v;
                DPoint3d pt;
                pt = firstArrowPoints_[1];
                pt.x += (dataPoint.x - firstArrowPoints_[0].x);
                pt.y += (dataPoint.y - firstArrowPoints_[0].y);
                v.org = dataPoint;
                v.end = pt;

                v_base.org = firstArrowPoints_[1];
                v_base.end = textLineOrigin_;

                if (SUCCESS != mdlVec_intersect(&interceptPoint, &v, &v_base)) {
                    return false;
                }
            }
        }

        //! последние точки все сводятся в одной базовой
        curArrowPoints_[2] = textLineOrigin_;

        EditElemHandle cirle;
        if (CreateCircle(cirle, curArrowPoints_[0], arrowRadius_, TRUE)) {
            elems.push_back(cirle);
        }
        EditElemHandle line;
        if (CreateStringLine(line, curArrowPoints_, 3)) {
            elems.push_back(line);
        }
        break;
    }
    default:
        break;
    }

    return elems.size() != 0;
}

/*------------------------------------------------------------------------------
Построение группы элементов привязанных к тексту (текста и полки под него)
----------------------------------------------------------------------------- */
bool LeaderSetTool::createTextGroupElems(
    ElementList& elems, 
    DPoint3dCR basePoint, 
    const DPoint3d* orientPointP, 
    DPoint3dR textOriginOut)
{
    //DPoint3d repOrientPoint = (orientPoint.x == 0 && orientPoint.y == 0)
    //    ? basePoint : orientPoint;
    
    Orientation orient = Orientation();
    if (orientPointP != NULL) {
        DPoint3d repOrientPoint = 
            getHorOrVertProjectionPoint(basePoint, *orientPointP);
        orient = Orientation(basePoint, repOrientPoint);
    }

    double offset = (textOffset_ * ACTIVE_ANN_SCALE);

    // коэффициент "отражения" отн. оси ординат для правильной ориентации
    int kX = orient.isVertical ? orient.kV : orient.kH;

    // TODO добавить отступ до текста
    textOriginOut = Offset(
        basePoint,
        offset * kX,
        textStyleHeight_ + textStyleLineSpacing_ / 4
    );
    // в качестве ссылки на текстовый элемент используем переменную класса
    
    if (!CreateText(textEeh_, grabParams_, textOriginOut, orient, textStyleId_))
    {
        return false;
    }

    TextSize textSize;
    getTextParams(textEeh_.GetElemDescrP(), NULL, &textSize, NULL, NULL);

    // линия под текстом:
    DPoint3d p1 = basePoint;
    p1.x += (kX * (textSize.width + (offset * ACTIVE_SCALE)));

    EditElemHandle line;
    {
        DPoint3d pts[2] = { basePoint, p1 };
        CreateLine(line, pts);
    }

    if (orient.isVertical) {
        // поворот текста на угол 90 град.
        Transform tran;
        mdlTMatrix_getIdentity(&tran);
        DVec3d axisZ;
        axisZ.x = axisZ.y = 0.0;
        axisZ.z = 1.0;
        mdlTMatrix_fromRotationAroundPointAndVector(
            &tran, &basePoint, &axisZ, PI / 2);
        //&tran, &textLineOrigin_, &axisZ, PI / 2);
        mdlElmdscr_transform(textEeh_.GetElemDescrP(), &tran);
        mdlElmdscr_transform(line.GetElemDescrP(), &tran);
        // при повороте смещается базовая точка текста:
        mdlText_extract(&textOriginOut, 
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
            textEeh_.GetElementP());
    }

    elems.push_back(textEeh_);    
    elems.push_back(line); //! линия после текста, чтобы рисовалась поверх
    return true;
}

/*------------------------------------------------------------------------------
Окончантельное создание и публикация Cell-элемента Leader в активную модель
----------------------------------------------------------------------------- */
StatusInt LeaderSetTool::CreateCellAndPublish() {

    origin_ = nArrows_ > 1 ? textLineOrigin_ : firstArrowPoints_[0];

    EditElemHandle cellEeh;
    { // собираем Cell:
        ::CreateCell(cellEeh, TOOLNAME_W, &origin_, TRUE);

        int ress = 0;
        for (ElementList::iterator it = elements_.begin();
            it != elements_.end(); ++it) 
        {
            // append element to cell
            ress = AddChildToCell(cellEeh, *it);
        }
    }

    // предварительно уменьшаем Cell на величину обратную текущему масштабу аннотаций,
    // т.к. после преобразования в тип AnnotationCell размеры всей геометрии изменятся,
    // ! кроме текстового элемента, текстовый элемент просто изменит собственное
    // свойство на "Is Annotation" = False
    {
        Transform tran;
        mdlTMatrix_getIdentity(&tran);
        double kScale = 1 / ACTIVE_ANN_SCALE;
        // привязка к базовой точке
        mdlTMatrix_fromPointAndScale(&tran, &origin_, kScale);

        // масштабирование Cell:
        mdlElmdscr_transform(cellEeh.GetElemDescrP(), &tran);
    }

    // Преобразуем обычный Cell в AnnotationCell    
    cellEeh.GetElementP()->cell_3d.flags.isAnnotation = TRUE; // то же самое, что и mdlCell_setIsAnnotation()

    TransformInfo tranInfo;
    {   // Активируем масшабирование:
        tranInfo.SetOptions(TRANSFORM_OPTIONS_AllowSizeChange | TRANSFORM_OPTIONS_ApplyAnnotationScale);
        // NOTE: Checks state of annotation lock...
        tranInfo.SetAnnotationScaleAction(ANNOTATIONSCALE_ADD); // задаёт уже само свойство "is Annotation" = True
        tranInfo.SetAnnotationScale(ACTIVE_ANN_SCALE); // задаёт действительные размеры в соответствии с масштабом
    }

    if (SUCCESS != cellEeh.GetHandler().ApplyTransform(cellEeh, tranInfo)) {             
        return ERROR;
    }

    if (SUCCESS != cellEeh.AddToModel(ACTIVEMODEL)) {
        return ERROR;
    }
    
    //EditElemHandle lineIter;
    //EditElemHandle textIter;
    //for (ChildEditElemIter child(cellEeh, EXPOSECHILDREN_EditProperties); child.IsValid(); child = child.ToNext()) {
    //    if (child.GetElementType() == TEXT_ELM) {
    //        textIter = EditElemHandle(child.GetElementP())
    //    }
    //}
    
    MSElementDescr* textElemDP = NULL;
    MSElementDescr* lineElemDP = NULL;

    MSElementDescr* elmdP = cellEeh.GetElemDescrP()->h.firstElem;
    do {
        int type = mdlElement_getType(&elmdP->el);
        switch (type) {
        case TEXT_NODE_ELM:
        case TEXT_ELM:
            textElemDP = elmdP;
            break;
        case LINE_ELM:
            lineElemDP = elmdP;
            break;
        default:
            break;
        }
    }  while ((elmdP = elmdP->h.next) != NULL);

    if (textElemDP != NULL && lineElemDP != NULL) {
        
        // TODO установить связь элементов:
        DependencyLinkage depLinkage;
        mdlDependency_initLinkage(
            &depLinkage,
            APPID_LEADER,
            DEPENDENCY_DATA_TYPE_ELEM_ID,
            DEPENDENCY_ON_COPY_DropDependency
        );

        // ! dependencyCallback отрабатывает корректно только, если:
        // базовый элемент  - линия
        // зависимый        - текст
        depLinkage.nRoots = 1;
        depLinkage.root.elemid[0] = cellEeh.GetElemRef()->GetElemID(); //lineElemDP->el.line_3d.ehdr.uniqueId;
       
        // MODIFY_REQUEST_HEADERS - функция вызывается для всех дочерних элементов цела поочерёдно
        UInt32 cellFilePos = mdlElmdscr_getFilePos(cellEeh.GetElemDescrP());
        mdlModify_elementSingle(ACTIVEMODEL, cellFilePos,
            MODIFY_REQUEST_HEADERS, MODIFY_ORIG, 
            modFunc_SetDependencyOnText, (void*)&depLinkage, 0
        );
    }

    return SUCCESS;
}

//// out eeh;
//EditElemHandle LeaderSetTool::GetElemHandle(MSElementR element) {
//    EditElemHandle eeh;
//    MSElementDescrP elDescrP;
//
//    mdlElmdscr_new(&elDescrP, NULL, &element);
//    elDescrP->h.dgnModelRef = ACTIVEMODEL;
//    //the second parameter lets the destructor free the descriptor 
//    // (way cool I don't have to remember to free this now).
//    eeh.SetElemDescr(elDescrP, true, false);
//    return eeh;
//}

void LeaderSetTool::CreateAcceptedTransient() {
    // Use general purpose tool transient (automatically cleared when tool exits).
    mdlTransient_free(&msTransientElmP, true);
       
    for (ElementList::iterator it = elements_.begin();
        it != elements_.end(); ++it)
    {
        msTransientElmP = mdlTransient_addElemDescr(msTransientElmP,
            (*it).GetElemDescrCP(), true, 0xffff, DRAW_MODE_TempDraw, FALSE, FALSE, TRUE);
    }
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
//after the tool is installed 
void LeaderSetTool::OnPostInstall () {
    
   // mdlOutput_keyin("WORD PLACE TEXT ICON");

    //I can start dynamics since only one point is necessary for text placement.
    BeginComplexDynamics ();

    // mdlAccuSnap_enableLocate(TRUE);
    mdlAccuSnap_enableSnap(TRUE);
    


   // mdlView_defaultCursor();
    //mdlLocate_setCursor();
    __super::OnPostInstall();
 }

/*---------------------------------------------------------------------------------**//**
* Called from OnPostInstall. Tools, expecially those that operate on the current fence
* or selection set may not require a data point in order to start displaying dynamics.
* @return true to call start dynamics before the first data point.
* @bsimethod                                                    Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
bool LeaderSetTool::NeedPointForDynamics() {
    return false;
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod
+---------------+---------------+---------------+---------------+---------------+------*/
//the tool is being installed are there any tasks to do?
bool LeaderSetTool::OnInstall () {
    return __super::OnInstall();
}

StatusInt Leader_TextDependencyCallback
( /* <= SUCCESS if callback succeeds; ERROR to undo all changes */
    ElementID          dependentID,    /* => element ID of dependent element */
    DgnModelRefP       modelRef,       /* => modelRef of dependent element */
    DependencyLinkage* pLinkage,       /* => copy of dependent element's linkage */
    UChar*             pRootStatus,    /* => change status of each root in linkage: DEPENDENCY_STATUS_XXX  */
    UChar              selfStatus      /* => change status of dependent itself    : DEPENDENCY_STATUS_XXX  */
) {
    if (APPID_LEADER != pLinkage->appID)
        return SUCCESS;

    if (selfStatus != DEPENDENCY_STATUS_CHANGED) {
        return SUCCESS;
    }

    MSElement el;
    mdlAssoc_getElement(&el, NULL, dependentID, modelRef);

    MSElementDescr* textDescrP;
    if (SUCCESS == mdlAssoc_getElementDescr(
        &textDescrP, NULL, el.ehdr.uniqueId, modelRef, FALSE))
    {
        if (!isTextElement(textDescrP)) {
            return ERROR;
        }        
    }   

    MSElementDescr* cellElemDscrP = NULL;
    if (SUCCESS == mdlAssoc_getElementDescr(
        &cellElemDscrP, NULL, pLinkage->root.elemid[0], modelRef, false))
    {
        if (cellElemDscrP->el.ehdr.type != CELL_HEADER_ELM) {
            return ERROR;
        }
    }
    else {
        return ERROR;
    }
    
    UInt32 cellFilePos = mdlElmdscr_getFilePos(cellElemDscrP);
    //mdlModify_elementDescr2(&cellElemDscrP, ACTIVEMODEL, 0,
    //    modFunc_ResizeBaseLine, (void*)&textSize, 0L);

    mdlModify_elementSingle(
        modelRef, cellFilePos, MODIFY_REQUEST_HEADERS, MODIFY_ORIG,
        modFunc_ResizeBaseLine, (void*)textDescrP, 0L
    );    

    return SUCCESS;
}

/* Функция модификации */
int modFunc_SetDependencyOnText
(
    MSElement   *elP,   /* element to add ink onto*/
    void		*dataP  /* link data to add */
) 
{
    if (!isTextElement(*elP)) {
        return MODIFY_STATUS_NOCHANGE;
    }

    DependencyLinkage* depLinkageP = (DependencyLinkage*)dataP;    
    int status = mdlDependency_appendLinkageEx(elP, depLinkageP, 0);
    
    return status == SUCCESS ?
        MODIFY_STATUS_REPLACE : MODIFY_STATUS_NOCHANGE;
}

int modFunc_ResizeBaseLine
(
    MSElementUnion      *element,      // <=> element to be modified
    void                *params,       // => user parameter
    DgnModelRefP        modelRef,      // => model to hold current elem
    MSElementDescr      *elmDscrP,     // => element descr for elem
    MSElementDescr      **newDscrPP,   // <= if replacing entire descr
    ModifyElementSource elemSource    // => The source for the element
) 
{
    // TODO доп проверка базовой линии
    
    if (element->hdr.ehdr.type != LINE_ELM) {
        return MODIFY_STATUS_NOCHANGE;
    }
    // TODO try catch - на приведение параметров?
    // размеры текста
    MSElementDescr* textDescrP = (MSElementDescr*)params;
    
    // параметры текста:
    Orientation orient;
    MSTextSize size;
    DPoint3d origin;
    int just;    
    getTextParams(textDescrP, &origin, &size, &orient, &just);

    TextJustification justE = (TextJustification)(just);

    Line_3d& line = element->line_3d;
        
    DPoint3d points[2];
    points[0] = points[1] = line.start;
    
    double delta = size.width;
    if (orient.kH != 0) {
    // горизонтально
        delta += abs(origin.x - line.start.x);
        if (justE == LeftTop) {
            points[1].x = line.start.x + delta;
        }
        else if (justE == RightTop) {

            points[1].x = line.start.x - delta;
        }
        else {
            return MODIFY_STATUS_NOCHANGE;
        }
    }
    else if (orient.kV != 0) {
    // вертикально
        delta += abs(origin.y - line.start.y);
        if (justE == LeftTop) {
            points[1].y = line.start.y + delta;
        }
        else if (justE == RightTop) {
            points[1].y = line.start.y - delta;
        }
        else {
            return MODIFY_STATUS_NOCHANGE;
        }
    }
    else {
        return MODIFY_STATUS_NOCHANGE;
    }
    
    EditElemHandle lineEeh;
    CreateLine(lineEeh, points);

    mdlElmdscr_duplicate(newDscrPP, lineEeh.GetElemDescrP());

    return MODIFY_STATUS_REPLACEDSCR;
}

bool isLeaderToolCell(MSElementCP elCP)
{
    MSWChar cellName[MAX_STR_LEN];
    (*elCP).cell_3d.GetName(cellName, MAX_STR_LEN);

    return wcscmp(cellName, TOOLNAME_W) == 0;
}