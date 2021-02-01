#pragma once

#include    <mdl.h>
#include    <stdio.h>
#include    <string.h>
#include    <malloc.h>
#include    <mselemen.fdf>
#include    <mselmdsc.fdf>
#include    <mslinkge.fdf>
#include    <msscancrit.fdf>
#include    <mstagdat.fdf>
#include    <mselems.h>
#include    <mscell.fdf>
#include    <leveltable.fdf>
#include    <mslstyle.fdf>
#include    <msstrlst.h>
#include    <mscnv.fdf>
#include    <msdgnobj.fdf>
#include    <msmodel.fdf>
#include    <msview.fdf>
#include    <msviewinfo.fdf>
#include    <msvar.fdf>
#include    <dlmsys.fdf>
#include    <msdialog.fdf>

#include    <msrmgr.h>
#include    <mssystem.fdf>
#include    <msparse.fdf>

#include <toolsubs.h>

#include <elementref.h>
#include <msdependency.fdf>
#include <msassoc.fdf>
#include <msmisc.fdf>
#include <mslocate.fdf>
#include <msstate.fdf>
#include <msoutput.fdf>
#include <mstypes.h>
#include <MicroStationAPI.h>
#include <mstmatrx.fdf>
#include <accudraw.h>

#include <interface/ElemHandle.h>
#include <interface/ElemHandleGeometry.h>
#include <interface/element/DisplayHandler.h> 

#include <list>
//#include "stringapiset.h"

#include "spds.h"
#include "spdscmd.h"
#include "TextGrabHelper.h"

USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;
USING_NAMESPACE_TEXT


// vector - нельзя, так как у EditElemHandle - приватный оператор "="
typedef class std::list<EditElemHandle> ElementList;

// коэффициенты ориентации
struct Orientation {
    int kH; // коэффициент горизональной ориентации, значения = {1, -1}
    int kV; // коэффициент вертикальной ориентации, значения = {1, -1}
    bool isVertical;

    Orientation() :
        kH(1), kV(1), isVertical(false)
    {}

    Orientation(DPoint3dCR p0, DPoint3dCR p1) {
        kH = p1.x == p0.x ? 0 : 
            p1.x > p0.x ? 1 : -1;
        kV = p1.y == p0.y ? 0 : 
            p1.y >= p0.y ? 1 : -1;
        isVertical = (p1.x == p0.x && kV != 0);
    }
};

// TODO проверить, т.к. перечисление взято из более поздней версии - CONNECT
enum TextJustification {
    LeftTop = 0,
    LeftMiddle = 1,
    LeftBaseline = 2,
    LeftMarginTop = 3,
    LeftMarginMiddle = 4,
    LeftMarginBaseline = 5,
    CenterTop = 6,
    CenterMiddle = 7,
    CenterBaseline = 8,
    RightMarginTop = 9,
    RightMarginMiddle = 10,
    RightMarginBaseline = 11,
    RightTop = 12,
    RightMiddle = 13,
    RightBaseline = 14,
    LeftCap = 15,
    LeftDescender = 16,
    LeftMarginCap = 17,
    LeftMarginDescender = 18,
    CenterCap = 19,
    CenterDescender = 20,
    RightMarginCap = 21,
    RightMarginDescender = 22,
    RightCap = 23,
    RightDescender = 24,
    Invalid = 127
};

    
bool CreateLine(EditElemHandleR eehOut, DPoint3d* points);
bool CreateStringLine(EditElemHandleR eehOut, DPoint3d* points, int numVerts);
bool CreateCircle(
    EditElemHandleR eehOut, DPoint3dCR center, double radius, int fillMode);
bool CreateText(EditElemHandleR eehOut, TextGrabParam grabParam,
    DPoint3dCR origin, Orientation orient, UInt32 textStyle);
bool CreateCell(
    EditElemHandleR eehOut, MSWCharCP name, DPoint3dP origin, BoolInt pointCell);

bool AddChildToCell(EditElemHandleR cell, EditElemHandleR child);

DPoint3d getHorOrVertProjectionPoint(DPoint3dCR base, DPoint3dCR target);
DPoint3d Offset(DPoint3dCR base, double dx = 0.0, double dy = 0.0, bool scaled = true);

bool ToEditHandle(EditElemHandleR eehOut, MSElement element);
bool ToEditHandle(EditElemHandleR eehOut, MSElementDescrP elemDescrP);

void applyAnnotationScaleFactorToTextParams
(
    TextParamWide*  textParamP,
    TextSizeParam*  textSizeP,
    double          annotationScale
);

// todo Удалить, если не понадобятся: -------------------
ElementTemplateP  getTemplate(MSWChar*  pwTemplatePath);
void createTextTemplate(void);
void createText(ElementTemplateP elementTemplate);
//-------------------------------------------------------

bool Scale(EditElemHandleR eeh, DPoint3dCR point, double scale);

bool SetAccudrawContext
(
    long            flags,
    Dpoint3d*       origin = NULL,
    Dpoint3d*       delta = NULL,
    double const*   distance = NULL,
    double const*   angle = NULL,
    void*           orientation = NULL
);

bool setIsAnnotation(
    EditElemHandleR eeh, bool isAnnotation, double annScale = -1.0);

bool getTextParams(MSElementDescrP textNodeDescrP, // in
    DPoint3d* originP, // out
    TextSize* sizeP, // out
    Orientation* orientP, // out
    int* justP // out
);

//std::string cp1251_to_utf8(const char *str);

bool isTextElement(MSElement& el);
bool isTextElement(MSElementDescrP elDescrP);