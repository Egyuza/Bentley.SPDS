#include "ElementHelper.h"
#include "FontManager.h"

#include    <mselementtemplate.fdf>

#define TEXT_TEMPLATE_NAME  L"CreatedGroup\\AEP SPDS Leader Text Template"

#if !defined (DIM)
#define DIM(a) ((sizeof(a)/sizeof((a)[0])))
#endif


// Unpublished in V8i
extern "C"
{ //These functions return the number of characters (not strictly bytes) inserted into the destination buffer
    int MSWideCharStringToMSWCharString(MSWChar* pOutString, UInt32 nOutChars, MSWideChar const * pInString, UInt32 codePage);
    int MSWCharStringToMSWideCharString(MSWideChar* pOutString, UInt32 nOutChars, MSWChar const * pInString, UInt32 codePage);
}

/*------------------------------------------------------------------------------
Создание простой линии по двум точкам
----------------------------------------------------------------------------- */
bool CreateLine(EditElemHandleR eehOut, DPoint3d* points) {
    MSElement el;
    mdlLine_create(&el, NULL, points);
    return ToEditHandle(eehOut, el);
}

/*------------------------------------------------------------------------------
Создание мультилинии
----------------------------------------------------------------------------- */
bool CreateStringLine(EditElemHandleR eehOut, DPoint3d* points, int numVerts) {
    MSElement el;
    mdlLineString_create(&el, NULL, points, numVerts);
    return ToEditHandle(eehOut, el);
}

/*------------------------------------------------------------------------------
Создание окружности
----------------------------------------------------------------------------- */
bool CreateCircle(EditElemHandleR eehOut, 
    DPoint3dCR center, double radius, int fillMode)
{
    MSElement el;    
    radius *= ACTIVE_SCALE * ACTIVE_ANN_SCALE;

    DPoint3d pts[3] = { center, center, center };
    pts[0].x = center.x - radius;
    pts[0].y = center.y;
    pts[1].x = center.x;
    pts[1].y = center.y + radius;
    pts[2].x = center.x + radius;
    pts[2].y = center.y;

    /* create the pushpin outline */
    //mdlCircle_createBy3Pts(&el, NULL, pts, fillMode);
  
    mdlEllipse_create(&el, 
        NULL, &DPoint3d(center), radius, radius, NULL, fillMode);

    {
        ///* allocate an element descr */
        //mdlElmdscr_new(&elmDscrP, NULL, &circle);
        ///* add fill to the element in the element descr */
        //mdlElmdscr_addFill(&elmDscrP);
        ///* call a function that will set the color of the fill linkage */
        //mdlElmdscr_operation(elmDscrP, gis_setColorFillLinkage, &pinColor, ELMD_ALL_ONCE);
        ///* draw the pushpin */
        //mdlElmdscr_display(elmDscrP, MASTERFILE, NORMALDRAW);
    }

    return ToEditHandle(eehOut, el);
}


/*------------------------------------------------------------------------------
Создание текстового элемента
----------------------------------------------------------------------------- */
bool CreateText(EditElemHandleR eehOut, TextGrabParam grabParam,
    DPoint3dCR origin, Orientation orient, UInt32 textStyleId)
{
    MSElement textEl;   
    { // создание текстового элемента:

        TextParamWide txtParamsWide;
        TextSizeParam textSize;
        textSize.mode = TXT_BY_TILE_SIZE;
        
        double& widthR = textSize.size.width;
        double& heightR = textSize.size.height;
       
        mdlTextStyle_getTextParamWideFromStyle(&txtParamsWide, 
            &widthR, &heightR, NULL, textStyleId, ACTIVEMODEL, TRUE);
        { 
        // ПАРАМЕТРЫ ТЕКСТА:
          // выравнивание:
            txtParamsWide.just = -1 == (orient.isVertical ? orient.kV : orient.kH)
                ? RightTop
                : LeftTop;
            // стиль:           
            txtParamsWide.textStyleId = textStyleId;            
        }

        applyAnnotationScaleFactorToTextParams(
            &txtParamsWide, &textSize, ACTIVE_SCALE);
        
        // TODO создание многострочного текстового элемента
        mdlTextNode_createWide(&textEl, NULL, &DPoint3d(origin), NULL, &textSize, &txtParamsWide);
       
        MSElementDescr* txtnode = NULL;
        mdlElmdscr_new(&txtnode, NULL, &textEl);


        // добавляем строки
        mdlText_addStringsToNodeDscrWide(txtnode, grabParam.textWide);

        if (!ToEditHandle(eehOut, txtnode)) {
            return false;
        }
    }

    // отключаем аннотативность
    {
        TransformInfo tranInfo;
        {   // Активируем масшабирование:
            tranInfo.SetOptions(TRANSFORM_OPTIONS_AllowSizeChange |
                TRANSFORM_OPTIONS_ApplyAnnotationScale);
            // задаёт действительные размеры в соответствии с масштабом
            // tranInfo.SetAnnotationScale(ACTIVE_ANN_SCALE); 
        }
        eehOut.GetHandler().ApplyTransform(eehOut, tranInfo);
    }

    { // масштабируем для имитации масштаба аннотоции
        Transform txtTran;
        mdlTMatrix_getIdentity(&txtTran);
        double txtScale = ACTIVE_ANN_SCALE;
        // привязка к базовой точке
        mdlTMatrix_fromPointAndScale(&txtTran, &origin, txtScale);
        mdlElmdscr_transform(eehOut.GetElemDescrP(), &txtTran);
    }

    return true;
}

