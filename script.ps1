$DebugPreference = "Continue"
$VerbosePreference = "Continue"
$InformationPreference = "Continue"

function ThrowIt($message){
	throw $message
}

$a = "asdf"
write-information "info"
write-host "one"
write-debug "two"
write-verbose "three"
write-host "four"


ThrowIt "fuzzy"