#include "handle_table.hpp"


/*static HandleTable() {
runspaces = ;
}*/

inline NativePowerShell_RunspaceHandle HandleTable::InsertRunspace(RunspaceHolder^ runspace) {
    return runspaces->insert(runspace);
}

inline RunspaceHolder^ HandleTable::GetRunspace(NativePowerShell_RunspaceHandle handle) {
    return runspaces->get(handle);
}

inline RunspaceHolder^ HandleTable::RemoveRunspace(NativePowerShell_RunspaceHandle handle) {
    return runspaces->remove(handle);
}

inline NativePowerShell_PowerShellHandle HandleTable::InsertPowershell(PowerShellHolder^ powershell) {
    return powershells->insert(powershell);
}

inline PowerShellHolder^ HandleTable::GetPowershell(NativePowerShell_PowerShellHandle handle) {
    return powershells->get(handle);
}

inline PowerShellHolder^ HandleTable::RemovePowershell(NativePowerShell_PowerShellHandle handle) {
    return powershells->remove(handle);
}



inline NativePowerShell_PowerShellObject HandleTable::InsertPSObject(PSObject^ object) {
    return psObjects->insert(object);
}
inline PSObject^ HandleTable::GetPSObject(NativePowerShell_PowerShellObject handle) {
    return psObjects->get(handle);
}
inline PSObject^ HandleTable::RemovePSObject(NativePowerShell_PowerShellObject handle) {
    return psObjects->get(handle);
}