//std::string cp1251_to_utf8(const char *str) {
//    std::string res;
//    int result_u, result_c;
//    result_u = MultiByteToWideChar(1251, 0, str, -1, 0, 0);
//    if (!result_u) { return 0; }
//    wchar_t *ures = new wchar_t[result_u];
//    if (!MultiByteToWideChar(1251, 0, str, -1, ures, result_u)) {
//        delete[] ures;
//        return 0;
//    }
//    result_c = WideCharToMultiByte(65001, 0, ures, -1, 0, 0, 0, 0);
//    if (!result_c) {
//        delete[] ures;
//        return 0;
//    }
//    char *cres = new char[result_c];
//    if (!WideCharToMultiByte(65001, 0, ures, -1, cres, result_c, 0, 0)) {
//        delete[] cres;
//        return 0;
//    }
//    delete[] ures;
//    res.append(cres);
//    delete[] cres;
//    return res;
//}


/*------------------------------------------------------------------------------
получение ближайшей (вертикальной или горизонтальной) проекции точки
----------------------------------------------------------------------------- */
DPoint3d getHorOrVertProjectionPoint(DPoint3dCR base, DPoint3dCR target) {
    if (base.x == target.x && base.y == target.y) {
        return base;
    }
    
    DVec3d v = target;
    v.x -= base.x;
    v.y -= base.y;
    DPoint3d new_target = target;

    double angle = mdlVec_angleXY(&v);
    if (angle >= PI / 4 && angle < 3 * PI / 4 ||
        angle <= -PI / 4 && angle > -3 * PI / 4) 
    { // вертикально:
        new_target.x = base.x;
    }
    else { // горизонтально
        new_target.y = base.y;
    }
    return new_target;
}

/*------------------------------------------------------------------------------
Результат перемещения точки
----------------------------------------------------------------------------- */
DPoint3d Offset(DPoint3dCR base, double dx, double dy, bool scaled) {
    DPoint3d pt = base;
    pt.x += (dx * (scaled ? ACTIVE_SCALE : 1));
    pt.y += (dy * (scaled ? ACTIVE_SCALE : 1));
    return pt;
}

// Конвертация в редактируемый сылочный тип на элемент
bool ToEditHandle(EditElemHandleR eehOut, MSElement element) {
    MSElementDescrP  elDescrP;
    mdlElmdscr_new(&elDescrP, NULL, &element);
    elDescrP->h.dgnModelRef = ACTIVEMODEL;
    //the second parameter lets the destructor free the descriptor 
    //(way cool I don't have to remember to free this now).
    eehOut.SetElemDescr(elDescrP, true, false);
    return eehOut.IsValid();
}

// Конвертация в редактируемый сылочный тип на элемент
bool ToEditHandle(EditElemHandleR eehOut, MSElementDescrP elDescrP) {
    elDescrP->h.dgnModelRef = ACTIVEMODEL;
    //the second parameter lets the destructor free the descriptor 
    //(way cool I don't have to remember to free this now).
    eehOut.SetElemDescr(elDescrP, true, false);
    return eehOut.IsValid();
}

