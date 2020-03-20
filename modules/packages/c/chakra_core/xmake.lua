
package ("chakra_core")
  set_homepage ("https://github.com/microsoft/ChakraCore")
  set_description ("ChakraCore is the core part of the Chakra JavaScript engine that powers Microsoft Edge")

  set_urls ("https://github.com/microsoft/ChakraCore.git")

  add_includedirs ("include")
  add_links ("ChakraCore")

  on_install ("windows", function (package)
    os.vrun("msbuild Build/Chakra.Core.sln -nologo -t:Rebuild -p:Configuration=%s -p:Platform=%s", package:debug() and "Debug" or "Release", is_arch("x64") and "x64" or "Win32")
    os.cp("lib/jsrt/*.h", package:installdir("include"))
    local binpath = "Build/VcBuild/bin/" .. (is_arch("x64") and "x64" or "x86") .. "_" .. (package:debug() and "debug" or "release")
    os.cp(path.join(binpath, "ChakraCore.dll"), package:installdir("lib"))
    os.cp(path.join(binpath, "ChakraCore.lib"), package:installdir("lib"))
    os.cp(path.join(binpath, "ChakraCore.pdb"), package:installdir("lib"))
  end)
package_end ()