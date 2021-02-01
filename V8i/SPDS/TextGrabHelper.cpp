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

    // ��������� ������������:
    //mdlChangeTrack_setFunction(
    //    CHANGE_TRACK_FUNC_Changed, &callbackDgnFileChanged);
   
    { // ��������� ���������� �����, ��� ����������� ����������� � Text Editor:
        char styleName[MAX_STR_LEN];
        if (SUCCESS == mdlSystem_getCfgVar(styleName, CFGV_TEXTSTYLE, MAX_STR_LEN)) {
            setActiveTextStyle(styleName);
        }
    }

    mdlState_setCurrentCommandName(commandName_);

    // ��������� ������ �������� ���������� ��������
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
        isActive_ = false; // ��� ���������
        // ��� ������� ������ ���������� ����:
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
        isActive_ = false; // ����� ��������
    }
    else {
        // ���������� ����������, ����� �� ���� ������ �������
        mdlSystem_setFunction(SYSTEM_ELMDSCR_TO_FILE, NULL);
        return ELMDTF_STATUS_SUCCESS;
    }

    if (action == ELMDTF_ACTION_APPEND) {

        if (mdlElement_getType(&newEdP->el) == TEXT_NODE_ELM ||
            mdlElement_getType(&newEdP->el) == TEXT_ELM)
        { // �������� ������� �������� ���������� ��������
        
            if (callbackFunc_ == NULL) {
                // todo ��������� �� ������
                return ELMDTF_STATUS_ABORT;
            }

            TextGrabParam grabParam;
            {
                //mdlText_extractStringsFromDscrWide(
                //    grabParam.textWide, MAX_STR_LEN, newEdP);

                char text[MAX_STR_LEN];
                mdlText_extractStringsFromDscr(
                    text, MAX_STR_LEN, newEdP);

                { // �����������  �� MultiBite � Unicode
                    MSWChar uText[MAX_STR_LEN];
                    mdlCnv_convertMultibyteToUnicode(
                        text, -1, uText, MAX_STR_LEN);

                    for (int i = 0; i < MAX_STR_LEN; ++i)
                    {
                        MSWideChar& x = grabParam.textWide[i];
                        x = (short)uText[i];
                    }
                }

                { // ������� ���������� �������� �����:
                    grabParam.stringsCount = 
                        mdlText_countBufferStrings(text);
                }
                // TODO ! ����� ����������� �� �����, 
                // ����� ����� ������������
                grabParam.origin = newEdP->el.text_3d.origin;
            }
            // ������ �������:
            callbackFunc_(grabParam);

            // ����� �������� �������� ���������� ��������:
            return ELMDTF_STATUS_ABORT;            
        }
    }
    return ELMDTF_STATUS_SUCCESS;
}