/*---------------------------------------------------------------------------------**//**
author        BentleySystems                          03/04
+---------------+---------------+---------------+---------------+---------------+------*/
void applyAnnotationScaleFactorToTextParams
(
    TextParamWide*  textParamP,
    TextSizeParam*  textSizeP,
    double          annotationScale
) {
    /*=====================================================================================
    Purpose:
    --------
    Apply the annotation scale as given to parameters.  In the future such code may not be
    needed if the required APIs are published.
    =====================================================================================*/
    if (annotationScale <= 0.0 || 1.0 == annotationScale)
        return;

    if (NULL != textSizeP) {
        textSizeP->size.width *= annotationScale;
        textSizeP->size.height *= annotationScale;
    }

    if (NULL != textParamP) {
        // In 'at least' mode, the linespacing is a multiplier, not a distance
        if (TXTLINESPACING_ATLEAST != textParamP->exFlags.acadLineSpacingType)
            textParamP->lineSpacing *= annotationScale;

        if (FALSE == textParamP->exFlags.acadInterCharSpacing)
            textParamP->characterSpacing *= annotationScale;

        textParamP->underlineSpacing *= annotationScale;
        textParamP->overlineSpacing *= annotationScale;

        textParamP->lineOffset.x *= annotationScale;
        textParamP->lineOffset.y *= annotationScale;

        textParamP->backgroundBorder.x *= annotationScale;
        textParamP->backgroundBorder.y *= annotationScale;
        
        //textParamP->annotationScale = annotationScale;
    }
}

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
ElementTemplateP  getTemplate(MSWChar*  pwTemplatePath) {
    ElementTemplateP  elementTemplate = NULL;

    mdlElementTemplate_loadByPath(&elementTemplate, pwTemplatePath);
    return elementTemplate;
}


/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
void createTextTemplate(void) {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create(&elementTemplate, TEXT_TEMPLATE_NAME)) {
        int             addedIndex;
        ValueDescr      valDesc;

        // define cell info
        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Example Text";
        if (SUCCESS != mdlElementTemplate_addElementParam(
            elementTemplate, &addedIndex, ELEMENTPARAM_TextStyle, &valDesc)) 
        {
            printf("\n ERROR adding TextStyle to template");
        }

        valDesc.value.wCharPFormat = NULL;

        // define textstyle override info
        TextStyleOverrideData textStyleOverride;
        memset(&textStyleOverride, 0, sizeof(textStyleOverride));

        wcsncpy(textStyleOverride.fontName, L"Comic Sans MS", (sizeof(textStyleOverride.fontName) / sizeof(wchar_t)) - 1);
        textStyleOverride.modifiers |= (UInt32)TEXTSTYLEMOD_FontNamePresent;
        textStyleOverride.textJustification = TXTJUST_LT;
        textStyleOverride.modifiers |= (UInt32)TEXTSTYLEMOD_TextJustificationPresent;
        textStyleOverride.nodeJustification = TXTJUST_LT;
        textStyleOverride.modifiers |= (UInt32)TEXTSTYLEMOD_NodeJustificationPresent;

        valDesc.formatType = FMT_VOIDP;
        valDesc.value.voidPFormat = &textStyleOverride;
        if (SUCCESS != mdlElementTemplate_addElementParam(elementTemplate,
            &addedIndex, ELEMENTPARAM_TextStyleOverride, &valDesc)) 
        {
            printf("\n ERROR adding TextStyleOverride to template");
        }
        valDesc.value.voidPFormat = NULL;

        mdlElementTemplate_save(elementTemplate);
        mdlElementTemplate_free(&elementTemplate);
    }
}

void createText(ElementTemplateP elementTemplate) {
    DgnModelRefP    modelRef = ACTIVEMODEL;
    DPoint3d        origin;
    TextParamWide   textParams;
    TextSizeParam   textSize;
    DPoint2d        templateTextSize;
    short           lineLength;
    double          annotationScaleFactor = 100.0;
    BoolInt         bApplyAnnotationScaleFactor = TRUE;

    origin.x = 0.0;
    mdlCnv_masterToUOR(&origin.x, -100.0, modelRef);
    mdlCnv_masterToUOR(&origin.y, 100.0, modelRef);
    origin.z = 0.0;

    textSize.mode = TXT_BY_TILE_SIZE;

    if (SUCCESS == mdlElementTemplate_getTextParameters(elementTemplate,
        &textParams, &templateTextSize, &lineLength,
        annotationScaleFactor, bApplyAnnotationScaleFactor, modelRef)) 
    {
        textSize.size.width = templateTextSize.x;
        textSize.size.height = templateTextSize.y;
    }
    else {
        DgnTextStylePtr activeStyle = TextStyleManager::GetActiveStyle();

        mdlTextStyle_getTextParamWideFromStyle(&textParams, &textSize.size.width, &textSize.size.height,
            NULL, activeStyle->GetID(), modelRef, TRUE);
    }

    MSElement textNodeElement;
    mdlTextNode_createWide(&textNodeElement, NULL, &origin, NULL, &textSize, &textParams);

    MSElementDescrP textNodeElementDescriptor = NULL;
    mdlElmdscr_new(&textNodeElementDescriptor, NULL, &textNodeElement);
    textNodeElementDescriptor->h.dgnModelRef = ACTIVEMODEL;

    mdlText_addStringsToNodeDscr(textNodeElementDescriptor, "What goes around\n");
    mdlText_addStringsToNodeDscr(textNodeElementDescriptor, "usually gets dizzy\n");
    mdlText_addStringsToNodeDscr(textNodeElementDescriptor, "and falls over.");

    EditElemHandle  ehandle(textNodeElementDescriptor, true, true);

    // apply the specified templates symbology
    mdlElementTemplate_applySymbology(elementTemplate, ehandle);

    if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment(elementTemplate, ehandle))
        printf("\n ERROR scheduling template attachment");

    // add the text to the file
    ehandle.AddToModel(modelRef);
}

