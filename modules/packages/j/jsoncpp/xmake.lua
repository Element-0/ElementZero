package ("jsoncpp")
  set_homepage ("https://github.com/open-source-parsers/jsoncpp")
  set_description ("A C++ library for interacting with JSON.")

  set_urls ("https://github.com/open-source-parsers/jsoncpp.git")

  add_deps ("cmake")
  add_includedirs ("include")
  add_links ("jsoncpp")

  on_install (function (package)
    import("package.tools.cmake").install (
      package,
      {
        "-DBUILD_SHARED_LIBS=ON",
        "-DJSONCPP_WITH_TESTS=OFF",
        "-DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF",
        "-DJSONCPP_WITH_PKGCONFIG_SUPPORT=OFF",
        "-DJSONCPP_WITH_CMAKE_PACKAGE=OFF"
      }
    )
    os.cp("**.lib", package:installdir("lib"))
    os.cp("**.dll", package:installdir("lib"))
    os.cp("**.exp", package:installdir("lib"))
  end)
package_end ()