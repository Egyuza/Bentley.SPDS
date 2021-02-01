/*----------------------------------------------------------------------+
|
|   $Source: /miscdev-root/miscdev/mdl/examples/CustomizeTestApp/CustomizeTestApp.cpp,v $
|  $RCSfile: CustomizeTestApp.cpp,v $
| $Revision: 1.1 $
|     $Date: 2008/06/02 15:09:34 $
|   $Author: Mark.Anderson $
|
|  $Copyright: (c) 2008 Bentley Systems, Incorporated. All rights reserved. $
|
+--------------------------------------------------------------------------------------
| This example is uses cells and text styles from the Example/General workspace
+--------------------------------------------------------------------------------------*/

#define PI 3.1415926535897932384626433

#include    <mdl.h>
#include    <toolsubs.h>
#include    <basetype.h>
#include    <malloc.h>
#include    <msrmgr.h>
#include    <mstypes.h>
#include    <string.h>
#include    <msparse.fdf>
#include    <msstate.fdf>
#include    <mssystem.fdf>
#include    <dlmsys.fdf>
#include    <mscnv.fdf>
#include    <msfile.fdf>
#include    <msdialog.fdf>
#include    <dlogids.h>
#include    <mswindow.fdf>
#include    <msdgnmodelref.fdf>
#include    <msdgnlib.fdf>
#include    <mselemen.fdf>
#include    <mselmdsc.fdf>
#include    <mscell.fdf>
#include    <msmline.fdf>
#include    <mstextstyle.fdf>
#include    <listmodel.fdf>

#include    <msdgncache.h>
#include    <msdgnmodelref.fdf>
#include    <ditemlib.fdf>

#include    <mselementtemplate.fdf>
#include    <msdimstyle.fdf>
#include    <msdim.fdf>
#include    <namedexpr.fdf>
#include    <MicroStationAPI.h>

#if !defined (DIM)
#define DIM(a) ((sizeof(a)/sizeof((a)[0])))
#endif

extern "C" int      mdlMline_createUsingStyle
(
MSElementDescr   **mlineElD,
MSElement         *inEl,
DPoint3d          *normal,
DPoint3d          *points,
int               nPoints,
MSWChar*          mlineStyleName, // if NULL use active
MlineOffsetMode*  pMode, // MLINE_PLACE_BYWORK, MLINE_PLACE_BYCENTER, MLINE_PLACE_BYMAX, MLINE_PLACE_BYMIN  or NULL to use active
double*           styleScale // if NULL use active styleScale settings
);

extern "C" int  mdlDim_createStubUsingStyle
(
MSElement  *dim,          /* <= New dimension element               */
MSElement  *seed,         /* => Seed dimension element or NULL      */
RotMatrix     *rMatrix,      /* => Dimension rotation matrix or NULL   */
int           dimType,       /* => Dimension type (-1 = use active)    */
int           dimView,       /* => View reading text (-1 = set later)  */
DgnModelRefP  modelRef,
MSWChar*      dimStyleName
);

#include    "CustomizeTestApp.h"
#include    "CustomizeTestAppids.h"
#include    "CustomizeTestAppcmd.h"

USING_NAMESPACE_BENTLEY_USTN
USING_NAMESPACE_BENTLEY_USTN_ELEMENT
USING_NAMESPACE_TEXT

