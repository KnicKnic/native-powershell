write-host $debugPreference, $verbosepreference,$informationPreference, $erroractionpreference, $warningpreference

# $global:DebugPreference = "Continue"
# $global:VerbosePreference = "Continue"
# $global:InformationPreference = "Continue"
# $global:ErrorActionPreference = "Continue"
# $global:WarningPreference = "Continue"
# write-host $debugPreference, $verbosepreference,$informationPreference, $erroractionpreference, $warningpreference

function ThrowIt($message){
	throw $message
}
get-proc | write-host
write-error "first error"

Get-Variable * | out-string | write-host
# # mkdir C:\Windows\ -ErrorAction Stop
mkdir C:\Windows\

# #cmd /C "dir c:\"
# $b = dir c:\
# #$b | format-table|Out-String|write-host

# $f = cmd /C "dir c:\"
# #write-host $f
# $a = "asdf"
# write-information "info"
write-error "errorasdf"
# write-host "one"
# write-debug "two"
# write-verbose "three"
# write-host "four"


# ThrowIt "fuzzy"