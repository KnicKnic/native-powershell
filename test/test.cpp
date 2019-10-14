// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <vector>
#include "host.h"
#include "utils/macros.hpp"
#include "utils/zero_resetable.hpp"

using namespace std;

std::wstring GetType(NativePowerShell_PowerShellObject handle);
std::wstring GetToString(NativePowerShell_PowerShellObject handle);

const wchar_t* MallocCopy(const wchar_t* str)
{
    if (str == nullptr)
        return nullptr;

    size_t s = 0;
    for (; str[s] != '\0'; ++s) {
    }
    ++s;
    auto dest = (wchar_t*)malloc(s * sizeof(str[0]));
    std::copy(str, str + s, dest);
    return (const wchar_t*)dest;
}

struct SomeContext {
    std::wstring LoggerContext;
    std::wstring CommandContext;
};

    template<typename T>
    void FreeFunction(T* ptr) {
        free((void *)ptr);
    }


class Invoker {
public:
    Invoker(NativePowerShell_PowerShellHandle handle) {
        exception = NativePowerShell_InvokeCommand(handle, &objects, &count);
    }
	DENY_COPY(Invoker);
	DEFAULT_MOVE(Invoker);
    ~Invoker() {
        for (unsigned int i = 0; i < count; ++i) {
            NativePowerShell_ClosePowerShellObject(objects[i]);
        }
        if (objects != nullptr) {
            free(objects);
            count = 0;
        }
        NativePowerShell_ClosePowerShellObject(exception);
    }
    const bool CallFailed() const {
        return exception.operator!=( NativePowerShell_InvalidHandleValue);
    }
	NativePowerShell_PowerShellObject operator[](unsigned int i) {
		return objects[i];
	}
	NativePowerShell_PowerShellObject* results() {
		return objects;
	}
	ZeroResetable< NativePowerShell_PowerShellObject*> objects;
	ZeroResetable< unsigned int> count;
	ZeroResetable< NativePowerShell_PowerShellObject> exception;
private:
};

std::wstring CopyAndFree(const wchar_t * cStr) {
    if (cStr == nullptr) {
        throw std::exception(__FUNCTION__  " got nullptr");
    }
    wstring toRet(cStr);
    FreeFunction(cStr);
    return toRet;
}

std::wstring GetType(NativePowerShell_PowerShellObject handle) {
    if('\0' == NativePowerShell_IsPSObjectNullptr(handle))
        return CopyAndFree(NativePowerShell_GetPSObjectType(handle));
    return L"nullptr";
}
std::wstring GetToString(NativePowerShell_PowerShellObject handle) {
    if ('\0' == NativePowerShell_IsPSObjectNullptr(handle))
        return CopyAndFree(NativePowerShell_GetPSObjectToString(handle));
    return L"nullptr";

}