#define LINEAR_TEMPLATE_NAME      L"CreatedGroup\\Created Linear Template"
#define SHAPE_TEMPLATE_NAME       L"CreatedGroup\\Created Shape Template"
#define HATCH_TEMPLATE_NAME       L"CreatedGroup\\Created Hatch Template"
#define AREAPATTERN_TEMPLATE_NAME L"CreatedGroup\\Created AreaPattern Template"
#define CELL_TEMPLATE_NAME        L"CreatedGroup\\Created Cell Template"
#define TEXT_TEMPLATE_NAME        L"CreatedGroup\\Created Text Template"
#define MLINE_TEMPLATE_NAME       L"CreatedGroup\\Created MLine Template"
#define DIMENSION_TEMPLATE_NAME   L"CreatedGroup\\Created Dimension Template"
#define HEADER_TEMPLATE_NAME      L"CreatedGroup\\Header"
#define COMPONENT1_TEMPLATE_NAME  L"CreatedGroup\\Component1"
#define COMPONENT2_TEMPLATE_NAME  L"CreatedGroup\\Component2"

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createLinearTemplate (void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, LINEAR_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Default";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Level, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;// color by index
        valDesc.value.sLongFormat = COLOR_BYLEVEL;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
             printf ("\n ERROR adding Color to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = WEIGHT_BYLEVEL;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Weight, &valDesc)) 
             printf ("\n ERROR adding Weight to template");

        valDesc.formatType = FMT_LONG; // line style by line code number
        valDesc.value.sLongFormat = STYLE_BYLEVEL;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
             printf ("\n ERROR adding LineStyle to template");

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void    createLine
(
ElementTemplateP  elementTemplate
)
    {
    StatusInt         status          = SUCCESS;
    DgnModelRefP      modelRef        = ACTIVEMODEL;

    BoolInt             is3D = mdlModelRef_is3D (modelRef);
    MSElement           element;
    MSElementDescrP     lineElmdscr=NULL;
    Dpoint3d            points[2];

    double      deltaUor = 1.0;

    mdlCnv_masterToUOR (&deltaUor, 30.0, modelRef);
    points[0].x = 0.0;
    points[0].y = 0.0;
    points[0].z = 0.0;
    points[1] = points[0];
    points[1].x += deltaUor;

    mdlLine_create (&element, NULL, points);
    mdlElmdscr_new (&lineElmdscr, NULL, &element);
    UInt32          filePos=0;

    EditElemHandle  ehandle (lineElmdscr, true, false); // Give the element descr to the handle; it will free it
    
    // apply the specified templates symbology
    mdlElementTemplate_applySymbology (elementTemplate, ehandle);

    // add the line to the file
    ehandle.AddToModel (modelRef);

    ElementRef elementRef = ehandle.GetElemRef ();
    if (NULL != elementRef)
        mdlElementTemplate_addTemplateReference (elementTemplate, elementRef);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createShapeTemplate (void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, SHAPE_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Default";
            
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Level, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;// color by index
        valDesc.value.sLongFormat = 6;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
             printf ("\n ERROR adding Color to template");

        valDesc.formatType = FMT_MSWCHAR; // color by rgb
        valDesc.value.wCharPFormat = L"230,10,0";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 5;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Weight, &valDesc)) 
             printf ("\n ERROR adding Weight to template");

        valDesc.formatType = FMT_LONG; // line style by line code number
        valDesc.value.sLongFormat = 3;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
             printf ("\n ERROR adding LineStyle to template");

        valDesc.formatType = FMT_MSWCHAR; // line style by name
        valDesc.value.wCharPFormat = L"Fence";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        ElementFillColorData fillColor;
        memset (&fillColor, 0, sizeof(fillColor));
        fillColor.colorData.colorSource = COLORSOURCE_Element;
        fillColor.colorData.colorType = COLORTYPE_GradientFill;
        fillColor.active = true;
        fillColor.mode = GRADIENT_MODE_Linear;
        fillColor.angle = 45.0*PI/180.0;  // in radians
        fillColor.nKeys = 2;
        fillColor.values[0] = 0.0;
        fillColor.colors[0].red = 255;
        fillColor.colors[0].blue = 0;
        fillColor.colors[0].green = 0;
        fillColor.values[1] = 1.0;
        fillColor.colors[1].red = 255;
        fillColor.colors[1].blue = 255;
        fillColor.colors[1].green = 255;

        valDesc.formatType = FMT_VOIDP; // fill color by FillColorData
        valDesc.value.voidPFormat = &fillColor;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_FillColor, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_MSWCHAR; // fill color by rgb
        valDesc.value.wCharPFormat = L"135,0,35";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_FillColor, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = PRIMARY_CLASS;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Class, &valDesc)) 
             printf ("\n ERROR adding Element Class to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = CONSTRUCTION_CLASS;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Class, &valDesc)) 
             printf ("\n ERROR adding Element Class to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 0; //Solid;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_AreaMode, &valDesc)) 
             printf ("\n ERROR adding AreaMode to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 1; // Hole
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_AreaMode, &valDesc)) 
             printf ("\n ERROR adding AreaMode to template");
        
        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 2; //Outlined
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_FillMode, &valDesc)) 
             printf ("\n ERROR adding FillMode to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 1; // Filled
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_FillMode, &valDesc)) 
             printf ("\n ERROR adding FillMode to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 0; //NotFilled
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_FillMode, &valDesc)) 
             printf ("\n ERROR adding FillMode to template");

        valDesc.formatType = FMT_DPFP;
        valDesc.value.doubleFormat = 50.0; // 50 %
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Transparency, &valDesc)) 
             printf ("\n ERROR adding Transparency to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 400;  // -500 to 500
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Priority, &valDesc)) 
             printf ("\n ERROR adding Priority to template");

        LineStyleParamData lsParamsData;
        memset (&lsParamsData, 0, sizeof(lsParamsData));
        lsParamsData.modifiers |= (UInt32)STYLEMOD_ScalePresent;
        lsParamsData.scale = 0.25;
        valDesc.formatType        = FMT_VOIDP;
        valDesc.value.voidPFormat = &lsParamsData;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyleParam, &valDesc)) 
             printf ("\n ERROR adding LineStyleParam to template");

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createShape 
(
ElementTemplateP elementTemplate
)
    {
    DgnModelRefP    modelRef     = ACTIVEMODEL;     
    BoolInt         is3D         = mdlModelRef_is3D (modelRef); 
    Dpoint3d        points[5];  
    double          deltaUor;   
    double          delta        = 30.0;            
    MSElementDescrP shapeElmdscr = NULL;            
    MSElement       element;     
    int             fillMode     = -1;      /* => -1=use active, 0=No fill, 1=Fill    */          
    int             numParams    = 0;

     mdlCnv_masterToUOR (&deltaUor, delta/2.0, modelRef);

    points[0].x = 0.0;
    mdlCnv_masterToUOR (&points[0].y, 40.0, modelRef);
    points[0].z = 0.0;
    points[1] = points[0];
    points[1].x += deltaUor;

    points[2] = points[1];
    points[2].y += deltaUor;

    points[3] = points[0];
    points[3].y += deltaUor;

    points[4] = points[0];

    mdlElementTemplate_getNumElementParams (elementTemplate, &numParams, ELEMENTPARAM_FillMode);
    if (numParams > 0) // if fill mode is defined then honor it
        {
        ValueDescr  valDesc;
        
        if (SUCCESS == mdlElementTemplate_getElementParam (elementTemplate, &valDesc, ELEMENTPARAM_FillMode, 0) && 
            FMT_LONG == valDesc.formatType)
            {
            fillMode = valDesc.value.sLongFormat;
            }
        }        
    else // if no fill mode is defined then infer it based on the definition of a Fill Color
        {
        mdlElementTemplate_getNumElementParams (elementTemplate, &numParams, ELEMENTPARAM_FillColor);
        if (numParams > 0)
            fillMode = 1;
        }

    mdlShape_create (&element, NULL, points, 5, fillMode);
    mdlElmdscr_new (&shapeElmdscr, NULL, &element);

    // Add the attributes
    EditElemHandle  ehandle (shapeElmdscr, true, false); // Give the element descr to the handle; it will free it
    ehandle.SetModelRef (modelRef);

    //apply the specified templates symbology
    mdlElementTemplate_applySymbology (elementTemplate, ehandle);
    if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment (elementTemplate, ehandle))
        printf ("\n ERROR scheduling template attachment %S", L"CreatedGroup\\CreatedTemplate 1");

    // add the line to the file
    ehandle.AddToModel (modelRef);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createHatchTemplate (void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, HATCH_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Default";
            
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Level, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_MSWCHAR; // color by rgb
        valDesc.value.wCharPFormat = L"0,10,200";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 0;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Weight, &valDesc)) 
             printf ("\n ERROR adding Weight to template");

        valDesc.formatType = FMT_LONG; // line style by line code number
        valDesc.value.sLongFormat = 0;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
             printf ("\n ERROR adding LineStyle to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 0; //Solid;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_AreaMode, &valDesc)) 
             printf ("\n ERROR adding AreaMode to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 0; //NotFilled
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_FillMode, &valDesc)) 
             printf ("\n ERROR adding FillMode to template");

        // hatch/xhatch
        PatternAngle hatchAngles;
        hatchAngles.Angle1 = 45.0*PI/180.0;  // in radians
        hatchAngles.Angle2 = -45.0*PI/180.0;  // in radians
        valDesc.formatType = FMT_VOIDP;
        valDesc.value.voidPFormat = &hatchAngles;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_PatternAngle, &valDesc)) 
             printf ("\n ERROR adding PatternAngle to template");
        valDesc.value.voidPFormat = NULL;

        PatternDelta deltas; // define in meters
        deltas.x = (mdlModelRef_getUorPerMaster(ACTIVEMODEL)/mdlModelRef_getUorPerMeter(ACTIVEMODEL)) * 2.0;  
        deltas.y = (mdlModelRef_getUorPerMaster(ACTIVEMODEL)/mdlModelRef_getUorPerMeter(ACTIVEMODEL)) * 4.0; 
        valDesc.formatType        = FMT_VOIDP;
        valDesc.value.voidPFormat = &deltas;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_PatternDelta, &valDesc)) 
             printf ("\n ERROR adding PatternDelta to template");
        valDesc.value.voidPFormat = NULL;

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createHatch 
(
ElementTemplateP elementTemplate
)
    {
    DgnModelRefP    modelRef     = ACTIVEMODEL;     
    BoolInt         is3D         = mdlModelRef_is3D (modelRef); 
    Dpoint3d        points[5];  
    double          deltaUor;   
    double          delta        = 30.0;            
    MSElementDescrP shapeElmdscr = NULL;            
    MSElement       element;     
    int             fillMode     = 0;      /* => -1=use active, 0=No fill, 1=Fill    */          
    int             numParams    = 0;


    // --- activate the template so active symbology is set ---
    mdlCnv_masterToUOR (&deltaUor, delta/2.0, modelRef);

    mdlCnv_masterToUOR (&points[0].x, 40.0, modelRef);
    points[0].y = 0.0;
    points[0].z = 0.0;
    points[1] = points[0];
    points[1].x += deltaUor;

    points[2] = points[1];
    points[2].y += deltaUor;

    points[3] = points[0];
    points[3].y += deltaUor;

    points[4] = points[0];

    mdlShape_create (&element, NULL, points, 5, fillMode);
    mdlElmdscr_new (&shapeElmdscr, NULL, &element);

    // Add the attributes
    EditElemHandle  ehandle (shapeElmdscr, true, false); // Give the element descr to the handle; it will free it
    ehandle.SetModelRef (modelRef);
    
    // apply the specified templates symbology
    mdlElementTemplate_applySymbology (elementTemplate, ehandle);

    if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment (elementTemplate, ehandle))
        printf ("\n ERROR scheduling template attachment %S", L"CreatedGroup\\CreatedTemplate 1");

    // add the line to the file
    ehandle.AddToModel (modelRef);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createAreaPatternTemplate(void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, AREAPATTERN_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        // define cell info
        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Circles";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_PatternCell, &valDesc)) 
            printf ("\n ERROR adding PatternCell to template");
        valDesc.value.wCharPFormat = NULL;
         
        valDesc.formatType         = FMT_DPFP;
        valDesc.value.doubleFormat = 0.05;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_PatternScale, &valDesc)) 
            printf ("\n ERROR adding PatternScale to template");

        valDesc.formatType         = FMT_DPFP;
        valDesc.value.doubleFormat = 60.0*PI/180.0;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_AreaPatternAngle, &valDesc)) 
            printf ("\n ERROR adding AreaPatternAngle to template");

        PatternDelta deltas;
        deltas.x = (mdlModelRef_getUorPerMaster(ACTIVEMODEL)/mdlModelRef_getUorPerMeter(ACTIVEMODEL)) * .5;  
        deltas.y = (mdlModelRef_getUorPerMaster(ACTIVEMODEL)/mdlModelRef_getUorPerMeter(ACTIVEMODEL)) * .5; 
        valDesc.formatType        = FMT_VOIDP;
        valDesc.value.voidPFormat = &deltas;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_AreaPatternDelta, &valDesc)) 
             printf ("\n ERROR adding AreaPatternDelta to template");
        valDesc.value.voidPFormat = NULL;

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private StatusInt createAreaPattern 
(
ElementTemplateP elementTemplate
)
    {
    MSWChar         cellName[256];
    ValueDescr      valDesc;

    if (SUCCESS != mdlElementTemplate_getElementParam (elementTemplate, &valDesc, ELEMENTPARAM_PatternCell, 0) && FMT_MSWCHAR == valDesc.formatType)
        return ERROR;

    wcsncpy(cellName, valDesc.value.wCharPFormat, 255);
    dlmSystem_mdlFree (valDesc.value.wCharPFormat);

    // make sure specified pattern cell is available
    if (SUCCESS != mdlCell_findCellEx (NULL, NULL, cellName, TRUE, DGNFILE_RIGHT_Export))
        return ERROR;

    DgnModelRefP    modelRef     = ACTIVEMODEL;     
    BoolInt         is3D         = mdlModelRef_is3D (modelRef); 
    Dpoint3d        points[5];  
    double          deltaUor;   
    double          delta        = 30.0;            
    MSElementDescrP shapeElmdscr = NULL;            
    MSElement       element;     
    int             fillMode     = 0;      /* => -1=use active, 0=No fill, 1=Fill    */          
    int             numParams    = 0;

    // --- activate the template so active symbology is set ---
    mdlCnv_masterToUOR (&deltaUor, delta/2.0, modelRef);

    mdlCnv_masterToUOR (&points[0].x, 80.0, modelRef);
    points[0].y = 0.0;
    points[0].z = 0.0;

    points[1] = points[0];
    points[1].x += deltaUor;

    points[2] = points[1];
    points[2].y += deltaUor;

    points[3] = points[0];
    points[3].y += deltaUor;

    points[4] = points[0];

    mdlShape_create (&element, NULL, points, 5, fillMode);
    mdlElmdscr_new (&shapeElmdscr, NULL, &element);

    // Add the attributes
    EditElemHandle  ehandle (shapeElmdscr, true, false); // Give the element descr to the handle; it will free it

    // apply the specified templates symbology
    // can not control pattern origin using standard function
    mdlElementTemplate_applySymbology (elementTemplate, ehandle);

    if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment (elementTemplate, ehandle))
        printf ("\n ERROR scheduling template attachment");

    // add the line to the file
    ehandle.AddToModel (modelRef);
    return SUCCESS;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createCellTemplate(void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, CELL_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        // define cell info
        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"ARROW";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_CellName, &valDesc)) 
            printf ("\n ERROR adding CellName to template");
        valDesc.value.wCharPFormat = NULL;
            
        DPoint3d scale;
        scale.x = .050;
        scale.y = .050;
        scale.z = .050;
        valDesc.formatType = FMT_VOIDP; // DPoint3d
        valDesc.value.voidPFormat = &scale;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Scale, &valDesc)) 
            printf ("\n ERROR adding Scale to template");
        valDesc.value.voidPFormat = NULL;

        // define terminator information
        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"ARROW";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Terminator, &valDesc)) 
            printf ("\n ERROR adding Terminator to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_DPFP;
        valDesc.value.doubleFormat = 3.25;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_TerminatorScale, &valDesc)) 
            printf ("\n ERROR adding Terminator Scale to template");

        // define active point info
        ActivePointData activePointData;
        memset (&activePointData, 0, sizeof(activePointData));
        activePointData.type = ACTIVEPOINTTYPE_Character;
        activePointData.value[0] = L'X';
        valDesc.formatType        = FMT_VOIDP;
        valDesc.value.voidPFormat = &activePointData;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_ActivePoint, &valDesc)) 
             printf ("\n ERROR adding ActivePoint to template");
          valDesc.value.voidPFormat = NULL;

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private  StatusInt createCell
(
ElementTemplateP elementTemplate 
)
    {
    DPoint3d        scale;
    MSWChar         cellName[256];
    ValueDescr      valDesc;
    DPoint3d        origin;
    DgnModelRefP    modelRef = ACTIVEMODEL;     

    scale.x = scale.y = scale.z = 1.0;

    mdlCnv_masterToUOR (&origin.x, 100.0, modelRef);
    mdlCnv_masterToUOR (&origin.y, 60.0, modelRef);
    origin.z = 0.0;

    if (SUCCESS == mdlElementTemplate_getElementParam (elementTemplate, &valDesc, ELEMENTPARAM_CellName, 0) && 
        FMT_MSWCHAR == valDesc.formatType)
        {
        wcsncpy(cellName, valDesc.value.wCharPFormat, 255);
        dlmSystem_mdlFree (valDesc.value.wCharPFormat);
        }

    if (SUCCESS == mdlElementTemplate_getElementParam (elementTemplate, &valDesc, ELEMENTPARAM_Scale, 0) && 
        FMT_VOIDP == valDesc.formatType)
        {
        memcpy (&scale, valDesc.value.voidPFormat, sizeof (scale));
        dlmSystem_mdlFree (valDesc.value.voidPFormat);
        }

    MSElementDescrP cellDscrP = NULL;
    MSElementDescrP pTags = NULL;

    mdlCell_getElmDscrExtended (&cellDscrP, NULL, &pTags, &origin, &scale, TRUE, modelRef, NULL, NULL, 
                                 0, FALSE, FALSE, TRUE, cellName, NULL);
    if (NULL == cellDscrP)
        return ERROR;

    if (NULL != pTags)
        mdlElmdscr_appendDscr (cellDscrP, pTags); 

    cellDscrP->h.isValid = 0;
    mdlElmdscr_validate (cellDscrP, ACTIVEMODEL);

    // Add the attributes
    EditElemHandle  ehandle (cellDscrP, true, false); // Give the element descr to the handle; it will free it

    if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment (elementTemplate, ehandle))
        printf ("\n ERROR scheduling template attachment");

    // add the line to the file
    ehandle.AddToModel (modelRef);

    // apply the specified templates symbology, since ehandle has been written then the cell name in the template will be used to replace cell
    // that was defined in the initial cell placement.
    mdlElementTemplate_applySymbology (elementTemplate, ehandle);

    return SUCCESS;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createTextTemplate(void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, TEXT_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        // define cell info
        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Example Text";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_TextStyle, &valDesc)) 
            printf ("\n ERROR adding TextStyle to template");
        valDesc.value.wCharPFormat = NULL;
            
        // define textstyle override info
        TextStyleOverrideData textStyleOverride;
        memset (&textStyleOverride, 0, sizeof (textStyleOverride));

        wcsncpy(textStyleOverride.fontName, L"Comic Sans MS", (sizeof(textStyleOverride.fontName)/sizeof(wchar_t))-1);
        textStyleOverride.modifiers |= (UInt32)TEXTSTYLEMOD_FontNamePresent;
        textStyleOverride.textJustification = TXTJUST_CC;
        textStyleOverride.modifiers |= (UInt32)TEXTSTYLEMOD_TextJustificationPresent;
        textStyleOverride.nodeJustification = TXTJUST_CC;
        textStyleOverride.modifiers |= (UInt32)TEXTSTYLEMOD_NodeJustificationPresent;

        valDesc.formatType        = FMT_VOIDP;
        valDesc.value.voidPFormat = &textStyleOverride;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_TextStyleOverride, &valDesc)) 
             printf ("\n ERROR adding TextStyleOverride to template");
        valDesc.value.voidPFormat = NULL;

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createText 
(
ElementTemplateP elementTemplate
)
    {
    DgnModelRefP    modelRef = ACTIVEMODEL;     
    DPoint3d        origin;
    TextParamWide   textParams;
    TextSizeParam   textSize;
    DPoint2d        templateTextSize;
    short           lineLength;
    double          annotationScaleFactor=1.0;
    BoolInt         bApplyAnnotationScaleFactor=FALSE;

    origin.x = 0.0;
    mdlCnv_masterToUOR (&origin.x, -100.0, modelRef);
    mdlCnv_masterToUOR (&origin.y, 100.0, modelRef);
    origin.z= 0.0;

    textSize.mode = TXT_BY_TILE_SIZE;

    if (SUCCESS == mdlElementTemplate_getTextParameters (elementTemplate, &textParams, &templateTextSize, &lineLength, 
                                                        annotationScaleFactor, bApplyAnnotationScaleFactor, modelRef))
        {
        textSize.size.width = templateTextSize.x;
        textSize.size.height = templateTextSize.y;
        }
    else
        {
        DgnTextStylePtr activeStyle = TextStyleManager::GetActiveStyle ();

        mdlTextStyle_getTextParamWideFromStyle (&textParams, &textSize.size.width, &textSize.size.height,
            NULL, activeStyle->GetID (), modelRef, TRUE);
        }

    MSElement textNodeElement;
    mdlTextNode_createWide (&textNodeElement, NULL, &origin, NULL, &textSize, &textParams);

    MSElementDescrP textNodeElementDescriptor = NULL;
    mdlElmdscr_new (&textNodeElementDescriptor, NULL, &textNodeElement);
    textNodeElementDescriptor->h.dgnModelRef = ACTIVEMODEL;

    mdlText_addStringsToNodeDscr (textNodeElementDescriptor, "What goes around\n");
    mdlText_addStringsToNodeDscr (textNodeElementDescriptor, "usually gets dizzy\n");
    mdlText_addStringsToNodeDscr (textNodeElementDescriptor, "and falls over.");

    EditElemHandle  ehandle (textNodeElementDescriptor, true, true);

    // apply the specified templates symbology
    mdlElementTemplate_applySymbology (elementTemplate, ehandle);

    if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment (elementTemplate, ehandle))
       printf ("\n ERROR scheduling template attachment");

    // add the text to the file
    ehandle.AddToModel (modelRef);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createMultilineTemplate (void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, MLINE_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Walls";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Level, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;// color by index
        valDesc.value.sLongFormat = 1;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
             printf ("\n ERROR adding Color to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 1;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Weight, &valDesc)) 
             printf ("\n ERROR adding Weight to template");

        valDesc.formatType = FMT_LONG; // line style by line code number
        valDesc.value.sLongFormat = 0;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
             printf ("\n ERROR adding LineStyle to template");

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Multi-Line Example";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_MlineStyle, &valDesc)) 
            printf ("\n ERROR adding level MlineStyle to template");
        valDesc.value.wCharPFormat = NULL;

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createMultiline 
(
ElementTemplateP elementTemplate
)
    {
    DPoint3d        points[2];
    DgnModelRefP    modelRef = ACTIVEMODEL;     

    MSWChar     mlineStyleName[256];
    ValueDescr  valDesc;

    if (SUCCESS == mdlElementTemplate_getElementParam (elementTemplate, &valDesc, ELEMENTPARAM_MlineStyle, 0) && 
        FMT_MSWCHAR == valDesc.formatType)
        {
        wcsncpy(mlineStyleName, valDesc.value.wCharPFormat, (sizeof(mlineStyleName)/sizeof(MSWChar))-1);
        dlmSystem_mdlFree (valDesc.value.wCharPFormat);

        mdlCnv_masterToUOR (&points[0].x, 100.0, modelRef);
        points[0].y = 0.0;
        points[0].z = 0.0;
        mdlCnv_masterToUOR (&points[1].x, 140.0, modelRef);
        points[1].y = 0.0;
        points[1].z = 0.0;
        MlineOffsetMode mode = MLINE_PLACE_BYCENTER;
        MSElementDescrP mlineElmdscr = NULL;

        if (SUCCESS == mdlMline_createUsingStyle (&mlineElmdscr, NULL, NULL, points, 2, mlineStyleName, &mode, NULL))
            {
            // Add the attributes
            EditElemHandle  ehandle (mlineElmdscr, true, false); // Give the element descr to the handle; it will free it

            // apply the specified templates symbology
            mdlElementTemplate_applySymbology (elementTemplate, ehandle);

            if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment (elementTemplate, ehandle))
                printf ("\n ERROR scheduling template attachment");

            // add the line to the file
            ehandle.AddToModel (modelRef);
            }
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createDimensionTemplate (void)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, DIMENSION_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Dimensions";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Level, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;// color by index
        valDesc.value.sLongFormat = 3;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
             printf ("\n ERROR adding Color to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 0;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Weight, &valDesc)) 
             printf ("\n ERROR adding Weight to template");

        valDesc.formatType = FMT_LONG; // line style by line code number
        valDesc.value.sLongFormat = 0;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
             printf ("\n ERROR adding LineStyle to template");

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Bubble Arrow";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_DimensionStyle, &valDesc)) 
            printf ("\n ERROR adding level DimensionStyle to template");
        valDesc.value.wCharPFormat = NULL;

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createDimension 
(
ElementTemplateP  elementTemplate
)
    {
    StatusInt  status = SUCCESS;

    DgnDimStyleP    srcStyle = mdlDimStyle_create ();
    if (NULL != srcStyle)
        {
        MSWChar     dimStyleName[256];
        ValueDescr  valDesc;

        if (SUCCESS == mdlElementTemplate_getElementParam (elementTemplate, &valDesc, ELEMENTPARAM_DimensionStyle, 0) && 
            FMT_MSWCHAR == valDesc.formatType)
            {
            wcsncpy(dimStyleName, valDesc.value.wCharPFormat, (sizeof(dimStyleName)/sizeof(MSWChar))-1);
            dlmSystem_mdlFree (valDesc.value.wCharPFormat);

            MSElement  dimElement;
            if (SUCCESS == (status = mdlDim_createStubUsingStyle ((MSElement *)&dimElement, NULL, NULL, DIMTYPE_SIZE_ARROW, 0, ACTIVEMODEL, dimStyleName)))
                {
                double          witLengthUor;
                Dpoint3d        points[3]; 
                MSElementDescrP dimElmdscr = NULL;            

                memset (points, 0, sizeof(points));
                mdlCnv_masterToUOR (&points[0].x, 80.0, ACTIVEMODEL);
                mdlCnv_masterToUOR (&points[0].y, 80.0, ACTIVEMODEL);
                mdlCnv_masterToUOR (&points[1].x, 110.0, ACTIVEMODEL);
                mdlCnv_masterToUOR (&points[1].y, 80.0, ACTIVEMODEL);

                mdlCnv_masterToUOR (&witLengthUor, 1.5, ACTIVEMODEL);

                // create a non-associative linear dimension
                for (int i=0; i<2; i++)
                    mdlDim_insertPoint (&dimElement, &points[i], NULL, -1, POINT_STD, ACTIVEMODEL);

                mdlDim_setHeight (&dimElement, witLengthUor, FALSE);
                mdlDim_validate (&dimElement);

                mdlElmdscr_new (&dimElmdscr, NULL, &dimElement);

                // Add the attributes
                EditElemHandle  ehandle (dimElmdscr, true, false); // Give the element descr to the handle; it will free it

                // apply the specified templates symbology
                mdlElementTemplate_applySymbology (elementTemplate, ehandle);

                if (SUCCESS != mdlElementTemplate_scheduleTemplateReferenceAttachment (elementTemplate, ehandle))
                    printf ("\n ERROR scheduling template attachment");

                // add the line to the file
                ehandle.AddToModel (ACTIVEMODEL);
                }

            mdlDimStyle_free (&srcStyle);
            }
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createOrphanCellHeaderTemplate ()
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, HEADER_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        // define cell info
        DPoint3d scale;
        scale.x = 1.0;
        scale.y = 1.0;
        scale.z = 1.0;
        valDesc.formatType = FMT_VOIDP; // DPoint3d
        valDesc.value.voidPFormat = &scale;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Scale, &valDesc)) 
            printf ("\n ERROR adding Scale to template");
        valDesc.value.voidPFormat = NULL;

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createOrphanCellComponent1Template ()
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, COMPONENT1_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Equipment";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Level, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;// color by index
        valDesc.value.sLongFormat = 6;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
             printf ("\n ERROR adding Color to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 3;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Weight, &valDesc)) 
             printf ("\n ERROR adding Weight to template");

        valDesc.formatType = FMT_LONG; // line style by line code number
        valDesc.value.sLongFormat = 0;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
             printf ("\n ERROR adding LineStyle to template");

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createOrphanCellComponent2Template ()
    {
    ElementTemplateP  elementTemplate = NULL;

    if (SUCCESS == mdlElementTemplate_create (&elementTemplate, COMPONENT2_TEMPLATE_NAME))
        {
        int             addedIndex; 
        ValueDescr      valDesc;    

        valDesc.formatType = FMT_MSWCHAR;
        valDesc.value.wCharPFormat = L"Equipment";
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Level, &valDesc)) 
            printf ("\n ERROR adding level Default to template");
        valDesc.value.wCharPFormat = NULL;

        valDesc.formatType = FMT_LONG;// color by index
        valDesc.value.sLongFormat = 5;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Color, &valDesc)) 
             printf ("\n ERROR adding Color to template");

        valDesc.formatType = FMT_LONG;
        valDesc.value.sLongFormat = 1;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_Weight, &valDesc)) 
             printf ("\n ERROR adding Weight to template");

        valDesc.formatType = FMT_LONG; // line style by line code number
        valDesc.value.sLongFormat = 1;
        if (SUCCESS != mdlElementTemplate_addElementParam (elementTemplate, &addedIndex, ELEMENTPARAM_LineStyle, &valDesc)) 
             printf ("\n ERROR adding LineStyle to template");

        mdlElementTemplate_save (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }
    }

