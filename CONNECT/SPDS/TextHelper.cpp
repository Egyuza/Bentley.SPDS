#include "TextHelper.h"

// USING_NAMESPACE_BENTLEY_DGNPLATFORM;


DgnFileP TextHelper::GetActiveDgnFile() {
    return ISessionMgr::GetActiveDgnFile();
}

DgnModelP TextHelper::GetActiveDgnModelP() {
    return ISessionMgr::GetActiveDgnModelP();
}

bool TextHelper::TextStyleExists(WCharCP styleName) {
    DgnFileP activeDgnFile = TextHelper::GetActiveDgnFile();

    return (NULL != activeDgnFile) && (DgnTextStyle::GetByName(styleName, *activeDgnFile).IsValid());
}

DgnTextStylePtr TextHelper::GetDefaultTextStyle() {
    DgnFileP activeDgnFile = GetActiveDgnFile();

    if (NULL != activeDgnFile) {
        DgnTextStylePtr textStyle = DgnTextStyle::GetSettings(*activeDgnFile);
        return textStyle;
    }

    return NULL;
}

DgnTextStylePtr TextHelper::GetTextStyle(WCharCP styleName) {
    DgnFileP activeDgnFile = GetActiveDgnFile();

    //If style exists, get it by name and return it.
    if (TextStyleExists(styleName)) {
        DgnTextStylePtr textStyle = DgnTextStyle::GetByName(styleName, *activeDgnFile);
        return textStyle;
    }

    //Otherwise return default text style.
    return GetDefaultTextStyle();
}

//End TextExample

///*---------------------------------------------------------------------------------**//**
//* Map: key-in to function
//* Commands
//+---------------+---------------+---------------+---------------+---------------+--*/
//MdlCommandNumber s_commandNumbers[] =
//{
//    { (CmdHandler)textExample_createTextStyles, CMD_TEXTEXAMPLE_CREATETEXTSTYLES },
//    { (CmdHandler)textExample_createText1Normal, CMD_TEXTEXAMPLE_CREATETEXT_EXAMPLE1_NORMAL },
//    { (CmdHandler)textExample_createText1Vertical, CMD_TEXTEXAMPLE_CREATETEXT_EXAMPLE1_VERTICAL },
//    { (CmdHandler)textExample_createText1UpsideDown, CMD_TEXTEXAMPLE_CREATETEXT_EXAMPLE1_UPSIDEDOWN },
//    { (CmdHandler)textExample_createText1Backwards, CMD_TEXTEXAMPLE_CREATETEXT_EXAMPLE1_BACKWARDS },
//    { (CmdHandler)textExample_createText2, CMD_TEXTEXAMPLE_CREATETEXT_EXAMPLE2 },
//    0
//};
//
///*---------------------------------------------------------------------------------**//**
//* MdlMain
//
//* @bsimethod                                                              Bentley Systems
//+---------------+---------------+---------------+---------------+---------------+--*/
//extern "C" DLLEXPORT  void MdlMain(int argc, WCharCP argv[])
//{
//    /* Open the resource file.*/
//    RscFileHandle   rfHandle;
//    mdlResource_openFile(&rfHandle, nullptr, RSC_READONLY);
//
//    /* Register commands */
//    mdlSystem_registerCommandNumbers(s_commandNumbers);
//    mdlParse_loadCommandTable(nullptr);
//};