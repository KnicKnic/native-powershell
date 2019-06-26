
using namespace System::Management::Automation;
using namespace System::Management::Automation::Host;

$psCommand = [System.Management.Automation.PowerShell]::Create([System.Management.Automation.RunspaceMode]::CurrentRunspace)
$null = $psCommand.AddScript(".\t3.ps1", $true)
$psCommand.Invoke()
