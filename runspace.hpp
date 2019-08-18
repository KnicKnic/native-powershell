#pragma once

#include "host.h"
#include "logger.hpp"
#include "host_internal.hpp"
#include "utils/typed_handles.hpp"
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
using namespace System::Management::Automation::Runspaces;
ref class RunspaceHolder {
public:
    Runspace^ runspace;
    NativePowerShell_ReceiveJsonCommand sendJsonCommand;
    Logger^ logger;
    MyHost^ host;
    void* context;
    RunspaceHolder(void * contextParam, NativePowerShell_ReceiveJsonCommand jsonCommand, Logger^ loggerParam)  
        : sendJsonCommand(jsonCommand)
        , logger(loggerParam)
        , context(contextParam)
    {}
    ~RunspaceHolder();
};