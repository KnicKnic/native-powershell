[![Build Status](https://dev.azure.com/oneeyedelf1/powershell.native/_apis/build/status/KnicKnic.native-powershell?branchName=master)](https://dev.azure.com/oneeyedelf1/powershell.native/_build/latest?definitionId=2&branchName=master)
# This repository is under heavy development come back in 2 or three weeks!
You could watch the repository in the mean time. I will create a release when I am done

# Goal
Create a C Dll that can be used to execute powershell scripts and powershell commands programmatically from other languages and get the results. 

This is not a one way interaction like occurs via process exec. Callbacks are supported (powershell asynchronosly calling back into C) and you can keep context open / loaded across multiple invocations.

I wrote this to create a golang binding. Since everyone has C bindings, you can follow that to create your own language binding.

## Non Goals - Simple Managed hosting
If you want to host managed DLL's you could directly create a managed host and do it that way. This offers a very simple alternative. Write some wrappers in powershell or directly in the hosting language via powershell statements and away you go.

# How
## Problem
Due to needing to return from powershell arbitrary managed objects as well as exceptions. I thought how best to approach this. One thought was to use JSON, which would represent the data which you wanted to consume. You can still do this, however I don't directly give you the way to do it yourself. 

## Inception
What I give is a way to take the output from one invocation of powershell to the input of another. I also provide the ability to get these output value types, and call .ToString() on them. You might think that this is not enough power and you need more. **But you have all the power you need in the powershell provided before you**. Simply execute a powershell query on the previous results to format the data how you want to consume it.

## Exceptions
I treat exceptions the same way I treat objects from above, you can get their type and string value. If you want to get the stack trace, execute a powershell query to get the stacktrace field of the exception and return it as a string.

## Logging
Lastly you can initialize all the logging functions for powershell. This means not only do you get the returned objects, but you get rich leveled logging inside powershell.

## Host to Powershell
From hosting layer to powershell it is all strings and previously returned objects. The thought process is this string could be whatever you want and you can just convert it in powershell. I will give some examples.

