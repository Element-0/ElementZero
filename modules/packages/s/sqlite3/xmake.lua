package("sqlite3")

  set_homepage("https://sqlite.org/")
  set_description("The most used database engine in the world")

  set_urls("https://sqlite.org/2020/sqlite-autoconf-$(version).tar.gz",
          {version = function (version) return version:gsub("%.", "") end})

  add_versions("3.31.0100", "62284efebc05a76f909c580ffa5c008a7d22a1287285d68b7825a2b6b51949ae")

  on_install("windows", function (package)
    os.vrun("nmake DEBUG=%s -f Makefile.msc DYNAMIC_SHELL=1 PLATFORM=%s USE_CRT_DLL=%s", package:debug() and "1" or "0", package:arch(), package:config("vs_runtime") == "MD" and "1" or "0")
    os.cp("*.h", package:installdir("include"))
    os.cp("sqlite3.lib", package:installdir("lib"))
    os.cp("sqlite3.pdb", package:installdir("lib"))
    os.cp("sqlite3.dll", package:installdir("lib"))
  end)

  on_install("macosx", "linux", function (package)
    import("package.tools.autoconf").install(package, {package:debug() and "--enable-debug" or ""})
  end)

  on_test(function (package)
    assert(package:has_cfuncs("sqlite3_open_v2", {includes = "sqlite3.h"}))
  end)