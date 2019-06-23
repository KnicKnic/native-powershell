#include "runspace.h"


using namespace System::Management::Automation::Runspaces;
using namespace System::Management::Automation;


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Globalization;
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;


#include "my_host.h"

RunspaceHolder::~RunspaceHolder() {
    delete runspace;
    delete logger;
    delete host;
}