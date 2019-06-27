// test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <algorithm>
#include "../host.h"

using namespace std;

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

    template<typename T>
    void FreeFunction(T* ptr) {
        free((void *)ptr);
    }

template<typename T>
class ZeroResetable {
	T var;
public:
	ZeroResetable() : var(T{ 0 }) {}
	ZeroResetable(ZeroResetable&& rhs) : var(rhs.var) {
		rhs.var = T{ 0 };
	}
	ZeroResetable& operator=(ZeroResetable&& rhs) {
		var = rhs.var;
		rhs.var = T{ 0 };
		return *this;
	}
	ZeroResetable& operator=(T& rhs) {
		var = rhs;
		return *this;
	}
	ZeroResetable& operator=(T&& rhs) {
		var = rhs;
		return *this;
	}
	~ZeroResetable() {
		var = T{ 0 };
	}
	operator T& () { return var; }
	operator const T& ()const { return var; }
	T& get() { return var; }
	template<typename Y>
	auto operator[](Y i) {
		return var[i];
	}
	T* operator->() { return &var; }
	auto operator*() { return *var; }
	T* operator&() { return &var; }
	const bool operator!=(T t) const{ return var != t; }
};

#define DENY_COPY(T) T(T&)=delete ; T& operator=(T&) = delete;
#define DEFAULT_MOVE(T) T(T&&)=default; T& operator=(T&&) = default;

class Invoker {
public:
    Invoker(PowershellHandle handle) {
        exception = InvokeCommand(handle, &objects, &count);
    }
	DENY_COPY(Invoker);
	DEFAULT_MOVE(Invoker);
	//Invoker(Invoker&) = delete; Invoker& operator=(Invoker&) = delete;
    ~Invoker() {
        for (unsigned int i = 0; i < count; ++i) {
            ClosePowerShellObject(objects[i]);
        }
        if (objects != nullptr) {
            free(objects);
            count = 0;
        }
        ClosePowerShellObject(exception);
    }
    const bool CallFailed() const {
        return exception != nullptr;
    }
	PowerShellObject operator[](unsigned int i) {
		return objects[i];
	}
	PowerShellObject* results() {
		return objects;
	}
	ZeroResetable< PowerShellObject*> objects;
	ZeroResetable< unsigned int> count;
	ZeroResetable< PowerShellObject> exception;
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

std::wstring GetType(PowerShellObject handle) {
    if('\0' == IsPSObjectNullptr(handle))
        return CopyAndFree(GetPSObjectType(handle));
    return L"nullptr";
}
std::wstring GetToString(PowerShellObject handle) {
    if ('\0' == IsPSObjectNullptr(handle))
        return CopyAndFree(GetPSObjectToString(handle));
    return L"nullptr";

}

int main()
{
    InitLibrary(MallocWrapper, free);
    auto runspace = CreateRunspace(Command, Logger);
    auto powershell = CreatePowershell(runspace);
    //AddScriptSpecifyScope(powershell, L"c:\\code\\psh_host\\script.ps1", 1);
    //AddCommand(powershell, L"c:\\code\\go-net\\t3.ps1");
    //AddScriptSpecifyScope(powershell, L"write-host $pwd", 0);
    AddScriptSpecifyScope(powershell, L"0;1;$null;dir c:\\", 1);

	//AddCommandSpecifyScope(powershell, L"..\\..\\go-net\\t3.ps1", 0);
    //AddScriptSpecifyScope(powershell, L"$a = \"asdf\"", 0);
    //AddArgument(powershell, L"c:\\ddddddd");
    {
        Invoker invoke(powershell);

        wcout << L"examining returned objects\n";
        for (unsigned int i = 0; i < invoke.count; ++i) {
            wcout << L"Got type: " << GetType(invoke[i]) << L"with value: " << GetToString(invoke[i]) << L'\n';
        }

		auto powershell2 = CreatePowershell(runspace);

		// note below will write to output, not return objects	
		AddScriptSpecifyScope(powershell2, L"write-host 'about to enumerate directory'; write-host $args; $len = $args.length; write-host \"arg count $len\"; $args | ft | out-string | write-host", 0);
		AddArgument(powershell2, L"String to start");
		AddPSObjectArguments(powershell2, invoke.objects, invoke.count);
		AddArgument(powershell2, L"String to end");

		Invoker invoke2(powershell2);
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
