
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdio.h>
#include <msclr\marshal_cppstd.h>


#include "host.h"
using namespace System::Management::Automation::Runspaces;
using namespace System::Management::Automation;


using namespace System;
using namespace System::Security;
using namespace System::Collections::Generic;
using namespace System::Globalization;
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;
#include "my_host.hpp"
#include "utils/memory_helpers.hpp"
#include "utils/managed_memory_helpers.hpp"
#include "runspace.hpp"
#include "powershell.hpp"
#include "handle_table.hpp"

#include "logger.hpp"

// Globals
NativePowerShell_FreePointer NativePowerShell_FreePointerPtr = nullptr;
NativePowerShell_AllocPointer NativePowerShell_AllocPointerPtr = nullptr;

const NativePowerShell_PowerShellObject EmptyNativePowerShell_PowerShellObjectHandle = (NativePowerShell_PowerShellObject)(0);


void NativePowerShell_InitLibrary(NativePowerShell_AllocPointer allocPtr, NativePowerShell_FreePointer freePtr) {
    NativePowerShell_AllocPointerPtr = allocPtr;
    NativePowerShell_FreePointerPtr = freePtr;
}


long NativePowerShell_AddCommandSpecifyScope(NativePowerShell_PowerShellHandle handle, NativePowerShell_StringPtr command,char useLocalScope)
{
    auto managedCommand = msclr::interop::marshal_as<System::String^>(command);
    auto powershell = HandleTable::GetPowershell(handle)->powershell;
    powershell->AddCommand(managedCommand, useLocalScope !=0);
    return 0;
}
long NativePowerShell_AddCommand(NativePowerShell_PowerShellHandle handle, NativePowerShell_StringPtr command)
{
    return NativePowerShell_AddCommandSpecifyScope(handle, command, char(1));
}
long NativePowerShell_AddArgument(NativePowerShell_PowerShellHandle handle, NativePowerShell_StringPtr argument)
{
	auto managedArgument = msclr::interop::marshal_as<System::String^>(argument);
	auto powershell = HandleTable::GetPowershell(handle)->powershell;
	powershell->AddArgument(managedArgument);
	return 0;
}

long NativePowerShell_AddParameterString(NativePowerShell_PowerShellHandle handle, NativePowerShell_StringPtr name, NativePowerShell_StringPtr value) {

    auto paramName = msclr::interop::marshal_as<System::String^>(name);
    auto paramValue = msclr::interop::marshal_as<System::String^>(value);
    auto powershell = HandleTable::GetPowershell(handle)->powershell;
    powershell->AddParameter(paramName, paramValue);
    return 0;
}


long NativePowerShell_AddParameterObject(NativePowerShell_PowerShellHandle handle, NativePowerShell_StringPtr name, NativePowerShell_PowerShellObject object) {

    auto paramName = msclr::interop::marshal_as<System::String^>(name);
    auto powershell = HandleTable::GetPowershell(handle)->powershell;
    if (object == EmptyNativePowerShell_PowerShellObjectHandle) {
        powershell->AddParameter(paramName);
    }
    else {
        PSObject^ psObject = HandleTable::GetPSObject(object);
        if (psObject == nullptr)
        {
            powershell->AddParameter(paramName, nullptr);
        }
        else {
            powershell->AddParameter(paramName, psObject->BaseObject);
        }
    }
    return 0;
}

long NativePowerShell_AddPSObjectArgument(NativePowerShell_PowerShellHandle handle, NativePowerShell_PowerShellObject object)
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
long NativePowerShell_AddPSObjectArguments(NativePowerShell_PowerShellHandle handle, NativePowerShell_PowerShellObject* objects, unsigned int count)
{
	for (unsigned int i = 0; i < count; ++i) {
        NativePowerShell_AddPSObjectArgument(handle, objects[i]);
	}
	return 0;
}

long NativePowerShell_AddScript(NativePowerShell_PowerShellHandle handle, NativePowerShell_StringPtr path) {
    return NativePowerShell_AddScriptSpecifyScope(handle, path, char(1));
}
long NativePowerShell_AddScriptSpecifyScope(NativePowerShell_PowerShellHandle handle, NativePowerShell_StringPtr path, char useLocalScope)
{
    auto managedPath = msclr::interop::marshal_as<System::String^>(path);
    auto powershellHolder = HandleTable::GetPowershell(handle);
    auto powershell = powershellHolder->powershell;
    powershell->AddScript(managedPath, useLocalScope!=0);
    return 0;
}
NativePowerShell_PowerShellObject NativePowerShell_InvokeCommand(NativePowerShell_PowerShellHandle handle, NativePowerShell_PowerShellObject **objects, unsigned int * objectCount )
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
    *objects = (NativePowerShell_PowerShellObject*) NativePowerShell_AllocPointerPtr(results->Count * sizeof(NativePowerShell_PowerShellObject));
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
    return EmptyNativePowerShell_PowerShellObjectHandle;
}

