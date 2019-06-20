#pragma once
#include "host.h"
#include "logger.h"
#include "autoptr.h"
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
using namespace System::Management::Automation::Runspaces;
ref class RunspaceHolder {
public:
    Runspace^ runspace;
    Logger* logger;
    RunspaceHolder(Runspace^ runspaceParam, Logger* loggerParam) : runspace(runspaceParam), logger(loggerParam) {}
    RunspaceHolder(Logger* loggerParam) : logger(loggerParam) {}

};