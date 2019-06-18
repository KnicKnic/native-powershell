#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	typedef struct RunspaceHandle_ {} *RunspaceHandle;
	typedef struct PowershellHandle_ {} *PowershellHandle;
	//typedef RunspaceHandle_d * RunspaceHandle;



    typedef const wchar_t* StringPtr;

	long MakeDir(PowershellHandle powershell, StringPtr path);
	PowershellHandle CreatePowershell(RunspaceHandle handle);


	void DeletePowershell(PowershellHandle handle);


	RunspaceHandle CreateRunspace();

	void DeleteRunspace(RunspaceHandle handle);


	long AddCommand(PowershellHandle handle, StringPtr command);
	long AddArgument(PowershellHandle handle, StringPtr argument);
	long InvokeCommand(PowershellHandle handle);

#ifdef __cplusplus
}
#endif