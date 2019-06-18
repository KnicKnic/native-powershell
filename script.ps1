$DebugPreference = "Continue"
$VerbosePreference = "Continue"
$InformationPreference = "Continue"

function ThrowIt($message){
	throw $message
}
cmd /C "dir c:\"
$b = dir c:\
$b | format-table|Out-String|write-host

$f = cmd /C "dir c:\"
write-host $f
$a = "asdf"
write-information "info"
write-host "one"
write-debug "two"
write-verbose "three"
write-host "four"


# ThrowIt "fuzzy"