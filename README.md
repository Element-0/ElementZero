# Element 0
<a href="https://github.com/codehz/ElementZero/actions">![status](https://img.shields.io/github/workflow/status/codehz/ElementZero/CI?style=for-the-badge)</a>
![platform](https://img.shields.io/badge/platform-win--x64%20%7C%20wine--linux--x64-green?style=for-the-badge)
<a href="https://t.me/ezloader">![Telegram](https://img.shields.io/badge/telegram-ezloader-%232CA5E0?style=for-the-badge&logo=Telegram)</a><br>

<a href="https://github.com/codehz/ElementZero/releases/latest">![Latest Tag](https://img.shields.io/github/v/tag/codehz/ElementZero?label=LATEST%20TAG&style=for-the-badge)![GitHub Releases (by Asset)](https://img.shields.io/github/downloads/codehz/ElementZero/latest/pkg.tar.xz?style=for-the-badge)</a><br>


Run windows version of BDS in linux with Mod support (Yes, it is also can be run in windows)

## FAQ

Q: Why not use a native linux version?<br>
A: The windows version has noticeable performance improvements, even when running under wine.

Q: Can the Windows version run directly under wine?<br>
A: No, because Microsoft uses Chakra.dll, which contains a lot of references to private dlls. (It also prevents you from running on systems prior to Windows 10.)

Q: How it works?<br>
A: I created a dll that simply forwards all APIs to the open source version of Chakra engine i.e. ChakraCore. And this version can run directly under wine.

## Features

0. Compatiable with official BDS
1. And support behavior pack with custom item/recipes/scripts (* vanilla server *does* support them, but disabled in code)
2. Can be run in windows and linux (with wine)
3. Native modding support
4. /transferserver is back!

## Build and install

0. Download bedrock dedicated server binary
1. Generate bedrock_server_mod.lib (see below)
2. Install vcpkg and install dependencies (see below)
3. Build it with Visual Studio 2019 (or use CMake manually see .githubn/workflows/ci.yaml)
4. Copy Dist/* to bds folder (or use [symlink](https://blogs.windows.com/windowsdeveloper/2016/12/02/symlinks-windows-10/))
5. Run bedrock_server_mod.exe

## Visual Studio 2019 workloads and components

0. Microsoft.VisualStudio.Workload.NativeDesktop
1. Microsoft.VisualStudio.Workload.ManagedDesktop
2. Microsoft.VisualStudio.Component.VC.Tools.x86.x64
3. Microsoft.VisualStudio.Component.Windows10SDK.18362
4. Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Llvm.Clang

## How to use vcpkg

0. Download and install [vcpkg](https://github.com/microsoft/vcpkg)
1. Install dependencies by `vcpkg install @vcpkg.txt` (run in $(SolutionDir))

## How to get bedrock_server_mod.lib

0. Download and install [EatPdb](https://github.com/CodeHz/EatPdb)
1. Copy $(SolutionDir)\eatpdb.yaml to bds folder
2. Run `eatpdb exec eatpdb.yaml`
3. Open `x64 Native Tools Command Prompt for VS 2019`, then run `lib /def:bedrock_server_mod.def /machine:x64`
4. Copy bedrock_server_mod.lib to $(SolutionDir)Lib\bedrock_server_mod.lib

## LICENSE

GPLv3
