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


template<typename IndexType, typename ValueType>
ref class ConcurrentTable {
    IndexType lastUsedIndex = 0;
    System::Collections::Concurrent::ConcurrentDictionary<IndexType, ValueType>^ holder = gcnew System::Collections::Concurrent::ConcurrentDictionary<IndexType, ValueType>();
public:
    ValueType get(IndexType index) {
        ValueType  value;
        if (!holder->TryGetValue(index, value))
        {
            throw "Key Not found";
        }
        return value;
    }
    IndexType insert(ValueType value) {
        auto index = System::Threading::Interlocked::Increment(lastUsedIndex);
        holder[index] = value;
        return index;
    }
    ValueType remove(IndexType index) {
        ValueType value;
        if (!holder->TryRemove(index, value))
        {
            throw "Key Not found";
        }
        return value;
    }
};

template<typename IndexType, typename ValueType>
ref class TypedConcurrentTable {
    ConcurrentTable<long long, ValueType>^ holder = gcnew ConcurrentTable<long long, ValueType>();
public:
    ValueType get(IndexType index) {
        return holder->get(GetHandle(index));
    }
    IndexType insert(ValueType value) {
        return MakeHandle<IndexType>(holder->insert(value));
    }
    ValueType remove(IndexType index) {
        return holder->remove(GetHandle(index));
    }
};



ref class HandleTable {

private:
    static TypedConcurrentTable<RunspaceHandle, RunspaceHolder^>^ runspaces = gcnew TypedConcurrentTable<RunspaceHandle, RunspaceHolder^>();
    static TypedConcurrentTable<PowershellHandle, PowerShellHolder^>^ powershells = gcnew TypedConcurrentTable<PowershellHandle, PowerShellHolder^>();
public:
    static RunspaceHandle InsertRunspace(RunspaceHolder^ runspace);
    static RunspaceHolder^ GetRunspace(RunspaceHandle handle);
    static RunspaceHolder^ RemoveRunspace(RunspaceHandle handle);
    static PowershellHandle InsertPowershell(PowerShellHolder^ powershell);
    static PowerShellHolder^ GetPowershell(PowershellHandle handle);
    static PowerShellHolder^ RemovePowershell(PowershellHandle handle);
};