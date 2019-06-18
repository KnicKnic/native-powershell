
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdio.h>
#include <msclr\marshal_cppstd.h>


#include "host.h"
using namespace System::Management::Automation::Runspaces;
using namespace System::Management::Automation;


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Globalization;
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
#include "my_host.h"

template<typename T,typename X>
T MakeHandle(X x) {
	return (T)x;
}

template<typename T>
long long GetHandle(T t)
{
	return (long long)t;
}

public ref class HandleTable {

public:
	static System::Collections::Concurrent::ConcurrentDictionary<long long, Runspace^>^ runspaces = gcnew System::Collections::Concurrent::ConcurrentDictionary<long long, Runspace^>();
	static System::Collections::Concurrent::ConcurrentDictionary<long long, PowerShell^>^ powershells = gcnew System::Collections::Concurrent::ConcurrentDictionary<long long, PowerShell^>();
	/*static HandleTable() {
		runspaces = ;
	}*/
	static RunspaceHandle InsertRunspace(Runspace^ runspace) {
		auto index = System::Threading::Interlocked::Increment(runspaceIndex);
		runspaces[index] = runspace;
		return MakeHandle<RunspaceHandle>(index);
	}
	static Runspace^ GetRunspace(RunspaceHandle handle) {
		Runspace^ runspace;
		if (!runspaces->TryGetValue(GetHandle(handle), runspace))
		{
			throw "Key Not found";
		}
		return runspace;
	}
	static Runspace^ RemoveRunspace(RunspaceHandle handle) {
		Runspace^ runspace;
		printf("About to remove runspace\n");
		if (!runspaces->TryRemove(GetHandle(handle), runspace))
		{
			printf("Remove runspace error\n");
			throw "Key Not found";
		}
		printf("Removed runspace\n");
		return runspace;
	}
	static PowershellHandle InsertPowershell(PowerShell^ powershell) {
		auto index = System::Threading::Interlocked::Increment(powershellIndex);
		powershells[index] = powershell;
		return MakeHandle<PowershellHandle>(index);
	}
	static PowerShell^ GetPowershell(PowershellHandle handle) {
		PowerShell^ powershell;
		if (!powershells->TryGetValue(GetHandle(handle), powershell))
		{
			throw "Key Not found";
		}
		return powershell;
	}
	static PowerShell^ RemovePowershell(PowershellHandle handle) {
		PowerShell^ powershell;
		printf("About to remove powershell\n");
		if (!powershells->TryRemove(GetHandle(handle), powershell))
		{
			printf("Remove powershell error\n");
			throw "Key Not found";
		}
		printf("Removed powershell\n");
		return powershell;
	}
private:

	static long long runspaceIndex = 0;
	static long long powershellIndex = 0;
};

long MakeDir(PowershellHandle handle, StringPtr path)
{
	auto managedPath = msclr::interop::marshal_as<System::String^>(path);
	auto powershell = HandleTable::GetPowershell(handle);
	powershell->AddCommand("mkdir");
	powershell->AddArgument(managedPath);
	powershell->Invoke();
	return 0;
}
long AddCommand(PowershellHandle handle, StringPtr command)
{
	auto managedCommand = msclr::interop::marshal_as<System::String^>(command);
	auto powershell = HandleTable::GetPowershell(handle);
	powershell->AddCommand(managedCommand);
	return 0;
}
long AddArgument(PowershellHandle handle, StringPtr argument)
{
    auto managedArgument = msclr::interop::marshal_as<System::String^>(argument);
    auto powershell = HandleTable::GetPowershell(handle);
    powershell->AddArgument(managedArgument);
    return 0;
}
long AddScript(PowershellHandle handle, StringPtr path, bool useLocalScope)
{
    auto managedPath = msclr::interop::marshal_as<System::String^>(path);
    auto powershell = HandleTable::GetPowershell(handle);
    powershell->AddScript(managedPath, useLocalScope);
    return 0;
}
long InvokeCommand(PowershellHandle handle)
{
	auto powershell = HandleTable::GetPowershell(handle);
    try {
        auto results = powershell->Invoke();
        for each (auto object in results) {

            System::Console::WriteLine("Got Object");
            System::Console::WriteLine(object->ToString());
            System::Console::WriteLine(object->GetType()->ToString());
            System::Console::WriteLine(object->BaseObject->GetType()->ToString());
        }
    }
    catch (System::Management::Automation::RuntimeException^ exception) {
        System::Console::WriteLine("Caught Exception of type " + exception->GetType()->ToString());
        System::Console::WriteLine(exception->ToString());
        if (exception->ErrorRecord) {
            System::Console::WriteLine("Powershell stack trace");
            System::Console::WriteLine(exception->ErrorRecord->ScriptStackTrace);
        }
    }
    catch (System::Object^ exception) {
        System::Console::WriteLine("Caught Exception of type " + exception->GetType()->ToString());
        System::Console::WriteLine(exception->ToString());

    }
	return 0;
}

PowershellHandle CreatePowershell(RunspaceHandle handle)
{
	auto powershell = PowerShell::Create();
	powershell->Runspace = HandleTable::GetRunspace(handle);
	return HandleTable::InsertPowershell(powershell);
}


void DeletePowershell(PowershellHandle handle)
{
	HandleTable::RemovePowershell(handle)->Stop();
}


RunspaceHandle CreateRunspace()
{
	Runspace^ runspace = RunspaceFactory::CreateRunspace(gcnew MyHost());
	runspace->Open();
	return HandleTable::InsertRunspace(runspace);
}


void DeleteRunspace(RunspaceHandle handle)
{
	HandleTable::RemoveRunspace(handle)->Close();
}

