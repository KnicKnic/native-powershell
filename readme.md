[![Build Status](https://dev.azure.com/oneeyedelf1/powershell.native/_apis/build/status/KnicKnic.native-powershell?branchName=master)](https://dev.azure.com/oneeyedelf1/powershell.native/_build/latest?definitionId=2&branchName=master)
[![GitHub commits since latest release (branch)](https://img.shields.io/github/commits-since/KnicKnic/native-powershell/latest.svg)](https://github.com/KnicKnic/native-powershell/releases/latest)

# Goal
Create a C Dll that can be used to execute powershell scripts and powershell commands programmatically from other languages and get the results. 

This is not a one way interaction like occurs via process exec. Callbacks are supported (powershell asynchronosly calling back into C) and you can keep context open / loaded across multiple invocations.

I wrote this to create a golang binding (https://github.com/KnicKnic/go-powershell). Since everyone has C bindings, you can follow that to create your own language binding.


## Status
This project works and is used by the [go powershell binding](https://github.com/KnicKnic/go-powershell). However I have not gotten around to fully documenting it.

Someone should add a sample project, but until then I suggest you look at the test code. This code should get refactored to seperate the supporting code from the rest. [test_unit\test_unit.cpp](test_unit\test_unit.cpp) start with `TEST_CASE("test logger")`

## Non Goals - Simple Managed hosting
If you want to host managed DLL's you could directly create a managed host and do it that way. This offers a very simple alternative. Write some wrappers in powershell or directly in the hosting language via powershell statements and away you go.

# How
## Problem
Due to needing to return from powershell arbitrary managed objects as well as exceptions. I thought how best to approach this. One thought was to use JSON, which would represent the data which you wanted to consume. You can still do this, however I don't directly give you the way to do it yourself. 

## Inception
What I give is a way to take the output from one invocation of powershell to the input of another. I also provide the ability to call 3 funcions of these, `GetType`, `ToString`, `==nullptr`. You might think that this is not enough power and you need more. **But you have all the power you need in the powershell provided before you**. Simply execute a powershell query on the previous results to format the data how you want to consume it.

## Exceptions
I treat exceptions the same way I treat objects from above, you can get their type and string value. If you want to get the stack trace, execute a powershell query to get the stacktrace field of the exception and return it as a string.

## Logging
Lastly you can initialize all the logging functions for powershell. This means not only do you get the returned objects, but you get rich leveled logging inside powershell.

## Host to Powershell
From hosting layer to powershell it is all strings and previously returned objects. The thought process is this string could be whatever you want and you can just convert it in powershell. I will give some examples.

## Code overview

1. host.cpp
    1. this contains the logic to execute powershell sessions and is the bulk of the project
1. host.h
    1. the actual exported functions & types of this dll.
1. my_host.cpp
    1. this contains a custom PSHost that allows us to implement the callbacks for logging.
        1. The only purpose we went through this implementation is to get logging callbacks.
1. test_unit/test_unit.cpp
    1. This contains a few unit tests.

## running the unit tests

1. open the solution
1. Ensure that the test_unit project is set to startup project
1. hit f5

# Docs

Generic info about writting a hosting app
https://docs.microsoft.com/en-us/powershell/developer/windows-powershell

writting a custom host
https://docs.microsoft.com/en-us/powershell/developer/hosting/host03-sample

injecting a cmdlet into your host
https://docs.microsoft.com/en-us/powershell/developer/hosting/runspace10-sample

shows how to change default session state parameters (this is used to change error preferences to log details to host ($DebugPreference = Continue))
https://docs.microsoft.com/en-us/powershell/developer/hosting/windows-powershell01-sample