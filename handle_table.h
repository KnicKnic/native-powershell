#pragma once
#include "host.h"
#include "runspace.h"
#include "powershell.h"

template<typename T, typename X>
T MakeHandle(X x) {
    return (T)x;
}

template<typename T>
long long GetHandle(T t)
{
    return (long long)t;
}


ref class HandleTable {

public:
    static System::Collections::Concurrent::ConcurrentDictionary<long long, RunspaceHolder^>^ runspaces = gcnew System::Collections::Concurrent::ConcurrentDictionary<long long, RunspaceHolder^>();
    static System::Collections::Concurrent::ConcurrentDictionary<long long, PowerShellHolder^>^ powershells = gcnew System::Collections::Concurrent::ConcurrentDictionary<long long, PowerShellHolder^>();
    /*static HandleTable() {
        runspaces = ;
    }*/
    static RunspaceHandle InsertRunspace(RunspaceHolder^ runspace);
    static RunspaceHolder^ GetRunspace(RunspaceHandle handle);
    static RunspaceHolder^ RemoveRunspace(RunspaceHandle handle);
    static PowershellHandle InsertPowershell(PowerShellHolder^ powershell);
    static PowerShellHolder^ GetPowershell(PowershellHandle handle);
    static PowerShellHolder^ RemovePowershell(PowershellHandle handle);
private:

    static long long runspaceIndex = 0;
    static long long powershellIndex = 0;
};