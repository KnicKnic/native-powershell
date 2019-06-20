
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
#include "host_internal.h"
#include "runspace.h"
#include "powershell.h"

#include "logger.h"

// Globals
ReceiveJsonCommand ReceiveJsonComamndPtr = nullptr;
FreePointer FreePointerPtr = nullptr;
AllocPointer AllocPointerPtr = nullptr;





void InitLibrary(ReceiveJsonCommand receivePtr, AllocPointer allocPtr, FreePointer freePtr) {
    ReceiveJsonComamndPtr = receivePtr;
    AllocPointerPtr = allocPtr;
    FreePointerPtr = freePtr;
}

template<typename T,typename X>
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
	static RunspaceHandle InsertRunspace(RunspaceHolder^ runspace) {
		auto index = System::Threading::Interlocked::Increment(runspaceIndex);
		runspaces[index] = runspace;
		return MakeHandle<RunspaceHandle>(index);
	}
	static RunspaceHolder^ GetRunspace(RunspaceHandle handle) {
        RunspaceHolder^ runspace;
		if (!runspaces->TryGetValue(GetHandle(handle), runspace))
		{
			throw "Key Not found";
		}
		return runspace;
	}
	static RunspaceHolder^ RemoveRunspace(RunspaceHandle handle) {
        RunspaceHolder^ runspace;
		printf("About to remove runspace\n");
		if (!runspaces->TryRemove(GetHandle(handle), runspace))
		{
			printf("Remove runspace error\n");
			throw "Key Not found";
		}
		printf("Removed runspace\n");
		return runspace;
	}
	static PowershellHandle InsertPowershell(PowerShellHolder^ powershell) {
		auto index = System::Threading::Interlocked::Increment(powershellIndex);
		powershells[index] = powershell;
		return MakeHandle<PowershellHandle>(index);
	}
	static PowerShellHolder^ GetPowershell(PowershellHandle handle) {
        PowerShellHolder^ powershell;
		if (!powershells->TryGetValue(GetHandle(handle), powershell))
		{
			throw "Key Not found";
		}
		return powershell;
	}
	static PowerShellHolder^ RemovePowershell(PowershellHandle handle) {
        PowerShellHolder^ powershell;
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
	auto powershell = HandleTable::GetPowershell(handle)->powershell;
	powershell->AddCommand("mkdir");
	powershell->AddArgument(managedPath);
	powershell->Invoke();
	return 0;
}
long AddCommand(PowershellHandle handle, StringPtr command)
{
	auto managedCommand = msclr::interop::marshal_as<System::String^>(command);
	auto powershell = HandleTable::GetPowershell(handle)->powershell;
	powershell->AddCommand(managedCommand);
	return 0;
}
long AddArgument(PowershellHandle handle, StringPtr argument)
{
    auto managedArgument = msclr::interop::marshal_as<System::String^>(argument);
    auto powershell = HandleTable::GetPowershell(handle)->powershell;
    powershell->AddArgument(managedArgument);
    return 0;
}
long AddScript(PowershellHandle handle, StringPtr path, bool useLocalScope)
{
    auto managedPath = msclr::interop::marshal_as<System::String^>(path);
    auto powershellHolder = HandleTable::GetPowershell(handle);
    auto powershell = powershellHolder->powershell;
    powershell->AddScript(managedPath, useLocalScope);
    return 0;
}
long InvokeCommand(PowershellHandle handle)
{
	auto powershellHolder = HandleTable::GetPowershell(handle);
    auto powershell = powershellHolder->powershell;
    auto Logger = powershellHolder->runspace->logger;
    try {
        auto results = powershell->Invoke();
        for each (auto object in results) {

            Logger->LogLine("Got Object");
            Logger->LogLine(object->ToString());
            Logger->LogLine(object->GetType()->ToString());
            Logger->LogLine(object->BaseObject->GetType()->ToString());
        }
        // TODO figure out why powershell errors are not displayed in host
        auto errors = powershell->Streams->Error;
        if (!(errors == nullptr )&& errors->Count > 0)
        {
            for each(auto err in errors)
            {
                Logger->LogLineError(err->ToString());
            }
        }
    }
    catch (System::Management::Automation::RuntimeException^ exception) {
        Logger->LogLineError("Caught Exception of type " + exception->GetType()->ToString());
        Logger->LogLineError(exception->ToString());
        if (exception->ErrorRecord) {
            Logger->LogLineError("Powershell stack trace");
            Logger->LogLineError(exception->ErrorRecord->ScriptStackTrace);
        }
    }
    catch (System::Object^ exception) {
        Logger->LogLineError("Caught Exception of type " + exception->GetType()->ToString());
        Logger->LogLineError(exception->ToString());

    }
	return 0;
}

