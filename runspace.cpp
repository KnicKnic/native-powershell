#include "runspace.hpp"


using namespace System::Management::Automation::Runspaces;
using namespace System::Management::Automation;


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Globalization;
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;


#include "my_host.hpp"

RunspaceHolder::~RunspaceHolder() {
    delete runspace;
    delete logger;
    delete host;
}