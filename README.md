# WineBDLauncher
![status](https://img.shields.io/badge/status-WIP-red?style=for-the-badge)
![platform](https://img.shields.io/badge/platform-win--x64%20%7C%20wine--linux--x64-green?style=for-the-badge)
<a href="https://t.me/wine_bdlauncher">![Telegram](https://img.shields.io/badge/telegram-wine_bdlauncher-%232CA5E0?style=for-the-badge&logo=Telegram)</a><br>

Run windows version of BDS in linux with Mod support

(Yes, it is also can be run in windows)

## Build and install

0. Download bedrock dedicated server binary
1. Generate bedrock_server_mod.lib (see below)
2. Build it with Visual Studio 2019 (or use MSBuild manually)
3. Copy Dist/*.dll to BDS folder (only dll files)
4. Download ChakraCore.dll to BDS folder (see below)
5. Run bedrock_server_mod.exe

## How to get bedrock_server_mod.lib

0. Download and install [EatPdb](https://github.com/CodeHz/EatPdb)
1. Run `eatpdb -i bedrock_server.exe -o bedrock_server_mod.exe`
2. Open Visual Studio Developer Console, then run `lib /def:bedrock_server_mod.def /machine:x64`
3. Copy bedrock_server_mod.lib to $(SolutionDir)Lib\bedrock_server_mod.lib

## How to get ChakraCore.dll

0. Download windows build from https://github.com/microsoft/ChakraCore/releases
1. Extract x64 variant to BDS folder (only dll)

## LICENSE

GPLv3