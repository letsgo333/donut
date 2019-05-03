---
layout: post
title: Donut - Injecting .NET Assemblies as Shellcode
---

# Introduction

![_config.yml]({{ site.baseurl }}/images/Introducing_Donut/donut.png)

```Shortly before publishing donut, Odzhan and I became aware of another team working on a shellcode generator for .NET Assemblies. They were at the same stage of their project at us. We both agreed that whomever of us who published first would ensure that the other received due credit for their work. As soon as they publish their tool, we will update this article with a link.```

Donut is a shellcode generation tool that creates x86 or x64 shellcode payloads from .NET Assemblies. This shellcode may be used to inject the Assembly into arbitrary Windows processes. Given an arbitrary .NET Assembly, parameters, and an entry point (such as Program.Main), it produces position-independent shellcode that loads it from memory. The .NET Assembly can either be staged from a URL or stageless by being embedded directly in the shellcode. Either way, the .NET Assembly is encrypted with the Chaskey block cipher and a 128-bit randomly generated key. After the Assembly is loaded through the CLR, the original reference is erased from memory to deter memory scanners. The Assembly is loaded into a new Application Domain to allow for running Assemblies in disposable AppDomains.

# How it Works

## .NET Primer

Before we begin, you must understand a few important components of .NET:

* (https://docs.microsoft.com/en-us/dotnet/standard/clr "Common Language Runtime"): Like Java, .NET uses a runtime environment (or "virtual machine") to interpret code at runtime. All .NET Code is compiled from an intermediate language to native code "Just-In-Time" before execution.
* (https://docs.microsoft.com/en-us/dotnet/standard/managed-code "Common Intermediate Language"): Speaking of an intermediate language, .NET uses CIL (also known as MSIL). All .NET languages (of which there are many) are "compiled" to this intermediate language. CIL is a generic object-oriented assembly language that can be interpreted into machine code for any hardware architecture. As such, the designers of .NET languages do not need to design their compilers around the architectures they will run on. Instead, they merely need to design it to compile to one language: CIL.
* (https://docs.microsoft.com/en-us/dotnet/framework/app-domains/assemblies-in-the-common-language-runtime ".NET Assemblies"): .NET applications are packaged into .NET Assemblies. They are so called because the code from your language of choice has been "assembled" into CIL but not truly compiled. Assemblies use an extension of the PE format and are represented as either an EXE or a DLL that contains CIL rather than native machine code.
* (https://docs.microsoft.com/en-us/dotnet/framework/app-domains/application-domains "Application Domains"): Assemblies are run inside of a safe "box" known as an Application Domain. Multiple Assemblies can exist within an AppDomain, and multiple AppDomains can exist within a process. AppDomains are intended to provide the same level of isolation between executing Assemblies as is normally provided for processes. Threads may move between AppDomains and can share objects through marshalling and delegates.

## Unmanaged Hosting API

Microsoft provides an API known as the (https://docs.microsoft.com/en-us/dotnet/framework/unmanaged-api/hosting/ "Unmanaged CLR Hosting API"). This API allows for unmanaged code (such as C or C++) to host, inspect, configure, and use Common Language Runtimes. It is a legitimate API that can be used for many purposes. Microsoft uses it for several of their products, and other companies use it to design custom loaders for their programs. It can be used to improve performance of .NET applications, create sandboxes, or just do wierd stuff. We do the latter.

One of the things it can do is manually load .NET Assemblies into arbitrary (https://docs.microsoft.com/en-us/dotnet/framework/app-domains/application-domains "Application Domains"). It can do this either from disk or from memory. We utilize its capability for loading from memory to load your payload without touching disk.

To see a standalone example of an Unmanaged CLR Hosting Assembly loader, check out Casey Smith's repo: https://github.com/caseysmithrc/AssemblyLoader 

## CLR Injection

The first action that donut's shellcode takes is to load the CLR. By default it will load the v4.0.30319 version of the CLR, which supports the versions 4.0+ of .NET. If the user specified otherwise, it will load v2.0.50727 of the CLR, which supports versions 3.5 and earlier. Once the CLR is loaded, the shellcode creates a new Application Domain. At this point, the .NET Assembly payload must be obtained. If the user provided a staging URL, then the Assembly is downloaded from it. Otherwise, it is obtained from memory. Either way, it will be decrypted and then loaded into the new AppDomain. After the Assembly is loaded but before it is run, the decrypted copy will be released and freed from memory with VirtualFree to deter memory scanners. Finally, the Entry Point specified by the user will be invoked along with any provided parameters.

If the CLR is already loaded into the host process, then donut's shellcode will still work. The .NET Assembly will just be loaded into a new Application Domain within the managed process. .NET is designed to allow for .NET Assemblies built for multiple versions of .NET to run simultaneously in the same process. As such, your payload should always run no matter the process's state before injection.

## Shellcode Generation

# The Current Challenge 

Currently, .NET tradecraft is limited to execution by one of two main ways:

* Assembly.Load():
* execute-assembly:

## Assembly.Load

Only lets you inject into current process. Must be done from within .NET.

## execute-assembly

Executes the same way every time. Relies on reflective DLL injection, which is often detected and 

# Using Donut

```  
usage: donut [options] -f <.NET assembly> | -u <URL hosting donut module>

       -f <path>            .NET assembly to embed in PIC and DLL.
       -u <URL>             HTTP server hosting the .NET assembly.
       -c <namespace.class> The assembly class name.
       -m <method>          The assembly method name.
       -p <arg1,arg2...>    Optional parameters for method, separated by comma or semi-colon.
       -a <arch>            Target architecture : 1=x86, 2=amd64(default).
       -r <version>         CLR runtime version. v4.0.30319 is used by default.
       -d <name>            AppDomain name to create for assembly. Randomly generated by default.

 examples:

    donut -a 1 -c TestClass -m RunProcess -p notepad.exe -f loader.dll
    donut -f loader.dll -c TestClass -m RunProcess -p notepad.exe -u http://remote_server.com/modules/
```

## Generating Shellcode

![_config.yml]({{ site.baseurl }}/images/Introducing_Donut/ST_generate_and_copy.png)

![_config.yml]({{ site.baseurl }}/images/Introducing_Donut/ST_generate_and_copy_86.png)

## Choosing a Host Process

![_config.yml]({{ site.baseurl }}/images/Introducing_Donut/ProcessManager.jpg)

## Injecting

You may use any standard shellcode injection technique to inject the .NET Assembly. 

![_config.yml]({{ site.baseurl }}/images/Introducing_Donut/ST_inject.png)

![_config.yml]({{ site.baseurl }}/images/Introducing_Donut/ST_success.png)

## Integrating into Tooling

# Advancing Tradecraft

It is our hope that releasing donut to the public will advance offensive and red team tradecraft in several ways:

* Provide red teams and adversary emulators with a means to emulate this technique that threat actors may have developed in secret.
* Provide blue teams a frame of refernce for detecting and mitigating CLR Injection techniques.
* Inspire tool developers to develop new types of techniques and tradecraft.

## Alternative Payloads

The main benefit of using .NET Assemblies as shellcode is that they can now be executed by anything that can execute shellcode on Windows. There are many more ways to inject shellcode than there are to load Assemblies. As such, offensive tool designers no longer need to design their payloads around running .NET. Instead, they may leverage their existing payloads and techniques that use shellcode.

## Injecting .NET At Will / Migration

Donut will also allow the developers of C2 Frameworks / RATs to add migrate-like functionality to their tools. By using Donut as a library (or calling the generator) on the server and then providing the result to an existing agent, it may inject a new instance of itself into another running process. This may also be used to inject arbitrary post-exploitation modules so long as I/O is properly redirected.

## Disposable AppDomains

When donut loads an Assembly, it loads it into a new AppDomain. Unless the user specifies the name of the AppDomain with the '-d' parameter, the AppDomain is given a random name. We specifically designed donut to run payloads in new AppDomains rather than using DefaultDomain. If this does not suit you, you can easily modify payload.c to use the default domain. By running the payload in its own AppDomain, this allows for the development of tools that run post-exploitation modules in disposable AppDomains. Application Domains can be unloaded, but individual Assemblies cannot. Therefore, to unload an Assembly when you are done with it, you must put it into its own AppDomain and unload that instead. A C# agent can have the shellcode generated on its server, inject the result into itself in a new thread, wait for the Assembly to finish executing, then unload the host AppDomain.

## Detecting CLR Injection

One of the companion projects for donut is ModuleMonitor. It uses WMI Event Win32_ModuleLoadTrace to monitor for module loading. It provides filters, detailed data, and has an option to monitor for CLR Injection attacks.

The CLR Sentry option follows some simple logic: If a process loads the CLR, but the program is not a .NET program, then the CLR has been injected into it.

While useful, there are both false positives and false negatives:

* False Postiive: There are (few) legitimate uses of the Unmanaged CLR Hosting API. If there weren't, then Microsoft wouldn't have made it. CLR Sentry will notice every unmanaged program that loads the CLR.  
* False Negatives: This will NOT notice injection of .NET code into processes that already have the CLR loaded. So, no use of the Reflection API and not when donut is used to inject shellcode into managed processes.

Please Note: This is intended only as a Proof-of-Concept to demonstrate the anomalous behavior produced by CLR injection and how it may be detected. It should not be used in any way in a production environment.

I am not a defender, but the following pseudocode is my attempt at an analytic that follows this logic.

```
void CLR_Injection:
    WHEN Image_Load event:
        if event.Module.Name contains "msco*.dll":
            if !(IsValidAssembly(event.Process.FilePath)):
            {
                print "A CLR has been injected into " + event.Process.Id
            }
```

The snippet below represents my implementation of this logic in C#. The full code can be found in ModuleMonitor.

```csharp
//CLR Sentry
//Author: TheWover
 while (true)
        {
            //Get the module load.
            Win32_ModuleLoadTrace trace = GetNextModuleLoad();

            //Split the file path into parts delimited by a '\'
            string[] parts = trace.FileName.Split('\\');

            //Check whether it is a .NET Runtime DLL
            if (parts[parts.Length - 1].Contains("msco"))
            {
                //Get a 
                Process proc = Process.GetProcessById((int) trace.ProcessID);

                //Check if the file is a .NET Assembly
                if (!IsValidAssembly(proc.StartInfo.FileName))
                {
                    //If it is not, then the CLR has been injected.
                    Console.WriteLine();

                    Console.WriteLine("[!] CLR Injection has been detected!");

                    //Display information from the event
                    Console.WriteLine("[>] Process {0} has loaded the CLR but is not a .NET Assembly:", trace.ProcessID);
                }
            }
        }
```

![_config.yml]({{ site.baseurl }}/images/Introducing_Donut/detected.png)

It is important to note that this behaviour represents all CLR Injection techniques, of which there are several. This detection should work for donut, as well as other tools such as Cobalt Strike's 'execute-assembly' command.

## OpSec Considerations

ModuleMonitor demonstrates an important point about CLR Injection: When performed against unmanaged processes, CLR Injection produces highly anomalous process behavior. The loading of a CLR after a process's initial execution or from unmanaged code is unusual. There are few legitimate use cases. From a defender's perspective, this allows you to build an analytics that monitor for the behavior described in the section above. 

However, as I mentioned, this analytic fails to detect CLR Injection into processes that already have the CLR loaded. As such, an operator could evade the analytic by simply injecting into processes that are already managed. I would recommend the following standard operating procedure:

1. Run ProcessManager from memory to enumerate processes. Take note of which you can inject into.
2. If there are any processes that are already managed, then consider them the set of potential targets.
3. If there are not any managed processes, then consider them the set of potential targets.
4. Either way, inject / migrate into the process that is most likely to produce network traffic and live the longest.

# Conclusion