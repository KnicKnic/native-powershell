
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>


#include "host.h"
long startpowershell() {
    auto v = System::Management::Automation::PowerShell::Create();
    v->AddCommand("mkdir")->AddArgument("c:\\fuzzy");
    v->Invoke();
    return 0;

}

