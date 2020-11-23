<h1 align="center">
<img src="https://ipfs.io/ipfs/QmYiuQE7WUeawAMTvQf9RUC7wWXb2b1wCFddVBpt6U7e1S/logo.png" width="256px" height="256px" />
</h1>
# ElementZero
<a href="https://github.com/codehz/ElementZero/actions">![status](https://img.shields.io/github/workflow/status/codehz/ElementZero/CI?style=for-the-badge)</a>
![platform](https://img.shields.io/badge/platform-win--x64%20%7C%20wine--linux--x64-green?style=for-the-badge)
<a href="https://t.me/ezloader">![Telegram](https://img.shields.io/badge/telegram-ezloader-%232CA5E0?style=for-the-badge&logo=Telegram)</a><br>

<a href="https://github.com/codehz/ElementZero/releases/latest">![Latest Tag](https://img.shields.io/github/v/tag/codehz/ElementZero?label=LATEST%20TAG&style=for-the-badge)![GitHub Releases (by Asset)](https://img.shields.io/github/downloads/codehz/ElementZero/latest/pkg.tar.xz?style=for-the-badge)</a><br>


Bedrock Dedicated Server mod with plugin support for Windows and Linux

## Getting Started

Check out our [Documentation](https://github.com/Element-0/ElementZero/wiki) for instructions on Installation, Configuration, and Customization.

## FAQ

Q: Why not use the native linux version?<br>
A: The windows version has noticeable performance improvements, even when running under wine.

Q: Can the Windows version run directly under wine?<br>
A: No, because Microsoft uses Chakra.dll, which contains a lot of references to private dlls. (It also prevents you from running on systems prior to Windows 10.)

Q: How does it work?<br>
A: I've created a dll that simply forwards all APIs to the open source version of Chakra engine, i.e. ChakraCore. And this version can run directly under wine.

## Features

1. Compatiable with official BDS
2. And support for behavior packs with custom item/recipes/scripts (vanilla server **does** support them, but it's disabled in code)
3. Can be run in windows and linux (with wine)
4. Native modding support
5. /transferserver is back!

## LICENSE

GPLv3
