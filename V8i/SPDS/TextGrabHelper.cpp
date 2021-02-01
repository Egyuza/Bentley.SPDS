#include "TextGrabHelper.h"

TextGrabHelper& TextGrabHelper::Instance() {
    static TextGrabHelper inst;
    return inst;
}

const MSWChar* TextGrabHelper::commandName_ = L"SpdsTextGrabbing";
bool TextGrabHelper::isActive_ = false;
GrabCallbackFuncP TextGrabHelper::callbackFunc_ = NULL;

TextGrabHelper::TextGrabHelper()    
{
}

void TextGrabHelper::startGrabbing(GrabCallbackFuncP callback)
{
    callbackFunc_ = callback;
    
    mdlSystem_setFunction(SYSTEM_ELMDSCR_TO_FILE, &userSystem_elmDscrToFile);
    mdlInput_setFunction(INPUT_COMMAND_FILTER, inputCommandCallback);

    // возможная альтернатива:
    //mdlChangeTrack_setFunction(
    //    CHANGE_TRACK_FUNC_Changed, &callbackDgnFileChanged);
   
    { // активация требуемого стиля, для корректного отображения в Text Editor:
        char styleName[MAX_STR_LEN];
        if (SUCCESS == mdlSystem_getCfgVar(styleName, CFGV_TEXTSTYLE, MAX_STR_LEN)) {
            setActiveTextStyle(styleName);
        }
    }

    mdlState_setCurrentCommandName(commandName_);

    // запускаем диалог создания текстового элемента
    mdlDialog_cmdNumberQueue(false, CMD_PLACE_DIALOGTEXT, "", TRUE);
    
    isActive_ = true;
}

int TextGrabHelper::inputCommandCallback
(
    Inputq_element*  queueElementP  /* <=> pointer to queue element */
)
{    
    MSWChar commandName[MAX_STR_LEN];
    wcsncpy(commandName, mdlState_getCurrentCommandName(), MAX_STR_LEN);

    if (wcscmp(commandName, commandName_) == 0) {
        isActive_ = true;
    }
    else {
        isActive_ = false; // для надёжности
        // эти команды должны сбрасывать хуки:
        mdlInput_setFunction(INPUT_COMMAND_FILTER, NULL);
        mdlSystem_setFunction(SYSTEM_ELMDSCR_TO_FILE, NULL);
    }
    return INPUT_COMMAND_ACCEPT;
}

int TextGrabHelper::userSystem_elmDscrToFile
(
    ElmDscrToFile_Actions  action,
    DgnModelRefP  modelRef,
    UInt32  filepos,
    MSElementDescr  *newEdP,
    MSElementDescr  *oldEdp,
    MSElementDescr  **replacementEdPP
)
{
    if (isActive_) {
        isActive_ = false; // сброс триггера
    }
    else {
        // сбрасываем обработчик, чтобы не было лишних вызовов
        mdlSystem_setFunction(SYSTEM_ELMDSCR_TO_FILE, NULL);
        return ELMDTF_STATUS_SUCCESS;
    }

    if (action == ELMDTF_ACTION_APPEND) {

        if (mdlElement_getType(&newEdP->el) == TEXT_NODE_ELM ||
            mdlElement_getType(&newEdP->el) == TEXT_ELM)
        { // перехват события создания текстового элемента
        
            if (callbackFunc_ == NULL) {
                // todo сообщение об ошибке
                return ELMDTF_STATUS_ABORT;
            }

            TextGrabParam grabParam;
            {
                //mdlText_extractStringsFromDscrWide(
                //    grabParam.textWide, MAX_STR_LEN, newEdP);

                char text[MAX_STR_LEN];
                mdlText_extractStringsFromDscr(
                    text, MAX_STR_LEN, newEdP);

                { // конвертация  из MultiBite в Unicode
                    MSWChar uText[MAX_STR_LEN];
                    mdlCnv_convertMultibyteToUnicode(
                        text, -1, uText, MAX_STR_LEN);

                    for (int i = 0; i < MAX_STR_LEN; ++i)
                    {
                        MSWideChar& x = grabParam.textWide[i];
                        x = (short)uText[i];
                    }
                }

                { // подсчёт количества введённых строк:
                    grabParam.stringsCount = 
                        mdlText_countBufferStrings(text);
                }
                // TODO ! точка оказывается не такой, 
                // какую задаёт пользователь
                grabParam.origin = newEdP->el.text_3d.origin;
            }
            // запуск функции:
            callbackFunc_(grabParam);

            // аборт операции создания текстового элемента:
            return ELMDTF_STATUS_ABORT;            
        }
    }
    return ELMDTF_STATUS_SUCCESS;
}