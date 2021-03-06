// test_unit.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include <vector>
#include <optional>
#include "host.h"
#include "utils/macros.hpp"
#include "utils/zero_resetable.hpp"
#include "utils/cpp_wrappers.hpp"

//#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_RUNNER // creating own main to do powershell native init
#include "./Catch2/single_include/catch2/catch.hpp"

using namespace std;
using namespace native_powershell;



struct SomeContext {
    std::wstring LoggerContext;
    std::wstring CommandContext;
    NativePowerShell_RunspaceHandle runspace;
    std::optional<NativePowerShell_PowerShellHandle> powershell;
};



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
            RunScript(realContext->runspace, realContext->powershell, L"[int]11", true);
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

TEST_CASE("nullptr | send-hostcommand") {
    auto command = +[](void* context, const wchar_t* s, NativePowerShell_PowerShellObject * input, unsigned long long inputCount, NativePowerShell_JsonReturnValues * returnValues)
    {};

    auto runspace = NativePowerShell_CreateRunspace((void*)0, command, nullptr);

    auto powershell = NativePowerShell_CreatePowerShell(runspace);

    REQUIRE(0 == NativePowerShell_AddScriptSpecifyScope(powershell, L"send-hostcommand -message \"hi\"; $nullptr | send-hostcommand -message \"hi\" ", FALSE));

    Invoker invoke(powershell);
}

// declaring at global scope to make testing for validateContext simpler
unsigned long long validateContext_contextValue = 94847573123111;
bool validateContext_validatedLogger = false;
bool validateContext_validatedCallback = false;


NativePowerShell_LogString_Holder MakeLogHolderFunction(NativePowerShell_LogString func) {
    NativePowerShell_LogString_Holder logHolder = { 0 };
    logHolder.Log = func;
    return logHolder;
}

#define MAKE_LOG_HOLDER(SomeFunc) &(MakeLogHolderFunction(SomeFunc))

TEST_CASE("validate context") {
    auto logger = +[](void* context, const wchar_t* s) {validateContext_validatedLogger = true;  REQUIRE(context != nullptr); REQUIRE((unsigned long long)context == validateContext_contextValue); };
    auto command = +[](void* context, const wchar_t* s, NativePowerShell_PowerShellObject * input, unsigned long long inputCount, NativePowerShell_JsonReturnValues * returnValues)
        {validateContext_validatedCallback = true;  REQUIRE(context != nullptr); REQUIRE((unsigned long long)context == validateContext_contextValue); };

    auto runspace = NativePowerShell_CreateRunspace((void *)validateContext_contextValue, command, MAKE_LOG_HOLDER(logger));

    auto powershell = NativePowerShell_CreatePowerShell(runspace);

    REQUIRE(0 == NativePowerShell_AddScriptSpecifyScope(powershell, L"write-host 5;send-hostcommand -message \"hi\"  ", FALSE));
    
    Invoker invoke(powershell);
    REQUIRE(validateContext_validatedLogger);
    REQUIRE(validateContext_validatedCallback);
}


TEST_CASE("test logger") {
    std::wstring resultString;
    auto logger = +[](void* context, const wchar_t* s) {std::wcout << std::wstring(s); (*((std::wstring*)(context))) += std::wstring(s); };
    
    auto runspace = NativePowerShell_CreateRunspace((void*)&resultString, nullptr, MAKE_LOG_HOLDER(logger));

    auto powershell = NativePowerShell_CreatePowerShell(runspace);

    REQUIRE(0 == NativePowerShell_AddScriptSpecifyScope(powershell, L"write-error a; write-debug b; write-information c; write-verbose d; write-warning e; write-error a; write-debug b; write-information c; write-verbose d; write-warning e;  ", FALSE));

    Invoker invoke(powershell);

    std::wstring strExpected(L"Debug: b\nInformation: c\nVerbose: d\nWarning: e\nDebug: b\nInformation: c\nVerbose: d\nWarning: e\nError: a\nError: a\n");

    REQUIRE(resultString == strExpected);
}

TEST_CASE("nested runspace"){
        struct SomeContext2 {
            bool logged;
            NativePowerShell_RunspaceHandle runspace;
            NativePowerShell_PowerShellHandle powershell;
        }; 
        auto logger = +[](void* context, const wchar_t* s) {((SomeContext2*)context)->logged = true; };

    auto command = +[](void* context, const wchar_t* s, NativePowerShell_PowerShellObject * input, unsigned long long inputCount, NativePowerShell_JsonReturnValues * returnValues)
    {
        auto realContext = (SomeContext2*)context;
        REQUIRE(realContext->logged == false);
        RunScript(realContext->runspace, realContext->powershell, L"write-host 15", true);

        REQUIRE(realContext->logged);

        return;
    };
    SomeContext2 context{ false };
    auto runspace = NativePowerShell_CreateRunspace(&context, command, MAKE_LOG_HOLDER(logger));
    context.runspace = runspace;
    REQUIRE(context.runspace != 0);

    auto powershell = NativePowerShell_CreatePowerShell(runspace);
    REQUIRE(powershell != 0);
    context.powershell = powershell;

    NativePowerShell_AddScriptSpecifyScope(powershell, L"send-hostcommand -message \"hi\"  ", FALSE);
    auto results = Invoker(powershell);
    REQUIRE(context.logged);
    REQUIRE(results.count == 0);

    NativePowerShell_DeletePowershell(powershell);
}