Invoker RunScript(NativePowerShell_RunspaceHandle& runspace, NativePowerShell_PowerShellHandle * parent, std::wstring command, bool useLocalScope, const std::vector<std::wstring>& elems) {

    NativePowerShell_PowerShellHandle powershell;
    if (parent != nullptr) {
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
Invoker RunScript(NativePowerShell_RunspaceHandle& runspace, NativePowerShell_PowerShellHandle* parent, std::wstring command, bool useLocalScope, const std::wstring& elems) {
    return RunScript(runspace, parent, command, useLocalScope, std::vector<std::wstring>({ elems }));
}
Invoker RunScript(NativePowerShell_RunspaceHandle& runspace, NativePowerShell_PowerShellHandle* parent, std::wstring command, bool useLocalScope) {
    return RunScript(runspace, parent, command, useLocalScope, std::vector<std::wstring>({  }));
}
Invoker RunScript(NativePowerShell_RunspaceHandle& runspace, NativePowerShell_PowerShellHandle* parent, std::wstring command, bool useLocalScope, const std::wstring& elem1, const std::wstring& elem2) {
    return RunScript(runspace, parent, command, useLocalScope, std::vector<std::wstring>({ elem1, elem2 }));
}
NativePowerShell_RunspaceHandle globalRunspace;
NativePowerShell_PowerShellHandle * globalPowershell = nullptr;

extern "C" {
    void Logger(void* context, const wchar_t* s)
    {
        auto realContext = (SomeContext*)context;
        std::wcout << realContext->LoggerContext << std::wstring(s) << L'\n';
    }
    void Command(void* context, const wchar_t* s, NativePowerShell_PowerShellObject* input, unsigned long long inputCount, NativePowerShell_JsonReturnValues* returnValues)
    {

        input; inputCount;
        auto realContext = (SomeContext*)context;
        std::wcout << realContext->CommandContext << std::wstring(s) << L'\n';

        for (size_t i = 0; i < inputCount; ++i) {
            // test nested creation
            RunScript(globalRunspace, globalPowershell, L"[int]11", true);
            auto& v = input[i];
            std::wcout << L"In data processing got " << GetToString(v) << L" of type " << GetType(v) << L'\n';
        }

        // allocate return object holders
        returnValues->count = 1 + inputCount;
        returnValues->objects = (NativePowerShell_GenericPowerShellObject*)malloc(sizeof(*(returnValues->objects)) * returnValues->count);
        if (returnValues->objects == nullptr) {
            throw "memory allocation failed for return values in command";
        }

        // allocate and fill out each object
        auto& object = returnValues->objects[0];
        object.releaseObject = char(1);
        object.type = NativePowerShell_PowerShellObjectTypeString;
        object.instance.string = MallocCopy(s);

        for (size_t i = 0; i < inputCount; ++i) {
            auto& v = returnValues->objects[1 + i];
            v.releaseObject = char(0);
            v.type = NativePowerShell_PowerShellObjectHandle;
            v.instance.psObject = input[i];
        }

        return;
    }

    unsigned char* MallocWrapper(unsigned long long size) {
        return (unsigned char*)malloc(size);
    }
}


int main()
{
    NativePowerShell_InitLibrary(MallocWrapper, free);
    SomeContext context{ L"MyLoggerContext: ", L"MyCommandContext: " };
    NativePowerShell_LogString_Holder logHolder = { 0 };
    logHolder.Log = Logger;
    auto runspace = NativePowerShell_CreateRunspace(&context, Command, &logHolder);
    globalRunspace = runspace;
    RunScript(runspace, nullptr, L"[int12", true);

    auto powershell = NativePowerShell_CreatePowerShell(runspace);
    //AddScriptSpecifyScope(powershell, L"c:\\code\\psh_host\\script.ps1", 1);
    //AddCommand(powershell, L"c:\\code\\go-net\\t3.ps1");
    //AddScriptSpecifyScope(powershell, L"write-host $pwd", 0);
    NativePowerShell_AddScriptSpecifyScope(powershell, L"0;1;$null;dir c:\\", 1);

	//AddCommandSpecifyScope(powershell, L"..\\..\\go-net\\t3.ps1", 0);
    //AddScriptSpecifyScope(powershell, L"$a = \"asdf\"", 0);
    //AddArgument(powershell, L"c:\\ddddddd");
    {
        Invoker invoke(powershell);

        wcout << L"examining returned objects\n";
        for (unsigned int i = 0; i < invoke.count; ++i) {
            wcout << L"Got type: " << GetType(invoke[i]) << L"with value: " << GetToString(invoke[i]) << L'\n';
        }

		auto powershell2 = NativePowerShell_CreatePowerShell(runspace);

		// note below will write to output, not return objects	
        NativePowerShell_AddScriptSpecifyScope(powershell2,
            L"write-host 'about to enumerate directory';"
            L"write-host $args; $len = $args.length; write-host \"arg count $len\";"
            L"$args | ft | out-string | write-host;"
            L"@(1,'asdf',$null,$false) | send-hostcommand -message 'I sent the host a command' | write-host;"
            L"send-hostcommand -message 'I sent the host a command' | write-host", 0);
        NativePowerShell_AddArgument(powershell2, L"String to start");
        NativePowerShell_AddPSObjectArguments(powershell2, invoke.objects, invoke.count);
        NativePowerShell_AddArgument(powershell2, L"String to end");

        globalPowershell = &powershell2;
		Invoker invoke2(powershell2);
        globalPowershell = nullptr;
    }
    NativePowerShell_DeletePowershell(powershell);

    powershell = NativePowerShell_CreatePowerShell(runspace);
    //AddScriptSpecifyScope(powershell, L"c:\\code\\psh_host\\script.ps1", 1);
    NativePowerShell_AddCommandSpecifyScope(powershell, L"..\\..\\go-net\\t3.ps1", 0);
    //AddScriptSpecifyScope(powershell, L"write-host $a", 0);

    //AddCommand(powershell, L"c:\\code\\go-net\\t3.ps1");
    //AddArgument(powershell, L"c:\\ddddddd");
    {
        Invoker invoke(powershell);
    }
    NativePowerShell_DeletePowershell(powershell);

    NativePowerShell_DeleteRunspace(runspace);
    std::cout << "Hello World!\n"; 
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
