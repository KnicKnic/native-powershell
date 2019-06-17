
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdio.h>
#include <msclr\marshal_cppstd.h>


#include "host.h"
using namespace System::Management::Automation::Runspaces;
using namespace System::Management::Automation;

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
private:

	static long long runspaceIndex = 0;
};


long startpowershell(RunspaceHandle handle, StringPtr str) {
    auto managedStr = msclr::interop::marshal_as<System::String^>(str);
    auto v = PowerShell::Create();
	v->Runspace = HandleTable::GetRunspace(handle);
    v->AddCommand("mkdir")->AddArgument(managedStr);
    v->Invoke();
    return 0;

}

RunspaceHandle CreateRunspace()
{
	Runspace^ runspace = RunspaceFactory::CreateRunspace();
	runspace->Open();
	return HandleTable::InsertRunspace(runspace);
}


void DeleteRunspace(RunspaceHandle handle)
{
	HandleTable::RemoveRunspace(handle)->Close();
}

