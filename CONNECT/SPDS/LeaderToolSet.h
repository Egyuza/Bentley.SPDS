#pragma once

#include "spds.h"
#include "CfgVarMonitor.h"

USING_NAMESPACE_BENTLEY_DGNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM;
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT;

struct LeaderToolSet : public DgnElementSetTool {
protected:
    bvector<DPoint3d>   m_points;

    CfgVarMonitor CVMonitor;

    LeaderToolSet(int toolName) : DgnElementSetTool(toolName) {}

    // pure virtual method, sub-class must override!
    StatusInt _OnElementModify(EditElementHandleR eeh) override {
        return SUCCESS;
    }

    virtual void _OnPostInstall() override;

    virtual bool _OnDataButton(DgnButtonEventCR ev) override;
    virtual bool _OnResetButton(DgnButtonEventCR ev) override;
    virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;
    virtual void _OnUndoPreviousStep() override;

    // Inherited via DgnElementSetTool
    virtual void _OnRestartTool() override { InstallNewInstance(GetToolId()); }

    bool CreateCell(EditElementHandleR eeh, bvector<DPoint3d> const& points);
    void CreateAcceptedSegmentsTransient();
    void SetupAndPromptForNextAction();

public:
    static void InstallNewInstance(int toolId);
};