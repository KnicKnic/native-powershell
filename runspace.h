#pragma once

#include "host.h"
#include "logger.h"
#include "host_internal.h"
#include "typed_handles.h"
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
using namespace System::Management::Automation::Runspaces;
ref class RunspaceHolder {
public:
    Runspace^ runspace;
    ReceiveJsonCommand sendJsonCommand;
    Logger^ logger;
    MyHost^ host;
    void* context;
    RunspaceHolder(void * contextParam, ReceiveJsonCommand jsonCommand, Logger^ loggerParam)  
        : sendJsonCommand(jsonCommand)
        , logger(loggerParam)
        , context(contextParam)
    {}
    ~RunspaceHolder();
};