#pragma once

#include "spds.h"
#include "ElementHelper.h"

USING_NAMESPACE_BENTLEY_USTN;
USING_NAMESPACE_BENTLEY_USTN_ELEMENT;

class EditSetTool : public MstnElementSetTool {
public:
    EditSetTool();
    ~EditSetTool();

protected:
    bool OnInstall() override;
    bool OnDataButton(MstnButtonEventCP ev) override;
    StatusInt OnElementModify(EditElemHandleR elHandle) override;
    bool OnPostLocate(HitPathCP path, char *cantAcceptReason) override;

};

