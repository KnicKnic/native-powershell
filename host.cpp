
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
#include "handle_table.h"

#include "logger.h"

// Globals
FreePointer FreePointerPtr = nullptr;
AllocPointer AllocPointerPtr = nullptr;




void InitLibrary(AllocPointer allocPtr, FreePointer freePtr) {
    AllocPointerPtr = allocPtr;
    FreePointerPtr = freePtr;
}


long AddCommandSpecifyScope(PowershellHandle handle, StringPtr command,char useLocalScope)
{
    auto managedCommand = msclr::interop::marshal_as<System::String^>(command);
    auto powershell = HandleTable::GetPowershell(handle)->powershell;
    powershell->AddCommand(managedCommand, useLocalScope !=0);
    return 0;
}
long AddCommand(PowershellHandle handle, StringPtr command)
{
    return AddCommandSpecifyScope(handle, command, (char)1);
}
long AddArgument(PowershellHandle handle, StringPtr argument)
{
    auto managedArgument = msclr::interop::marshal_as<System::String^>(argument);
    auto powershell = HandleTable::GetPowershell(handle)->powershell;
    powershell->AddArgument(managedArgument);
    return 0;
}
long AddScript(PowershellHandle handle, StringPtr path) {
    return AddScriptSpecifyScope(handle, path, (char)1);
}
long AddScriptSpecifyScope(PowershellHandle handle, StringPtr path, char useLocalScope) 
{
    auto managedPath = msclr::interop::marshal_as<System::String^>(path);
    auto powershellHolder = HandleTable::GetPowershell(handle);
    auto powershell = powershellHolder->powershell;
    powershell->AddScript(managedPath, useLocalScope!=0);
    return 0;
}
long InvokeCommand(PowershellHandle handle)
{
	auto powershellHolder = HandleTable::GetPowershell(handle);
    auto powershell = powershellHolder->powershell;
    auto Logger = powershellHolder->runspace->logger;
    try {
        //auto invocationSettings = gcnew PSInvocationSettings();
        //invocationSettings->ErrorActionPreference = System::Management::Automation::ActionPreference::Continue;
        //invocationSettings->Host = powershellHolder->runspace->host;
        //auto results = powershell->Invoke(nullptr, invocationSettings);
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
    MakeUsing(HandleTable::RemovePowershell(handle))->powershell->Stop();
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

        std::wstring commandInput = msclr::interop::marshal_as<std::wstring>(message);
        auto output = MakeAutoDllFree( host->runspace->sendJsonCommand(commandInput.c_str()));
        //const wchar_t * output = host->runspace->sendJsonCommand(commandInput.c_str());
        //std::unique_ptr<const wchar_t, FreePointerHelper> outputReleaser(output);
        if (output != nullptr) {
            auto outputManaged = msclr::interop::marshal_as<System::String^>(output.get());
            WriteObject(outputManaged, false);
        }
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
RunspaceHandle CreateRunspace(ReceiveJsonCommand receiveJsonCommand, LogString BaseLogString)
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

    
    auto logger = gcnew Logger(BaseLogString);
    auto holder = gcnew RunspaceHolder(receiveJsonCommand, logger);
    auto host = gcnew MyHost(holder);
    Runspace^ runspace = RunspaceFactory::CreateRunspace(host,iss);
    holder->host = host;
    holder->runspace = runspace;
	runspace->Open();
	return HandleTable::InsertRunspace(holder);
}


void DeleteRunspace(RunspaceHandle handle)
{
    MakeUsing(HandleTable::RemoveRunspace(handle))->runspace->Close();
}

