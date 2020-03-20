package ("yaml-cpp")
  set_homepage ("https://github.com/jbeder/yaml-cpp")
  set_description ("A YAML parser and emitter in C++")

  set_urls ("https://github.com/jbeder/yaml-cpp.git")

  add_deps ("cmake")
  add_includedirs ("include")

  on_install (function (package)
    import("package.tools.cmake").install (
      package,
      {"-DBUILD_SHARED_LIBS=ON", "-DYAML_CPP_BUILD_TESTS=OFF", "-DYAML_MSVC_SHARED_RT=ON"}
    )
    os.cp("**.lib", package:installdir("lib"))
    os.cp("**.dll", package:installdir("lib"))
    os.cp("**.exp", package:installdir("lib"))
  end)
package_end ()