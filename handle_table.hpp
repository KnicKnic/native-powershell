#pragma once
#include "host.h"
#include "runspace.hpp"
#include "powershell.hpp"
#include "utils/typed_handles.hpp"

ref class HandleTable {

private:
    static TypedConcurrentTable<NativePowerShell_PowerShellObject, PSObject^>^ psObjects = gcnew TypedConcurrentTable<NativePowerShell_PowerShellObject, PSObject^>();
        static TypedConcurrentTable<NativePowerShell_RunspaceHandle, RunspaceHolder^>^ runspaces = gcnew TypedConcurrentTable<NativePowerShell_RunspaceHandle, RunspaceHolder^>();
    static TypedConcurrentTable<NativePowerShell_PowerShellHandle, PowerShellHolder^>^ powershells = gcnew TypedConcurrentTable<NativePowerShell_PowerShellHandle, PowerShellHolder^>();
public:
    static NativePowerShell_RunspaceHandle InsertRunspace(RunspaceHolder^ runspace);
    static RunspaceHolder^ GetRunspace(NativePowerShell_RunspaceHandle handle);
    static RunspaceHolder^ RemoveRunspace(NativePowerShell_RunspaceHandle handle);
    static NativePowerShell_PowerShellHandle InsertPowershell(PowerShellHolder^ powershell);
    static PowerShellHolder^ GetPowershell(NativePowerShell_PowerShellHandle handle);
    static PowerShellHolder^ RemovePowershell(NativePowerShell_PowerShellHandle handle);
    static NativePowerShell_PowerShellObject InsertPSObject(PSObject^ object);
    static PSObject^ GetPSObject(NativePowerShell_PowerShellObject handle);
    static PSObject^ RemovePSObject(NativePowerShell_PowerShellObject handle);
};