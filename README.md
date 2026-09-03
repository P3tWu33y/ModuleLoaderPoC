# ModuleLoaderPoC

> A small Windows reverse-engineering proof of concept demonstrating DLL proxying through a commonly used system library.

**Status:** Archived / Proof of Concept
**Language:** C++
**Architecture:** x86 / 32-bit
**Platform:** Windows
**Focus:** DLL Proxying • Windows Internals • Reverse Engineering

---

## Overview

**ModuleLoaderPoC** is a small proof of concept exploring a classic Windows DLL proxying technique.

The idea is simple:

1. Identify a DLL that the target application expects to load.
2. Create a replacement DLL with the same filename.
3. Place the replacement DLL in a location where the target application will resolve it.
4. Allow the proxy DLL to execute custom code while preserving the expected functionality of the original dependency.

This PoC was originally developed while experimenting with older **32-bit game clients**, particularly MU Online.

The project was intentionally kept small. Its purpose was to demonstrate the underlying loading mechanism rather than provide a complete framework.

---

## DLL Proxying

Windows applications frequently depend on DLLs for functionality provided by the operating system or third-party libraries.

If an application imports a DLL by name, the Windows loader performs DLL name/path resolution according to its loading rules.

This can make DLL proxying possible in situations where an application expects a particular library to exist but does not ship a private copy of that library alongside the executable.

In this PoC, the proxy is named:

```text
Version.dll
```

The resulting DLL can be placed alongside a compatible 32-bit target executable.

When the target attempts to resolve the dependency, the proxy DLL can be loaded into the target process.

---

## Concept

The simplified loading flow looks like this:

```text
                Target Application
                       │
                       │ imports Version.dll
                       ▼
                Windows Loader
                       │
                       ▼
              Local Version.dll
                       │
                       ├──── Execute proxy code
                       │
                       └──── Forward / provide
                            expected functionality
```

The important concept is that the application does not need to explicitly call `LoadLibrary()` on the proxy DLL.

The DLL is loaded as a consequence of the application's normal dependency resolution.

---

## Why `Version.dll`?

A number of older Windows applications and games use APIs provided by `Version.dll`.

Older 32-bit game clients can therefore provide interesting targets for studying DLL loading and proxying behavior.

The PoC was originally tested with this approach against **MU Online** clients.

The technique is not inherently specific to MU Online, however. Its applicability depends on the target application's dependencies, architecture, DLL-loading behavior, and Windows loader behavior.

---

## Architecture

The project targets **32-bit applications**.

```text
Target
  │
  ├── x86 executable
  │
  └── imports Version.dll
           │
           ▼
      ModuleLoaderPoC
           │
           ▼
       Proxy DLL
```

Architecture compatibility is important:

> An x64 process cannot load an x86 DLL, and an x86 process cannot load an x64 DLL.

Therefore, the PoC must be built for the same architecture as the target process.

---

## What This Project Demonstrates

Although the implementation is intentionally small, the project demonstrates several useful Windows internals concepts:

* Dynamic-link library loading
* DLL dependency resolution
* DLL search-order behavior
* DLL proxying
* x86 Windows development
* Native C++
* PE/DLL concepts
* Process initialization
* Reverse-engineering methodology
* Working with legacy Windows applications

---

## Reverse-Engineering Context

This project came from practical experimentation with older game clients.

The general investigation process was:

```text
Target Application
       │
       ▼
Inspect imported DLLs
       │
       ▼
Identify candidate dependency
       │
       ▼
Understand DLL resolution
       │
       ▼
Build compatible proxy DLL
       │
       ▼
Place proxy beside target
       │
       ▼
Observe process behavior
       │
       ▼
Execute custom code in-process
```

This makes the project a useful example of how knowledge obtained through reverse engineering can be turned into a small native Windows PoC.

---

## Project Structure

```text
ModuleLoaderPoC/
│
├── example/
│   └── ...
│
└── README.md
```

The `example` directory contains the proof-of-concept implementation.

---

## Limitations

This repository is deliberately minimal and should not be considered a production-ready DLL proxying framework.

Important limitations include:

* Designed around 32-bit targets
* Target behavior depends on the application's DLL dependencies
* DLL resolution behavior can vary between Windows versions and configurations
* The PoC does not attempt to provide a universal proxying mechanism
* Some applications may explicitly load DLLs using absolute paths
* Applications using different architectures require a matching proxy

---

## Project Status

**Archived.**

Development was discontinued after the proof of concept demonstrated the intended technique.

Rather than removing the project, I am keeping it public as a small example of earlier reverse-engineering work and experimentation with Windows internals.

---

## Why This Repository Exists

This is a deliberately small project.

The value of the repository is less about the amount of code and more about the concept it demonstrates:

> Understanding how a Windows executable resolves its dependencies, identifying an opportunity to influence that loading process, and implementing the idea as a native C++ proof of concept.

It represents hands-on experimentation with:

**Reverse Engineering → PE/DLL Analysis → Windows Loader Behavior → Native C++**

---

## Disclaimer

This project is provided for educational and security research purposes.

It is intended to demonstrate Windows DLL loading and proxying concepts in controlled environments.

The author is not responsible for the use of this project against software or systems without authorization.
