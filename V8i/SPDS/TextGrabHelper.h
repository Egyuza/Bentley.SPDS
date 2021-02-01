#pragma once

#include <string>

#include <cmdlist.h>
#include <msdialog.fdf>
#include <mssystem.fdf>
#include <msinput.fdf>

#include "spds.h"
#include "CfgVariables.h"
#include "TextHelper.h"
#include <mselems.h>

struct TextGrabParam {
    MSWideChar textWide[MAX_STR_LEN];
    //char text[MAX_STR_LEN];
    DPoint3d origin;
    int stringsCount;
};

typedef void (*GrabCallbackFuncP) (const TextGrabParam& text);

class TextGrabHelper {
public:
    static TextGrabHelper& Instance();
    void startGrabbing(GrabCallbackFuncP callback);
    
private:
    // реализация синглтон -----------------------------------------------------
    TextGrabHelper();  // конструктор недоступен
    ~TextGrabHelper() {} // и деструктор
    // необходимо также запретить копирование, реализация не нужна
    TextGrabHelper(TextGrabHelper const&) {}
    TextGrabHelper& operator= (TextGrabHelper const&) {}
    // -------------------------------------------------------------------------
    
    static GrabCallbackFuncP callbackFunc_; // ссылка на функцию должна быть статической

    // триггер запущенного состояния процесса перехвата текста, для надёжности
    static bool isActive_;
    static const MSWChar* commandName_;

    static int userSystem_elmDscrToFile
    (
        ElmDscrToFile_Actions  action,
        DgnModelRefP  modelRef,
        UInt32  filepos,
        MSElementDescr  *newEdP,
        MSElementDescr  *oldEdp,
        MSElementDescr  **replacementEdPP
    );

    static int inputCommandCallback
    (
        Inputq_element*  queueElementP  /* <=> pointer to queue element */
    );
};