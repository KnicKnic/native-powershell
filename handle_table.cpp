#include "handle_table.h"


/*static HandleTable() {
runspaces = ;
}*/

inline RunspaceHandle HandleTable::InsertRunspace(RunspaceHolder^ runspace) {
    auto index = System::Threading::Interlocked::Increment(runspaceIndex);
    runspaces[index] = runspace;
    return MakeHandle<RunspaceHandle>(index);
}

inline RunspaceHolder^ HandleTable::GetRunspace(RunspaceHandle handle) {
    RunspaceHolder^ runspace;
    if (!runspaces->TryGetValue(GetHandle(handle), runspace))
    {
        throw "Key Not found";
    }
    return runspace;
}

inline RunspaceHolder^ HandleTable::RemoveRunspace(RunspaceHandle handle) {
    RunspaceHolder^ runspace;
    printf("About to remove runspace\n");
    if (!runspaces->TryRemove(GetHandle(handle), runspace))
    {
        printf("Remove runspace error\n");
        throw "Key Not found";
    }
    printf("Removed runspace\n");
    return runspace;
}

inline PowershellHandle HandleTable::InsertPowershell(PowerShellHolder^ powershell) {
    auto index = System::Threading::Interlocked::Increment(powershellIndex);
    powershells[index] = powershell;
    return MakeHandle<PowershellHandle>(index);
}

inline PowerShellHolder^ HandleTable::GetPowershell(PowershellHandle handle) {
    PowerShellHolder^ powershell;
    if (!powershells->TryGetValue(GetHandle(handle), powershell))
    {
        throw "Key Not found";
    }
    return powershell;
}

inline PowerShellHolder^ HandleTable::RemovePowershell(PowershellHandle handle) {
    PowerShellHolder^ powershell;
    printf("About to remove powershell\n");
    if (!powershells->TryRemove(GetHandle(handle), powershell))
    {
        printf("Remove powershell error\n");
        throw "Key Not found";
    }
    printf("Removed powershell\n");
    return powershell;
}
