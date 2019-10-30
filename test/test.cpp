// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <vector>
#include "host.h"
#include "utils/macros.hpp"
#include "utils/zero_resetable.hpp"
#include "utils/cpp_wrappers.hpp"

using namespace std;
using namespace native_powershell;

struct SomeContext {
    std::wstring LoggerContext;
    std::wstring CommandContext;
};

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

            std::optional<NativePowerShell_PowerShellHandle> powershellHandle;
            if (globalPowershell != nullptr) {
                powershellHandle = *globalPowershell;
            }
            // test nested creation
            RunScript(globalRunspace, powershellHandle, L"[int]11", true);
            auto& v = input[i];
            std::wcout << L"In data processing got " << GetToString(v) << L" of type " << GetType(v) << L'\n';
        }

        // allocate return object holders
        returnValues->count = 1 + inputCount;
        returnValues->objects = (NativePowerShell_GenericPowerShellObject*)NativePowerShell_DefaultAlloc(sizeof(*(returnValues->objects)) * returnValues->count);
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
}


int main()
{
    SomeContext context{ L"MyLoggerContext: ", L"MyCommandContext: " };
    NativePowerShell_LogString_Holder logHolder = { 0 };
    logHolder.Log = Logger;
    auto runspace = NativePowerShell_CreateRunspace(&context, Command, &logHolder);
    globalRunspace = runspace;
    RunScript(runspace, std::nullopt, L"[int12", true);

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
