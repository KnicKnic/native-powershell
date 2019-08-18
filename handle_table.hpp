#pragma once
#include "host.h"
#include "runspace.hpp"
#include "powershell.hpp"
#include "utils/typed_handles.hpp"

ref class HandleTable {

private:
    static TypedConcurrentTable<PowerShellObject, PSObject^>^ psObjects = gcnew TypedConcurrentTable<PowerShellObject, PSObject^>();
        static TypedConcurrentTable<RunspaceHandle, RunspaceHolder^>^ runspaces = gcnew TypedConcurrentTable<RunspaceHandle, RunspaceHolder^>();
    static TypedConcurrentTable<PowershellHandle, PowerShellHolder^>^ powershells = gcnew TypedConcurrentTable<PowershellHandle, PowerShellHolder^>();
public:
    static RunspaceHandle InsertRunspace(RunspaceHolder^ runspace);
    static RunspaceHolder^ GetRunspace(RunspaceHandle handle);
    static RunspaceHolder^ RemoveRunspace(RunspaceHandle handle);
    static PowershellHandle InsertPowershell(PowerShellHolder^ powershell);
    static PowerShellHolder^ GetPowershell(PowershellHandle handle);
    static PowerShellHolder^ RemovePowershell(PowershellHandle handle);
    static PowerShellObject InsertPSObject(PSObject^ object);
    static PSObject^ GetPSObject(PowerShellObject handle);
    static PSObject^ RemovePSObject(PowerShellObject handle);
};