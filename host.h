#pragma once
#ifdef __cplusplus
extern "C" {
#endif

    typedef void (*FreePointer)(void*);
    typedef unsigned char* (*AllocPointer)(unsigned long long size);
    typedef const wchar_t * (*ReceiveJsonCommand)(const wchar_t*);
    typedef void (*LogString)(const wchar_t*);

    void InitLibrary( AllocPointer, FreePointer);

	typedef struct RunspaceHandle_ {} *RunspaceHandle;
	typedef struct PowershellHandle_ {} *PowershellHandle;
	//typedef RunspaceHandle_d * RunspaceHandle;



    typedef const wchar_t* StringPtr;

	PowershellHandle CreatePowershell(RunspaceHandle handle);


	void DeletePowershell(PowershellHandle handle);


	RunspaceHandle CreateRunspace(ReceiveJsonCommand, LogString );

	void DeleteRunspace(RunspaceHandle handle);


	long AddCommand(PowershellHandle handle, StringPtr command);
	long AddArgument(PowershellHandle handle, StringPtr argument);
	long InvokeCommand(PowershellHandle handle);
    long AddScript(PowershellHandle handle, StringPtr path, bool useLocalScope = false);

#ifdef __cplusplus
}
#endif