#pragma once

#include <string>
#include <vector>
#include <optional>

#include "utils/macros.hpp"
#include "utils/zero_resetable.hpp"


extern "C" {
    unsigned char* NativePowerShell_MallocWrapper(unsigned long long size) {
        return (unsigned char*)NativePowerShell_DefaultAlloc(size);
    }
}

namespace native_powershell{


template<typename T>
void FreeFunction(T* ptr) {
    NativePowerShell_DefaultFree((void*)ptr);
}

inline
std::wstring GetType(NativePowerShell_PowerShellObject handle);
inline
std::wstring GetToString(NativePowerShell_PowerShellObject handle);

inline
const wchar_t* MallocCopy(const wchar_t* str)
{
    if (str == nullptr)
        return nullptr;

    size_t s = 0;
    for (; str[s] != '\0'; ++s) {
    }
    ++s;
    auto dest = (wchar_t*)NativePowerShell_DefaultAlloc(s * sizeof(str[0]));
    std::copy(str, str + s, dest);
    return (const wchar_t*)dest;
}


class Invoker {
public:
    inline
    Invoker(NativePowerShell_PowerShellHandle handle) {
        exception = NativePowerShell_InvokeCommand(handle, &objects, &count);
    }
    DENY_COPY(Invoker);
    DEFAULT_MOVE(Invoker);
    inline
    ~Invoker() {
        for (unsigned int i = 0; i < count; ++i) {
            if (objectsToNotClose.size() == 0 || !objectsToNotClose[i]) {
                NativePowerShell_ClosePowerShellObject(objects[i]);
            }
        }
        if (objects != nullptr) {
            NativePowerShell_DefaultFree(objects);
            count = 0;
        }
        NativePowerShell_ClosePowerShellObject(exception);
    }
    inline
    const bool CallFailed() const {
        return exception.operator!=(NativePowerShell_InvalidHandleValue);
    }
    inline
    NativePowerShell_PowerShellObject operator[](unsigned int i) {
        return objects[i];
    }
    inline
    NativePowerShell_PowerShellObject* results() {
        return objects;
    }
    inline
    void DoNotCloseIndex(unsigned int i) {
        if (objectsToNotClose.size() == 0) {
            objectsToNotClose.resize(count, false);
        }
        objectsToNotClose[i] = true;
    }
    ZeroResetable< NativePowerShell_PowerShellObject*> objects;
    ZeroResetable< unsigned int> count;
    std::vector<bool> objectsToNotClose;
    ZeroResetable< NativePowerShell_PowerShellObject> exception;
private:
};

inline
std::wstring CopyAndFree(const wchar_t* cStr) {
    if (cStr == nullptr) {
        throw "CopyAndFree got nullptr";
    }
    std::wstring toRet(cStr);
    FreeFunction(cStr);
    return toRet;
}

inline
std::wstring GetType(NativePowerShell_PowerShellObject handle) {
    if ('\0' == NativePowerShell_IsPSObjectNullptr(handle))
        return CopyAndFree(NativePowerShell_GetPSObjectType(handle));
    return L"nullptr";
}
inline
std::wstring GetToString(NativePowerShell_PowerShellObject handle) {
    if ('\0' == NativePowerShell_IsPSObjectNullptr(handle))
        return CopyAndFree(NativePowerShell_GetPSObjectToString(handle));
    return L"nullptr";

}

inline
Invoker RunScript(NativePowerShell_RunspaceHandle& runspace, std::optional<NativePowerShell_PowerShellHandle> parent, std::wstring command, bool useLocalScope, const std::vector<std::wstring>& elems) {

    NativePowerShell_PowerShellHandle powershell;
    if (parent) {
        powershell = NativePowerShell_CreatePowerShellNested(*parent);
    }
    else {
        powershell = NativePowerShell_CreatePowerShell(runspace);
    }
    NativePowerShell_AddScriptSpecifyScope(powershell, command.c_str(), useLocalScope ? 1 : 0);
    for (auto& arg : elems) {
        NativePowerShell_AddArgument(powershell, arg.c_str());
    }
    auto results = Invoker(powershell);

    NativePowerShell_DeletePowershell(powershell);
    return results;
}

inline
Invoker RunScript(NativePowerShell_RunspaceHandle& runspace, std::optional<NativePowerShell_PowerShellHandle> parent, std::wstring command, bool useLocalScope) {
    return RunScript(runspace, parent, command, useLocalScope, std::vector<std::wstring>({  }));
}

template<typename ...T>
Invoker RunScript(NativePowerShell_RunspaceHandle& runspace, std::optional<NativePowerShell_PowerShellHandle> parent, std::wstring command, bool useLocalScope, const std::wstring& elems, T&& ... t) {
    return RunScript(runspace, parent, command, useLocalScope, std::vector<std::wstring>({ elems, std::forward<T>(t)... }));
}

};