/*---------------------------------------------------------------------------------**//**
* Create Orphan Cell by creating and apply symbology to each component elements before
* adding in to the cell. Each component is associated to a different Element Template.
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createOrphanCell 
(
ElementTemplateP headerTemplate,
ElementTemplateP component1Template,
ElementTemplateP component2Template
)
    {
    DPoint3d        origin;
    Dpoint3d        points[5];  
    DgnModelRefP    modelRef = ACTIVEMODEL;     
    int             fillMode = 0;      /* => -1=use active, 0=No fill, 1=Fill    */          
    BoolInt         is3D     = mdlModelRef_is3D (modelRef); 
    double          deltaUor;   

    mdlCnv_masterToUOR (&origin.x, 45.0, modelRef);
    mdlCnv_masterToUOR (&origin.y, 45.0, modelRef);
    origin.z = 0.0;

    MSElementDescr* pCellElmDscr = NULL;
    MSElementDescr* shapeElmdscr = NULL;
    MSElementDescr* cirDescr     = NULL;
    MSElement        cellElement;
    MSElement        shapeElement;
    MSElement        cirElem;

    mdlCell_create(&cellElement, NULL, &origin, FALSE);
    mdlElmdscr_new (&pCellElmDscr, NULL, &cellElement);

    points[0] = origin;

    // --- activate the template so active symbology is set ---
    mdlCnv_masterToUOR (&deltaUor, 25.0, modelRef);

    points[0].z = 0.0;
    points[1] = points[0];
    points[1].x += deltaUor;

    points[2] = points[1];
    points[2].y += deltaUor;

    points[3] = points[0];
    points[3].y += deltaUor;

    points[4] = points[0];

    mdlShape_create (&shapeElement, NULL, points, 5, fillMode);
    mdlElmdscr_new (&shapeElmdscr, NULL, &shapeElement);

    // apply the template symbology to the element before adding it to the cell header
    EditElemHandle  component1Handle (shapeElmdscr, true, false); 
    mdlElementTemplate_applySymbology (component1Template, component1Handle);
    mdlElementTemplate_scheduleTemplateReferenceAttachment (component1Template, component1Handle);
    mdlElmdscr_appendDscr (pCellElmDscr, component1Handle.ExtractElemDescr ());

    DPoint3d centerPt = points[0];
    double   radius = deltaUor/2.0;
    centerPt.x += radius;
    centerPt.y += radius;
	
    mdlEllipse_create (&cirElem, NULL, &centerPt, radius, radius, NULL /*&rMatNormal*/,0);
    mdlElmdscr_new (&cirDescr, NULL, &cirElem);

    // apply the template symbology to the element before adding it to the cell header
    EditElemHandle  component2Handle (cirDescr, true, false);
    mdlElementTemplate_applySymbology (component2Template, component2Handle);
    mdlElementTemplate_scheduleTemplateReferenceAttachment (component2Template, component2Handle);
    mdlElmdscr_appendDscr (pCellElmDscr, component2Handle.ExtractElemDescr ());

    pCellElmDscr->h.isValid = 0;
    mdlElmdscr_validate (pCellElmDscr, modelRef);

    EditElemHandle  cellhandle (pCellElmDscr, true, false); // Give the element descr to the handle; it will free it

    // no reason to apply symbology to type 2 cell header
    mdlElementTemplate_scheduleTemplateReferenceAttachment (headerTemplate, cellhandle);

    // add to file
    cellhandle.AddToModel (modelRef);
    }


