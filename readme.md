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
Due to needing to return from powershell arbitrary managed objects as well as exceptions. I thought how best to approach this. What I have done is just stock converted things to JSON. In the return layer. This means you need a json parser in the hosting language.

I was unsure how to handle exceptions and I decided it was best to convert those to JSON as well. This gives you access to the type, the stack trace, the powershell stack trace, and any other fields you may know of.

Lastly you can initialize all the logging functions for powershell. This means not only do you get the returned objects, but you get rich leveled logging inside powershell.

From hosting layer to powershell it is all strings. The thought process is this string could be whatever you want and you can just convert it in powershell. I will give some examples.

