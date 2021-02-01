#pragma once

#include <DgnPlatform/Tools/ConfigurationManager.h>

USING_NAMESPACE_BENTLEY_DGNPLATFORM;

struct CfgVarMonitor : public SimpleConfigurationVariableMonitor
{    
public:
    void _VariableChanged(WCharCP variableName) override;

    // ! Called when any change to the monitored Variable is detected. 
    // Application should retrieve the new configuration variable value and 
    // refresh their internal state.
    void _UpdateState(WCharCP variableName) override;
};