/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void dumpActiveFileTemplateList 
(
)
    {
    ListRow* pRow = NULL;
    ListCell* pCell = NULL;

    ListModel   *pListModel = mdlListModel_create (2);
    mdlElementTemplate_populateNamesListModel (pListModel, 0, TRUE); //get full path

    int numLoaded = mdlListModel_getRowCount (pListModel);
    if (0 == numLoaded)
        {
        printf ("\nNo Element Template data found in the active file\n");
        return;
        }

    printf ("\n================== Active File Element Template List Data ========================\n");

    for (pRow = mdlListModel_getFirstRow (pListModel); pRow; pRow = mdlListModel_getNextRow (pListModel, pRow))
        {
        ValueDescr   valDesc;
        char* name = NULL;

        pCell = mdlListRow_getCellAtIndex (pRow, 0);
        mdlListCell_getDisplayText (pCell, &name);
        mdlListCell_getValue (pCell, &valDesc);
        ElementID elementID = valDesc.value.uLong64Format;

        printf ("\n Path = %s (%ld)", name, elementID);
        }

    printf ("\n=============== End of Active File Element Template List Data ====================\n");

    mdlListModel_destroy (pListModel, TRUE);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private ElementTemplateP  getTemplate (MSWChar*  pwTemplatePath)
    {
    ElementTemplateP  elementTemplate = NULL;

    mdlElementTemplate_loadByPath (&elementTemplate, pwTemplatePath);
    return elementTemplate;
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void placeElementsWithTemplates
(
)
    {
    ElementTemplateP  elementTemplate = NULL;

    if (NULL != (elementTemplate = getTemplate (LINEAR_TEMPLATE_NAME)))
        {
        createLine (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }

    // ------- shape with fill --------------
    if (NULL != (elementTemplate = getTemplate (SHAPE_TEMPLATE_NAME)))
        {
        createShape (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }

    // ------- shape with hatching --------------
    if (NULL != (elementTemplate = getTemplate (HATCH_TEMPLATE_NAME)))
        {
        createHatch (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }

    // --- cell library with cell "Circles" must be available ---
    if (NULL != (elementTemplate = getTemplate (AREAPATTERN_TEMPLATE_NAME)))
        {
        if (SUCCESS != createAreaPattern (elementTemplate))
            mdlElementTemplate_delete (&elementTemplate);
        else
            mdlElementTemplate_free (&elementTemplate);
       }

    // --- cell library with cell "ARROW" must be available ---
    if (NULL != (elementTemplate = getTemplate (CELL_TEMPLATE_NAME)))
        {
        if (SUCCESS != createCell (elementTemplate))
            mdlElementTemplate_delete (&elementTemplate);
        else
            mdlElementTemplate_free (&elementTemplate);
        }

    if (NULL != (elementTemplate = getTemplate (TEXT_TEMPLATE_NAME)))
        {
        createText (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }

    if (NULL != (elementTemplate = getTemplate (MLINE_TEMPLATE_NAME)))
        {
        createMultiline (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }

    if (NULL != (elementTemplate = getTemplate (DIMENSION_TEMPLATE_NAME)))
        {
        createDimension (elementTemplate);
        mdlElementTemplate_free (&elementTemplate);
        }

    elementTemplate = getTemplate (HEADER_TEMPLATE_NAME);
    ElementTemplateP  component1Template = getTemplate (COMPONENT1_TEMPLATE_NAME);
    ElementTemplateP  component2Template = getTemplate (COMPONENT2_TEMPLATE_NAME);

    if (NULL != elementTemplate && NULL != component1Template && NULL != component2Template)
        createOrphanCell (elementTemplate, component1Template, component2Template);

    if (NULL != elementTemplate)
        mdlElementTemplate_free (&elementTemplate);

    if (NULL != component1Template)
        mdlElementTemplate_free (&component1Template);

    if (NULL != component2Template)
        mdlElementTemplate_free (&component2Template);
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void createTemplates
(
)
    {
    ElementTemplateP  elementTemplate = NULL;

    createLinearTemplate ();
    createShapeTemplate ();
    createHatchTemplate ();
    createAreaPatternTemplate ();
    createCellTemplate ();
    createTextTemplate ();
    createMultilineTemplate ();
    createDimensionTemplate ();
    createOrphanCellHeaderTemplate ();
    createOrphanCellComponent1Template ();
    createOrphanCellComponent2Template ();

    mdlElementTemplate_refreshUserInterface ();
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
void customizeTestApp_command_test_templates 
(
char*   unparsed
)
    {
    createTemplates ();
    placeElementsWithTemplates ();
    dumpActiveFileTemplateList ();
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private void customizeTestApp_command_testExpressions
(
char*   unparsed
)
    {
    BoolInt result;

    // evaluate expression in ustation.dgnlib
    if (SUCCESS == mdlNamedExpression_EvaluateAsCriteria (&result, L"Is2DModel", NULL))
        {
        printf ("\nIs2DModel=%s", result!=FALSE?"True":"False");
        }
  /* dropped out for use with the beta 2 Athens
    MSWChar resultString[1024];
    if (SUCCESS == mdlNamedExpression_EvaluateAsString (resultString, DIM(resultString), L"Is2DModel", NULL))
        {
        printf ("\nIs2DModel=%S", resultString);
        }

    MSWChar currentCommand[1024];
    if (SUCCESS == mdlNamedExpression_EvaluateAsString (currentCommand, DIM(currentCommand), L"CommandString", NULL))
        {
        printf ("\nCommandString=%S", currentCommand);
        }*/
    }

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
Private int     customizeTestApp_onUnload
(
void    
)    
    {
    return 0;
    }

/*---------------------------------------------------------------------------------**//**
* Commands
+---------------+---------------+---------------+---------------+---------------+------*/
Private MdlCommandNumber commandNumber[] = 
    {
        {customizeTestApp_command_test_templates,           CMD_CUSTOMIZE_TEST_TEMPLATES },
        {customizeTestApp_command_testExpressions,          CMD_CUSTOMIZE_TEST_EXPRESSIONS },
        0
    };

/*---------------------------------------------------------------------------------**//**
* @bsimethod                                    Bentley                         05/2008
+---------------+---------------+---------------+---------------+---------------+------*/
extern "C"  DLLEXPORT int   MdlMain
(
int             argc,
char           *argv[]
)
    {
    RscFileHandle   rscFileH;   /* a resource file handle */

    mdlResource_openFile (&rscFileH, NULL, 0);
    mdlSystem_registerCommandNumbers (commandNumber);
    mdlParse_loadCommandTable (NULL);

    mdlSystem_setFunction (SYSTEM_UNLOAD_PROGRAM, customizeTestApp_onUnload);

    mdlState_registerStringIds (STRINGLISTID_CommandNames,  STRINGLISTID_CommandNames);

    return SUCCESS;
    } 