PowershellHandle CreatePowershell(RunspaceHandle handle)
{
    auto runspaceHolder = HandleTable::GetRunspace(handle);
	auto powershell = PowerShell::Create();
    auto powershellHolder = gcnew PowerShellHolder(runspaceHolder, powershell);
	powershell->Runspace = runspaceHolder->runspace;
	return HandleTable::InsertPowershell(powershellHolder);
}


void DeletePowershell(PowershellHandle handle)
{
	HandleTable::RemovePowershell(handle)->powershell->Stop();
}


/// <summary>
/// Class that implements the GetProcCommand.
/// </summary>
[Cmdlet(VerbsCommunications::Send, "HostCommand")]
public ref class SendHostCommand : Cmdlet
{
public:
    [Parameter(Position = 0)]
    property System::String^ message;
        /// <summary>
        /// For each of the requested process names, retrieve and write
        /// the associated processes.
        /// </summary>
protected:
    void ProcessRecord()override
    {
        //// Get the current processes.

        //array< int >^ processes = gcnew array< int >(3);
        //processes[0] = 5;
        //processes[1] = 6;
        //processes[2] = 7;

        //// Write the processes to the pipeline making them available
        //// to the next cmdlet. The second argument (true) tells the
        //// system to enumerate the array, and send one process object
        //// at a time to the pipeline.
        //WriteObject(processes, true);

        //std::wstring nativeString = msclr::interop::marshal_as<std::wstring>(message);
        //printf("printf: %ws\n", nativeString.c_str());

        MyHost^ host = safe_cast<MyHost^>(this->CommandRuntime->Host->PrivateData->BaseObject);
        host->GetLogger()->LogLine(host->Name + message);
    }

} // End GetProcCommand class.
;



void SetISSEV(
    System::Management::Automation::Runspaces::InitialSessionStateEntryCollection<System::Management::Automation::Runspaces::SessionStateVariableEntry^>^ entries,
    System::String^ name,
    System::Object^ value) 
{
  int foundIndex = 0;
  for each(auto  entry in entries)
  {
    if (entry->Name->Equals(name, StringComparison::OrdinalIgnoreCase))
    {
      entries->RemoveItem( foundIndex);
      entries->Add(gcnew SessionStateVariableEntry(entry->Name, value, entry->Description));
      return;
    }

    foundIndex++;
  }

  throw gcnew System::IndexOutOfRangeException;
}
RunspaceHandle CreateRunspace(LogString BaseLogString)
{
    auto iss = InitialSessionState::CreateDefault();
    // Add the get-proc cmdlet to the InitialSessionState object.
    auto ssce = gcnew SessionStateCmdletEntry("Send-HostCommand", SendHostCommand::typeid, nullptr);
    iss->Commands->Add(ssce);

    // ensure logging enabled
    SetISSEV(iss->Variables, "ErrorActionPreference", System::Management::Automation::ActionPreference::Continue);
    SetISSEV(iss->Variables, "DebugPreference", System::Management::Automation::ActionPreference::Continue);
    SetISSEV(iss->Variables, "WarningPreference", System::Management::Automation::ActionPreference::Continue);
    SetISSEV(iss->Variables, "VerbosePreference", System::Management::Automation::ActionPreference::Continue);
    SetISSEV(iss->Variables, "InformationPreference", System::Management::Automation::ActionPreference::Continue);
    
    // todo the below is not memory safe! need to fix how Logger is stored
    auto logger = new Logger(BaseLogString);
    auto holder = gcnew RunspaceHolder(logger);
    auto host = gcnew MyHost(holder);
    Runspace^ runspace = RunspaceFactory::CreateRunspace(host,iss);
    holder->runspace = runspace;
	runspace->Open();
	return HandleTable::InsertRunspace(holder);
}


void DeleteRunspace(RunspaceHandle handle)
{
    auto runspaceHolder = HandleTable::RemoveRunspace(handle);
	runspaceHolder->runspace->Close();
    delete runspaceHolder->logger;
}

