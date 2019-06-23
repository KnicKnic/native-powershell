#pragma once

#include "host.h"
#include "logger.h"
#include "autoptr.h"
#include "host_internal.h"
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
using namespace System::Management::Automation::Runspaces;
ref class RunspaceHolder {
public:
    Runspace^ runspace;
    ReceiveJsonCommand sendJsonCommand;
    Logger^ logger;
    MyHost^ host;
    RunspaceHolder(ReceiveJsonCommand jsonCommand, Logger^ loggerParam)  
        : sendJsonCommand(jsonCommand)
        , logger(loggerParam)
    {}
    ~RunspaceHolder();
};