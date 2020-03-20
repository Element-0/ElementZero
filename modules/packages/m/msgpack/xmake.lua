package ("msgpack")
  set_homepage ("https://github.com/msgpack/msgpack-c")
  set_description ("MessagePack implementation for C and C++ / msgpack.org[C/C++]")

  set_urls ("https://github.com/msgpack/msgpack-c.git")

  add_deps ("cmake")
  add_includedirs ("include")

  on_install (function (package)
    import("package.tools.cmake").install (
      package,
      {"-DMSGPACK_BUILD_TESTS=OFF","-DMSGPACK_BUILD_EXAMPLES=OFF"}
    )
  end)
package_end ()