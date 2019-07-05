
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

const PowerShellObject EmptyPowerShellObjectHandle = (PowerShellObject)(0);


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
    return AddCommandSpecifyScope(handle, command, char(1));
}
long AddArgument(PowershellHandle handle, StringPtr argument)
{
	auto managedArgument = msclr::interop::marshal_as<System::String^>(argument);
	auto powershell = HandleTable::GetPowershell(handle)->powershell;
	powershell->AddArgument(managedArgument);
	return 0;
}

long AddParameterString(PowershellHandle handle, StringPtr name, StringPtr value) {

    auto paramName = msclr::interop::marshal_as<System::String^>(name);
    auto paramValue = msclr::interop::marshal_as<System::String^>(value);
    auto powershell = HandleTable::GetPowershell(handle)->powershell;
    powershell->AddParameter(paramName, paramValue);
    return 0;
}

long AddPSObjectArgument(PowershellHandle handle, PowerShellObject object)
{
	PSObject^ psObject = HandleTable::GetPSObject(object);
	auto powershell = HandleTable::GetPowershell(handle)->powershell;
    if (psObject == nullptr)
    {
        powershell->AddArgument(psObject);
    }
    else {
        powershell->AddArgument(psObject->BaseObject);
    }
	return 0;
}
long AddPSObjectArguments(PowershellHandle handle, PowerShellObject* objects, unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i) {
		AddPSObjectArgument(handle, objects[i]);
	}
	return 0;
}

