set_project "ElementZero"
set_languages "c++"

add_cxxflags "-DUNICODE"
add_cxxflags "-D_UNICODE"
add_cxxflags "-MD"
add_cxxflags "-EHsc"

set_languages "c++20"

add_linkdirs "Lib"
add_includedirs "Public"
add_includedirs "MinecraftHeaders"
set_installdir "$(projectdir)/Dist"
set_symbols "debug"
set_kind "shared"

includes "rules.lua"
add_rules "export_name"
add_rules "default_install"

task ("auto")
  on_run (function () 
    os.exec "xmake"
    os.exec "xmake install"
  end)
  set_menu {
    usage = "xmake auto",
    description = "build and install",
    options = {}
  }
task_end ()

includes "deps.lua"

includes "ModLoader"
includes "Base"
includes "BuiltinMods"