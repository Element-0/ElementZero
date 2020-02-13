# WineBDLauncher
<a href="https://github.com/codehz/wine-bdlauncher/actions">![status](https://img.shields.io/github/workflow/status/codehz/wine-bdlauncher/CI?style=for-the-badge)</a>
![platform](https://img.shields.io/badge/platform-win--x64%20%7C%20wine--linux--x64-green?style=for-the-badge)
<a href="https://t.me/wine_bdlauncher">![Telegram](https://img.shields.io/badge/telegram-wine_bdlauncher-%232CA5E0?style=for-the-badge&logo=Telegram)</a><br>

<a href="https://github.com/codehz/wine-bdlauncher/releases/latest/download/pkg.tar.xz">![GitHub Releases (by Asset)](https://img.shields.io/github/downloads/codehz/wine-bdlauncher/latest/pkg.tar.xz?style=for-the-badge)</a><br>

Run windows version of BDS in linux with Mod support

(Yes, it is also can be run in windows)



## FAQ

Q: Why not use a native linux version?<br>
A: The windows version has noticeable performance improvements, even when running under wine.

Q: Can the Windows version run directly under wine?<br>
A: No, because Microsoft uses Chakra.dll, which contains a lot of references to private dlls. (It also prevents you from running on systems prior to Windows 10.)

Q: How it works?<br>
A: I created a dll that simply forwards all APIs to the open source version of Chakra engine i.e. ChakraCore. And this version can run directly under wine.

## Build and install

0. Download bedrock dedicated server binary
1. Generate bedrock_server_mod.lib (see below)
2. Build it with Visual Studio 2019 (or use MSBuild manually)
3. Copy Dist/* to bds folder (or use [symlink](https://blogs.windows.com/windowsdeveloper/2016/12/02/symlinks-windows-10/))
4. Run bedrock_server_mod.exe

## How to get bedrock_server_mod.lib

0. Download and install [EatPdb](https://github.com/CodeHz/EatPdb)
1. Copy $(SolutionDir)\eatpdb.yaml to bds folder
2. Run `eatpdb exec eatpdb.yaml`
3. Open `x64 Native Tools Command Prompt for VS 2019`, then run `lib /def:bedrock_server_mod.def /machine:x64`
4. Copy bedrock_server_mod.lib to $(SolutionDir)Lib\bedrock_server_mod.lib

## LICENSE

GPLv3