long AddScript(PowershellHandle handle, StringPtr path) {
    return AddScriptSpecifyScope(handle, path, char(1));
}
long AddScriptSpecifyScope(PowershellHandle handle, StringPtr path, char useLocalScope) 
{
    auto managedPath = msclr::interop::marshal_as<System::String^>(path);
    auto powershellHolder = HandleTable::GetPowershell(handle);
    auto powershell = powershellHolder->powershell;
    powershell->AddScript(managedPath, useLocalScope!=0);
    return 0;
}
PowerShellObject InvokeCommand(PowershellHandle handle, PowerShellObject **objects, unsigned int * objectCount )
{
	auto powershellHolder = HandleTable::GetPowershell(handle);
    auto powershell = powershellHolder->powershell;
    auto runspaceHolder = powershellHolder->runspace;
    auto Logger = powershellHolder->runspace->logger;
    *objectCount = 0;
    *objects = nullptr;
    System::Collections::ObjectModel::Collection<System::Management::Automation::PSObject^>^ results;
    try {
        results = powershell->Invoke();
    }
    //catch (System::Management::Automation::RuntimeException^ exception) {
    //    Logger->LogLineError("Caught Exception of type " + exception->GetType()->ToString());
    //    Logger->LogLineError(exception->ToString());
    //    if (exception->ErrorRecord) {
    //        Logger->LogLineError("Powershell stack trace");
    //        Logger->LogLineError(exception->ErrorRecord->ScriptStackTrace);
    //    }
    //}
    catch (System::Object^ exception) {
        return HandleTable::InsertPSObject(gcnew PSObject(exception));
    }
    *objectCount = (unsigned int)results->Count;
    *objects = (PowerShellObject*) AllocPointerPtr(results->Count * sizeof(PowerShellObject));
    long long i = 0;
    for each (auto object in results) {
        (*objects)[i] = HandleTable::InsertPSObject(object);
        ++i;
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
    return EmptyPowerShellObjectHandle;
}

void ClosePowerShellObject(PowerShellObject psobject) {
    if (psobject != EmptyPowerShellObjectHandle) {
        HandleTable::RemovePSObject(psobject);
    }
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
/// Class that implements the Send-HostCommand.
/// </summary>
[Cmdlet(VerbsCommunications::Send, "HostCommand")]
public ref class SendHostCommand : Cmdlet
{
public:
    [Parameter(Position = 0, Mandatory = true)]
    property System::String ^ message;

    [Parameter( Mandatory = false, ValueFromPipeline=true)]
    property array<System::Object^>^ input;
private:
    List<PSObject^>^ inputObjects = gcnew List<PSObject^>();
protected:
    void ProcessRecord()override
    {
        Cmdlet::ProcessRecord();
        //Console::WriteLine("In Process, count: {0}, firstValue {1}, first type {2}", input->Length, input[0]->ToString(), input[0]->GetType());
        
        //if $null is passed, then input is null. We will assume it is always 1 object(I think it is)
        if (input == nullptr) {
            inputObjects->Add(MakePSObject(nullptr));
        }
        else {
            for each (auto obj in input) {
                inputObjects->Add(MakePSObject(obj));
            }
        }
    }

    PSObject^ MakePSObject(System::Object^ obj) {
        if (obj == nullptr) {
            //return gcnew PSObject(nullptr);
            return nullptr;
        }
        if (obj->GetType() == PSObject::typeid) {
            return safe_cast<PSObject^>(obj);
        }
        return gcnew PSObject(obj);
    }

    void EndProcessing()override
    {
        Cmdlet::EndProcessing();

        MyHost^ host = safe_cast<MyHost^>(this->CommandRuntime->Host->PrivateData->BaseObject);
        auto sendJsonCommand = host->runspace->sendJsonCommand;
        if (sendJsonCommand != nullptr)
        {
            // TODO: figure out how to differentiate between below
            //
            // by default if no input specified we get 1 nullptr object input
            // to clean this up, we delete all single list that is nullptr
            //
            // one problem with this approach is we cannot differentiate between
            // `$null | send-hostcommand -message "foo"`
            // `send-hostcommand -message "foo"
            if (inputObjects->Count == 1 && inputObjects[0] == nullptr) {
                inputObjects = gcnew List<PSObject^>();
            }

            std::vector<PowerShellObject> inputObjectsC;
            inputObjectsC.reserve(inputObjects->Count);
            for each (auto obj in inputObjects)
            {
                inputObjectsC.push_back(HandleTable::InsertPSObject(obj));
            }

            std::wstring commandInput = msclr::interop::marshal_as<std::wstring>(message);

            JsonReturnValues returnValues;
            returnValues.count = 0;
            returnValues.objects = nullptr;
            sendJsonCommand(host->runspace->context, commandInput.c_str(),inputObjectsC.data(), inputObjectsC.size(), &returnValues);
            auto freeObjectList = MakeAutoDllFree(returnValues.objects);
            for (unsigned long i = 0; i < returnValues.count; ++i) {
                auto& object = returnValues.objects[i];
                switch (object.type) {
                case PowershellObjectTypeString:
                {
                    auto outputManaged = msclr::interop::marshal_as<System::String^>(object.instance.string);
                    if (object.releaseObject != char(0))
                    {
                        auto freeString = MakeAutoDllFree(object.instance.string);
                    }
                    WriteObject(outputManaged, false);
                    break;
                }
                case PowershellObjectHandle:
                {
                    auto psObject = HandleTable::GetPSObject(object.instance.psObject);
                    if (psObject != nullptr && psObject->BaseObject != nullptr)
                    {
                        WriteObject(psObject, false);
                    }
                    else {
                        WriteObject(nullptr, false);
                    }
                    if (object.releaseObject != char(0))
                    {
                        HandleTable::RemovePSObject(object.instance.psObject);
                    }
                    break;
                }
                default:
                    throw "should not hit default case";
                    
                }
            }
            for (auto& obj : inputObjectsC) {
                HandleTable::RemovePSObject(obj);
            }
        }
    }

};





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
RunspaceHandle CreateRunspace(void * context, ReceiveJsonCommand receiveJsonCommand, LogString BaseLogString)
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

    
    auto logger = gcnew Logger(context, BaseLogString);
    auto holder = gcnew RunspaceHolder(context, receiveJsonCommand, logger);
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

LPCWSTR MakeHostString(System::String^ str) {
    std::wstring cppStr = msclr::interop::marshal_as<std::wstring>(str);
    LPWSTR cStr = (LPWSTR)AllocPointerPtr((cppStr.length() + 1) * 2);
    std::copy(cppStr.c_str(), cppStr.c_str() + (cppStr.length() + 1),cStr);
    return cStr;
}

StringPtr GetPSObjectType(PowerShellObject handle) {
    auto psObject = HandleTable::GetPSObject(handle);
    return MakeHostString(psObject->BaseObject->GetType()->ToString());
}
StringPtr GetPSObjectToString(PowerShellObject handle) {
    auto psObject = HandleTable::GetPSObject(handle);
    return MakeHostString(psObject->BaseObject->ToString());
}
char IsPSObjectNullptr(PowerShellObject handle) {
    if (handle == EmptyPowerShellObjectHandle) {
        return char(1);
    }
    PSObject^ psObject = HandleTable::GetPSObject(handle);
    if (psObject == nullptr) {
        return char(1);
    }
    if (psObject->BaseObject == nullptr) {
        return char(1);
    }
    return char(0);
}

PowerShellObject AddPSObjectHandle(PowerShellObject handle) {
    return HandleTable::InsertPSObject(HandleTable::GetPSObject(handle));
}

