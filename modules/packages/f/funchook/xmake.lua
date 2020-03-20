package ("funchook")
  set_homepage ("https://github.com/kubo/funchook")
  set_description ("Funchook - an API Hook Library")

  set_urls ("https://github.com/kubo/funchook.git")

  add_deps ("cmake")
  add_includedirs ("include")

  add_links ("funchook_dll")

  on_install (function (package)
    import("package.tools.cmake").install (
      package,
      {"-DFUNCHOOK_BUILD_STATIC=OFF", "-DFUNCHOOK_BUILD_TESTS=OFF"}
    )
    os.cp("**.lib", package:installdir("lib"))
    os.cp("**.dll", package:installdir("lib"))
    os.cp("**.exp", package:installdir("lib"))
  end)
package_end ()