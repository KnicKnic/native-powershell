#pragma once
#ifdef __cplusplus
extern "C" {
#endif

	typedef struct RunspaceHandle_ {} *RunspaceHandle;
	//typedef RunspaceHandle_d * RunspaceHandle;



    typedef const wchar_t* StringPtr;


	RunspaceHandle CreateRunspace();

	void DeleteRunspace(RunspaceHandle handle);

	long startpowershell(RunspaceHandle handle, StringPtr str);
#ifdef __cplusplus
}
#endif