#pragma once
#ifdef __cplusplus
extern "C" {
#endif
    
    #define InvalidPowershellHandleValue (void *)0;
    typedef void (*FreePointer)(void*);
    typedef unsigned char* (*AllocPointer)(unsigned long long size);
    typedef void (*LogString)(void * context, const wchar_t* messages);

    void InitLibrary( AllocPointer, FreePointer);

	typedef struct RunspaceHandle_ {} *RunspaceHandle;
    typedef struct PowershellHandle_ {} *PowershellHandle;
    typedef struct PowerShellObject_ {} *PowerShellObject;
	//typedef RunspaceHandle_d * RunspaceHandle;
       
    typedef const wchar_t* StringPtr;



    typedef enum PowershellObjectType_ { PowershellObjectTypeString, PowershellObjectHandle }PowershellObjectType;

    typedef struct GenericPowershellObject_ {
        PowershellObjectType type;
        union PowershellObjectInstance {
            StringPtr string;
            PowerShellObject psObject;
            // continue for other ones such as UInt64...
        } instance;
        char releaseObject; // if true reciever of this object will release the instance
    }GenericPowershellObject, * PGenericPowershellObject;


    typedef struct JsonReturnValues_ {
        GenericPowershellObject* objects;
        unsigned long           count;
    }JsonReturnValues,*PJsonReturnValues;
    typedef void (*ReceiveJsonCommand)(void* context, const wchar_t* command, PowerShellObject * inputs, unsigned long long inputCount, JsonReturnValues* returnValues);

	PowershellHandle CreatePowershell(RunspaceHandle handle);


	void DeletePowershell(PowershellHandle handle);


	RunspaceHandle CreateRunspace(void * context, ReceiveJsonCommand, LogString );

	void DeleteRunspace(RunspaceHandle handle);


    long AddCommand(PowershellHandle handle, StringPtr command);
    long AddCommandSpecifyScope(PowershellHandle handle, StringPtr command, char useLocalScope);
	long AddArgument(PowershellHandle handle, StringPtr argument);
	long AddPSObjectArgument(PowershellHandle handle, PowerShellObject object);
	long AddPSObjectArguments(PowershellHandle handle, PowerShellObject* objects, unsigned int count);

    // caller is responsible for calling ClosePowerShellObject on all returned objects, as well as
    // calling the appropriate free routine on objects assuming it is not nullptr
    PowerShellObject InvokeCommand(PowershellHandle handle, PowerShellObject** objects, unsigned int* objectCount);
    long AddScript(PowershellHandle handle, StringPtr path);
    long AddScriptSpecifyScope(PowershellHandle handle, StringPtr path, char useLocalScope);
    void ClosePowerShellObject(PowerShellObject psobject);

    StringPtr GetPSObjectType(PowerShellObject handle);
    StringPtr GetPSObjectToString(PowerShellObject handle);
    char IsPSObjectNullptr(PowerShellObject handle);
    PowerShellObject AddPSObjectHandle(PowerShellObject handle);

#ifdef __cplusplus
}
#endif