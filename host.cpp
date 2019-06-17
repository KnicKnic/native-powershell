
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <msclr\marshal_cppstd.h>


#include "host.h"
long startpowershell(StringPtr str) {
    auto managedStr = msclr::interop::marshal_as<System::String^>(str);
    auto v = System::Management::Automation::PowerShell::Create();
    v->AddCommand("mkdir")->AddArgument(managedStr);
    v->Invoke();
    return 0;

}