TEST_CASE("tests", "[native-powershell]")
{
    SomeContext context{ L"MyLoggerContext: ", L"MyCommandContext: " };
    auto runspace = NativePowerShell_CreateRunspace(&context, Command, MAKE_LOG_HOLDER(Logger));
    context.runspace = runspace;
    RunScript(runspace, nullopt, L"[int12", true);

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

        context.powershell = powershell2;
        Invoker invoke2(powershell2);
        context.powershell = nullopt;
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

//
//// need to uncomment test
//// note that send-hostcommand does not work in remote sessions, should not accept Command
//TEST_CASE("remotetest")
//{
//    SomeContext context{ L"MyLoggerContext: ", L"MyCommandContext: " };
//    auto computer = L"computer";
//    auto username = L"username";
//    auto pass = L"pass";
//    auto runspace = NativePowerShell_CreateRemoteRunspace(&context, Logger, computer, username, pass);
//    context.runspace = runspace;
//    RunScript(runspace, nullopt, L"[int12", true);
//
//    auto powershell = NativePowerShell_CreatePowerShell(runspace);
//    //AddScriptSpecifyScope(powershell, L"c:\\code\\psh_host\\script.ps1", 1);
//    //AddCommand(powershell, L"c:\\code\\go-net\\t3.ps1");
//    //AddScriptSpecifyScope(powershell, L"write-host $pwd", 0);
//    NativePowerShell_AddScriptSpecifyScope(powershell, L"hostname; 0;1;$null;dir c:\\", 1);
//
//    //AddCommandSpecifyScope(powershell, L"..\\..\\go-net\\t3.ps1", 0);
//    //AddScriptSpecifyScope(powershell, L"$a = \"asdf\"", 0);
//    //AddArgument(powershell, L"c:\\ddddddd");
//    {
//        Invoker invoke(powershell);
//
//        wcout << L"examining returned objects\n";
//        for (unsigned int i = 0; i < invoke.count; ++i) {
//            wcout << L"Got type: " << GetType(invoke[i]) << L"with value: " << GetToString(invoke[i]) << L'\n';
//        }
//
//        auto powershell2 = NativePowerShell_CreatePowerShell(runspace);
//
//        // note below will write to output, not return objects	
//        NativePowerShell_AddScriptSpecifyScope(powershell2,
//            L"write-host 'about to enumerate directory';"
//            L"write-host $args; $len = $args.length; write-host \"arg count $len\";"
//            L"$args | ft | out-string | write-host;"
//            L"@(1,'asdf',$null,$false) | send-hostcommand -message 'I sent the host a command' | write-host;"
//            L"send-hostcommand -message 'I sent the host a command' | write-host", 0);
//        NativePowerShell_AddArgument(powershell2, L"String to start");
//        NativePowerShell_AddPSObjectArguments(powershell2, invoke.objects, invoke.count);
//        NativePowerShell_AddArgument(powershell2, L"String to end");
//
//        context.powershell = powershell2;
//        Invoker invoke2(powershell2);
//        context.powershell = nullopt;
//    }
//    NativePowerShell_DeletePowershell(powershell);
//
//    powershell = NativePowerShell_CreatePowerShell(runspace);
//    //AddScriptSpecifyScope(powershell, L"c:\\code\\psh_host\\script.ps1", 1);
//    NativePowerShell_AddCommandSpecifyScope(powershell, L"..\\..\\go-net\\t3.ps1", 0);
//    //AddScriptSpecifyScope(powershell, L"write-host $a", 0);
//
//    //AddCommand(powershell, L"c:\\code\\go-net\\t3.ps1");
//    //AddArgument(powershell, L"c:\\ddddddd");
//    {
//        Invoker invoke(powershell);
//    }
//    NativePowerShell_DeletePowershell(powershell);
//
//    NativePowerShell_DeleteRunspace(runspace);
//    std::cout << "Hello World!\n";
//}

int main(int argc, char* argv[]) {
    // global setup...

    int result = Catch::Session().run(argc, argv);

    // global clean-up...

    return result;
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



