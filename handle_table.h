#pragma once
#include "host.h"
#include "runspace.h"
#include "powershell.h"
#include "typed_handles.h"

ref class HandleTable {

private:
    static TypedConcurrentTable<PowerShellObject, System::Object^>^ psObjects = gcnew TypedConcurrentTable<PowerShellObject, System::Object^>();
        static TypedConcurrentTable<RunspaceHandle, RunspaceHolder^>^ runspaces = gcnew TypedConcurrentTable<RunspaceHandle, RunspaceHolder^>();
    static TypedConcurrentTable<PowershellHandle, PowerShellHolder^>^ powershells = gcnew TypedConcurrentTable<PowershellHandle, PowerShellHolder^>();
public:
    static RunspaceHandle InsertRunspace(RunspaceHolder^ runspace);
    static RunspaceHolder^ GetRunspace(RunspaceHandle handle);
    static RunspaceHolder^ RemoveRunspace(RunspaceHandle handle);
    static PowershellHandle InsertPowershell(PowerShellHolder^ powershell);
    static PowerShellHolder^ GetPowershell(PowershellHandle handle);
    static PowerShellHolder^ RemovePowershell(PowershellHandle handle);
    static PowerShellObject InsertPSObject(System::Object^ object);
    static System::Object^ GetPSObject(PowerShellObject handle);
    static System::Object^ RemovePSObject(PowerShellObject handle);
};