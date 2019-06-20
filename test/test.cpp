// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "../host.h"
extern "C" {
    void Logger(const wchar_t* s)
    {
        std::wcout << L"My Member Logger: " << std::wstring(s) << L'\n';
    }
    const wchar_t* Command(const wchar_t* s)
    {
        return s;
    }

    unsigned char* MallocWrapper(unsigned long long size) {
        return (unsigned char*)malloc(size);
    }
}

int main()
{
    InitLibrary(Command, MallocWrapper, free);
    auto runspace = CreateRunspace(Logger);
    auto powershell = CreatePowershell(runspace);
    AddScript(powershell, L"c:\\code\\psh_host\\script.ps1");
    //AddArgument(powershell, L"c:\\ddddddd");
    InvokeCommand(powershell);
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