bool CreateCell(
    EditElemHandleR eehOut, MSWCharCP name, DPoint3dP origin, BoolInt pointCell) 
{
    MSElement cell;
    mdlCell_create(&cell, name, origin, pointCell);
    return ToEditHandle(eehOut, cell);
}


bool Scale(EditElemHandleR eeh, DPoint3dCR point, double scale) {
    Transform tran;
    mdlTMatrix_getIdentity(&tran);
    // привязка к базовой точке
    mdlTMatrix_fromPointAndScale(&tran, &point, scale);
    // масштабирование
    return SUCCESS == mdlElmdscr_transform(eeh.GetElemDescrP(), &tran); 
}

bool AddChildToCell(EditElemHandleR cell, EditElemHandleR child) {

    if (cell.GetElementType() != CELL_HEADER_ELM) {
        return false;
    }
    MSElementDescrP el_descrP = child.ExtractElemDescr();
    return SUCCESS == mdlElmdscr_appendDscr(cell.GetElemDescrP(), el_descrP);
}


bool SetAccudrawContext
(
    long            flags,
    Dpoint3d*       origin,
    Dpoint3d*       delta,
    double const*   distance,
    double const*   angle,
    void*           orientation
)
{
    return SUCCESS == mdlState_setAccudrawContext(
        flags, origin, delta, distance, angle, orientation);
}

bool setIsAnnotation(
    EditElemHandleR eeh, bool isAnnotation, double annScale)
{
    TransformInfo tranInfo;
    {   // Активируем масшабирование:

        tranInfo.SetOptions(TRANSFORM_OPTIONS_AllowSizeChange |
            TRANSFORM_OPTIONS_ApplyAnnotationScale);

        AnnotationScaleAction action = isAnnotation ?
            ANNOTATIONSCALE_ADD : ANNOTATIONSCALE_REMOVE;

        // задаёт уже само свойство "is Annotation" = True/False
        tranInfo.SetAnnotationScaleAction(action);

        if (isAnnotation && annScale > 0) {
            // задаёт действительные размеры в соответствии с масштабом
            tranInfo.SetAnnotationScale(annScale);
        }
    }
    return SUCCESS == eeh.GetHandler().ApplyTransform(eeh, tranInfo);
}

bool getTextParams(MSElementDescrP textDescrP,
    DPoint3d* originP, TextSize* sizeP, Orientation* orientP, int* justP)
{
    if (!isTextElement(textDescrP))
        return false;

    DPoint3d rngPoints[5];

    int type = textDescrP->el.ehdr.type;
    if (type == TEXT_NODE_ELM) {
        mdlTextNode_extractShape(rngPoints, NULL, textDescrP, FALSE, 0);

        TextParam textParam;
        mdlTextNode_extract(originP, NULL, NULL, NULL, &textParam, NULL, 
            &textDescrP->el, textDescrP);
        if (justP != NULL) {
            *justP = textParam.just;
        }
    }
    else if (type == TEXT_ELM) {
        mdlText_extractShape(rngPoints, NULL, &textDescrP->el, FALSE, 0);
        mdlText_extract(originP, NULL, NULL, NULL, NULL, NULL, NULL, 
            justP, NULL, NULL, &textDescrP->el);
    }
    
    /* точки прямоугольника, описывающего текстовый элемент:
    *    3 - 2
    *    |   |
    *    0 - 1
    */
    if (sizeP != NULL) {
        sizeP->width = abs(mdlVec_distanceXY(&rngPoints[0], &rngPoints[1]));
        sizeP->height = abs(mdlVec_distanceXY(&rngPoints[0], &rngPoints[3]));
    }

    if (orientP != NULL) {
        *orientP = Orientation(rngPoints[0], rngPoints[1]);
    }

    return true;
}

bool isTextElement(MSElement& el)
{
    int type = el.ehdr.type;
    return type == TEXT_NODE_ELM ||type == TEXT_ELM;
}

bool isTextElement(MSElementDescrP elDescrP)
{
    return elDescrP != NULL && isTextElement(elDescrP->el);
}