package ("sqlite_cpp")
  set_homepage ("https://github.com/SRombauts/SQLiteCpp")
  set_description ("SQLiteC++ (SQLiteCpp) is a smart and easy to use C++ SQLite3 wrapper.")

  set_urls ("https://github.com/SRombauts/SQLiteCpp.git")

  add_deps ("cmake", "sqlite3")
  add_links ("SQLiteCpp")
  add_includedirs ("include")

  on_install (function (package)
    import("package.tools.cmake").install (
      package,
      {"-DSQLITECPP_RUN_CPPLINT=OFF", "-DSQLITECPP_RUN_CPPCHECK=OFF", "-DSQLITECPP_INTERNAL_SQLITE=OFF"}
    )
    os.cp("**.lib", package:installdir("lib"))
  end)
package_end ()