void NativePowerShell_ClosePowerShellObject(NativePowerShell_PowerShellObject psobject) {
    if (psobject != EmptyNativePowerShell_PowerShellObjectHandle) {
        HandleTable::RemovePSObject(psobject);
    }
}

NativePowerShell_PowerShellHandle NativePowerShell_CreatePowerShell(NativePowerShell_RunspaceHandle handle)
{
    auto runspaceHolder = HandleTable::GetRunspace(handle);
	auto powershell = PowerShell::Create();
    auto powershellHolder = gcnew PowerShellHolder(runspaceHolder, powershell);
	powershell->Runspace = runspaceHolder->runspace;
	return HandleTable::InsertPowershell(powershellHolder);
}
NativePowerShell_PowerShellHandle NativePowerShell_CreatePowerShellNested(NativePowerShell_PowerShellHandle handle)
{   
    auto parentPowershell = HandleTable::GetPowershell(handle);
    auto powershell = parentPowershell->powershell->CreateNestedPowerShell();
    auto powershellHolder = gcnew PowerShellHolder(parentPowershell->runspace, powershell);
    powershell->Runspace = parentPowershell->runspace->runspace;
    return HandleTable::InsertPowershell(powershellHolder);
}



void NativePowerShell_DeletePowershell(NativePowerShell_PowerShellHandle handle)
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
            //Console::WriteLine("got $null for input");
        }
        else {
            //Console::WriteLine("got non null for input");
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

            std::vector<NativePowerShell_PowerShellObject> inputObjectsC;
            inputObjectsC.reserve(inputObjects->Count);
            for each (auto obj in inputObjects)
            {
                inputObjectsC.push_back(HandleTable::InsertPSObject(obj));
            }

            std::wstring commandInput = msclr::interop::marshal_as<std::wstring>(message);

            NativePowerShell_JsonReturnValues returnValues;
            returnValues.count = 0;
            returnValues.objects = nullptr;
            sendJsonCommand(host->runspace->context, commandInput.c_str(),inputObjectsC.data(), inputObjectsC.size(), &returnValues);
            auto freeObjectList = MakeAutoDllFree(returnValues.objects);
            for (unsigned long i = 0; i < returnValues.count; ++i) {
                auto& object = returnValues.objects[i];
                switch (object.type) {
                case NativePowerShell_PowerShellObjectTypeString:
                {
                    auto outputManaged = msclr::interop::marshal_as<System::String^>(object.instance.string);
                    if (object.releaseObject != char(0))
                    {
                        auto freeString = MakeAutoDllFree(object.instance.string);
                    }
                    WriteObject(outputManaged, false);
                    break;
                }
                case NativePowerShell_PowerShellObjectHandle:
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

WSManConnectionInfo^ MakeConnectionInfo(const wchar_t* computerName, const wchar_t* username, const wchar_t* password) {

    auto managedComputerName = msclr::interop::marshal_as<System::String^>(computerName);
    auto uri = gcnew Uri("http://" + managedComputerName + ":5985/wsman");
    //auto uri = gcnew Uri("https://" + managedComputerName + ":5986/wsman");
    //auto uri = gcnew Uri("https://" + managedComputerName + "/OcsPowershell");
    WSManConnectionInfo^ connection;

    /*if (username != nullptr) {
        auto managedUsername = msclr::interop::marshal_as<System::String^>(username);
        auto managedPassword = msclr::interop::marshal_as<System::String^>(password);

        SecureString^ securePass = gcnew SecureString();
        for each (auto x in managedPassword)
        {
            securePass->AppendChar(x);
        }
        PSCredential^ managedCredentials = gcnew PSCredential(managedUsername, securePass);

        connection = gcnew WSManConnectionInfo(uri, "http://schemas.microsoft.com/powershell/Microsoft.PowerShell", managedCredentials);
    }
    else {
        connection = gcnew WSManConnectionInfo(uri);
    }*/
    //connection->ComputerName = managedComputerName;

    if (username != nullptr) {
        auto managedUsername = msclr::interop::marshal_as<System::String^>(username);
        auto managedPassword = msclr::interop::marshal_as<System::String^>(password);

        SecureString^ securePass = gcnew SecureString();
        for each(auto x in managedPassword)
        {
            securePass->AppendChar(x);
        }
        PSCredential^ managedCredentials = gcnew PSCredential(managedUsername, securePass);
        
        connection = gcnew WSManConnectionInfo(
            false,
            managedComputerName,
            5985,
            msclr::interop::marshal_as<System::String^>(L"wsman"),
            msclr::interop::marshal_as<System::String^>(L"http://schemas.microsoft.com/powershell/Microsoft.PowerShell"),
            managedCredentials);
    }
    else {
        connection = gcnew WSManConnectionInfo(uri);
    }
    //connection->AuthenticationMechanism = AuthenticationMechanism::Negotiate;

    // Set the OperationTimeout property and OpenTimeout properties.
    // The OperationTimeout property is used to tell Windows PowerShell
    // how long to wait (in milliseconds) before timing out for an
    // operation. The OpenTimeout property is used to tell Windows
    // PowerShell how long to wait (in milliseconds) before timing out
    // while establishing a remote connection.
    connection->OperationTimeout = 4 * 60 * 1000; // 4 minutes.
    connection->OpenTimeout = 1 * 60 * 1000; // 1 minute.

    connection->SkipCACheck = true;
    connection->SkipCNCheck = true;
    connection->SkipRevocationCheck = true;

    return connection;

}

NativePowerShell_RunspaceHandle CreateRunspaceInternal(void* context, NativePowerShell_ReceiveJsonCommand receiveJsonCommand, NativePowerShell_LogString BaseLogString, const wchar_t* computerName, const wchar_t* username, const wchar_t* password)
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
    Runspace^ runspace;
    if (computerName == nullptr) {
        runspace = RunspaceFactory::CreateRunspace(host, iss);
    }
    else {
        auto connectionInfo = MakeConnectionInfo(computerName, username, password);

        runspace = RunspaceFactory::CreateRunspace(connectionInfo, host, gcnew  TypeTable(gcnew array< String^ >(0)));
        //runspace = RunspaceFactory::CreateRunspace(connectionInfo);
    }
    holder->host = host;
    holder->runspace = runspace;
    runspace->Open();
    return HandleTable::InsertRunspace(holder);
}


NativePowerShell_RunspaceHandle NativePowerShell_CreateRunspace(void * context, NativePowerShell_ReceiveJsonCommand receiveJsonCommand, NativePowerShell_LogString BaseLogString)
{
    return CreateRunspaceInternal(context, receiveJsonCommand, BaseLogString, nullptr, nullptr, nullptr);
}

NativePowerShell_RunspaceHandle NativePowerShell_CreateRemoteRunspace(void* context, NativePowerShell_LogString BaseLogString, const wchar_t* computerName, const wchar_t* username, const wchar_t* password)
{
    return CreateRunspaceInternal(context, nullptr, BaseLogString, computerName, username, password);
}


void NativePowerShell_DeleteRunspace(NativePowerShell_RunspaceHandle handle)
{
    MakeUsing(HandleTable::RemoveRunspace(handle))->runspace->Close();
}

LPCWSTR MakeHostString(System::String^ str) {
    std::wstring cppStr = msclr::interop::marshal_as<std::wstring>(str);
    LPWSTR cStr = (LPWSTR)NativePowerShell_AllocPointerPtr((cppStr.length() + 1) * 2);
    std::copy(cppStr.c_str(), cppStr.c_str() + (cppStr.length() + 1),cStr);
    return cStr;
}

NativePowerShell_StringPtr NativePowerShell_GetPSObjectType(NativePowerShell_PowerShellObject handle) {
    auto psObject = HandleTable::GetPSObject(handle);
    return MakeHostString(psObject->BaseObject->GetType()->ToString());
}
NativePowerShell_StringPtr NativePowerShell_GetPSObjectToString(NativePowerShell_PowerShellObject handle) {
    auto psObject = HandleTable::GetPSObject(handle);
    return MakeHostString(psObject->BaseObject->ToString());
}
char NativePowerShell_IsPSObjectNullptr(NativePowerShell_PowerShellObject handle) {
    if (handle == EmptyNativePowerShell_PowerShellObjectHandle) {
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

NativePowerShell_PowerShellObject NativePowerShell_AddPSObjectHandle(NativePowerShell_PowerShellObject handle) {
    return HandleTable::InsertPSObject(HandleTable::GetPSObject(handle));
}

