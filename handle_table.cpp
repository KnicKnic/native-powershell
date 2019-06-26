#include "handle_table.h"


/*static HandleTable() {
runspaces = ;
}*/

inline RunspaceHandle HandleTable::InsertRunspace(RunspaceHolder^ runspace) {
    return runspaces->insert(runspace);
}

inline RunspaceHolder^ HandleTable::GetRunspace(RunspaceHandle handle) {
    return runspaces->get(handle);
}

inline RunspaceHolder^ HandleTable::RemoveRunspace(RunspaceHandle handle) {
    return runspaces->remove(handle);
}

inline PowershellHandle HandleTable::InsertPowershell(PowerShellHolder^ powershell) {
    return powershells->insert(powershell);
}

inline PowerShellHolder^ HandleTable::GetPowershell(PowershellHandle handle) {
    return powershells->get(handle);
}

inline PowerShellHolder^ HandleTable::RemovePowershell(PowershellHandle handle) {
    return powershells->remove(handle);
}



inline PowerShellObject HandleTable::InsertPSObject(System::Object^ object) {
    return psObjects->insert(object);
}
inline System::Object^ HandleTable::GetPSObject(PowerShellObject handle) {
    return psObjects->get(handle);
}
inline System::Object^ HandleTable::RemovePSObject(PowerShellObject handle) {
    return psObjects->get(handle);
}