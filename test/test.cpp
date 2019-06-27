// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include "../host.h"
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
extern "C" {
    void Logger(const wchar_t* s)
    {
        std::wcout << L"My Member Logger: " << std::wstring(s) << L'\n';
    }
    const wchar_t* Command(const wchar_t* s)
    {
        std::wcout << L"Send command got: " << std::wstring(s) << L'\n';
        return MallocCopy(s);
    }

    unsigned char* MallocWrapper(unsigned long long size) {
        return (unsigned char*)malloc(size);
    }
}

class Invoker {
public:
    Invoker(PowershellHandle handle) {
        exception = InvokeCommand(handle, &objects, &objectCount);
    }
    ~Invoker() {
        for (unsigned int i = 0; i < objectCount; ++i) {
            ClosePowerShellObject(objects[i]);
        }
        if (objects != nullptr) {
            free(objects);
            objectCount = 0;
        }
        ClosePowerShellObject(exception);
    }
    bool CallFailed() const {
        return exception != nullptr;
    }
    unsigned int Count()const {
        return objectCount;
    }
    PowerShellObject operator[](unsigned int i) {
        return objects[i];
    }
private:
    PowerShellObject* objects = nullptr;
    unsigned int objectCount = 0;
    PowerShellObject exception = nullptr;
};


int main()
{
    InitLibrary(MallocWrapper, free);
    auto runspace = CreateRunspace(Command, Logger);
    auto powershell = CreatePowershell(runspace);
    //AddScriptSpecifyScope(powershell, L"c:\\code\\psh_host\\script.ps1", 1);
    //AddCommand(powershell, L"c:\\code\\go-net\\t3.ps1");
    //AddScriptSpecifyScope(powershell, L"write-host $pwd", 0);

	AddCommandSpecifyScope(powershell, L"..\\..\\go-net\\t3.ps1", 0);
    //AddScriptSpecifyScope(powershell, L"$a = \"asdf\"", 0);
    //AddArgument(powershell, L"c:\\ddddddd");
    {
        Invoker invoke(powershell);
    }
    DeletePowershell(powershell);

    powershell = CreatePowershell(runspace);
    //AddScriptSpecifyScope(powershell, L"c:\\code\\psh_host\\script.ps1", 1);
    AddCommandSpecifyScope(powershell, L"..\\..\\go-net\\t3.ps1", 0);
    //AddScriptSpecifyScope(powershell, L"write-host $a", 0);

    //AddCommand(powershell, L"c:\\code\\go-net\\t3.ps1");
    //AddArgument(powershell, L"c:\\ddddddd");
    {
        Invoker invoke(powershell);
    }
    DeletePowershell(powershell);

    DeleteRunspace(runspace);
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
