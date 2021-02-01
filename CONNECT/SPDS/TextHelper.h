#pragma once

#include <Mstn/MdlApi/MdlApi.h>
#include <DgnView/AccuDraw.h>
#include <DgnView/DgnElementSetTool.h>
#include <DgnPlatform/ITextEdit.h>
#include <DgnPlatform/TextHandlers.h>
#include <Mstn/isessionmgr.h>
#include <Mstn/ElementPropertyUtils.h>

#include <DgnPlatform\DgnPlatformAPI.h>
#include <DgnPlatform\CustomItemType.h>
#include <DgnPlatform\ECReportNode.h>
#include <ECObjects/ECObjectsAPI.h>

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;
USING_NAMESPACE_BENTLEY_ECOBJECT;

struct TextHelper {
public:

    static DgnFileP GetActiveDgnFile();

    static DgnModelP GetActiveDgnModelP();

    static bool TextStyleExists(WCharCP styleName);

    static DgnTextStylePtr GetDefaultTextStyle();

    static DgnTextStylePtr GetTextStyle(WCharCP styleName);
};

void textExample_createTextStyles(WCharCP);

void textExample_createText1Normal(WCharCP);

void textExample_createText1Vertical(WCharCP);

void textExample_createText1UpsideDown(WCharCP);

void textExample_createText1Backwards(WCharCP);

void textExample_createText2(WCharCP);