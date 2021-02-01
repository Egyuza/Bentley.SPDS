#include "TextHelper.h"

DgnFileP getActiveDgnFileP() {
    return ISessionMgr::GetActiveDgnFile();
}
DgnFileR getActiveDgnFileR() {
    return *ISessionMgr::GetActiveDgnFile();
}

DgnModelRefP getActiveDgnModelP() {
    return ISessionMgr::GetActiveModel();
}

bool isTextStyleExists(MSWCharCP styleName) {
    return TextStyleManager::Get(styleName, getActiveDgnFileR()).IsValid();
}

DgnTextStylePtr getDefaultTextStylePtr() {
    DgnFileP dgnFileP= getActiveDgnFileP();

    if (NULL != dgnFileP) {
        return TextStyleManager::GetActiveStyle(*dgnFileP);
    }
    return NULL;
}

DgnTextStylePtr getTextStylePtr(MSWCharCP styleName) {
    if (isTextStyleExists(styleName)) {
        return TextStyleManager::Get(styleName, getActiveDgnFileR());
    }
    return getDefaultTextStylePtr();
}

bool setActiveTextStyle(MSCharCP styleName) {
    MSWChar nameWP[MAX_STR_LEN];
    mbstowcs(nameWP, styleName, MAX_STR_LEN); // конвертация строки
    return setActiveTextStyle(nameWP);
}


bool setActiveTextStyle(MSWCharCP styleName) {
    return SUCCESS == TextStyleManager::SetActiveStyle(*getTextStylePtr(styleName));
}