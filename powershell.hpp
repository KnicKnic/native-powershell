#pragma once
#pragma once
#include "host.h"
#include "logger.hpp"
#include "runspace.hpp"
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
using namespace System::Management::Automation::Runspaces;
ref class PowerShellHolder {
public:
    PowerShell^ powershell;
    RunspaceHolder^ runspace;
    PowerShellHolder(RunspaceHolder^ runspaceHolder, PowerShell^ powershellParam) : runspace(runspaceHolder), powershell(powershellParam) {}
    ~PowerShellHolder() {
        delete powershell;
    }
};