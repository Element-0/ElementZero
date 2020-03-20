set_project "ElementZero"
set_languages "c++"

add_repositories "repo deps"

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

on_install (function (target)
  local res = path.join(target:installdir(), target:values("prefix")) .. path._SEP
  local libpath = path.join(target:installdir(), "Lib") .. path._SEP
  print ("install " .. target:targetfile() .. " to " .. res)
  local dll = target:targetfile()
  local lib = path.join(path.directory(dll), path.basename(dll)) .. ".lib"
  local pdb = path.join(path.directory(dll), path.basename(dll)) .. ".pdb"
  os.cp (dll, res)
  os.trycp (lib, libpath)
  os.cp (pdb, libpath)
end)

before_install (function (target)
  local pkgs = target:info().packages
  local deppath = path.join(target:installdir(), "Deps") .. path._SEP
  for _, name in ipairs(pkgs) do
    for _, dir in ipairs(target:pkg(name):get("linkdirs")) do 
      for _, dll in ipairs(os.files(path.join(dir, "*.dll"))) do
        if not target:values("prefix") then
          print ("Install global dep " .. path.filename(dll))
          os.cp (dll, target:installdir())
        else
          if depdll == nil then
            depdll = {}
          end
          if not depdll[dll] then
            print ("Install dep " .. path.filename(dll))
            os.cp (dll, deppath)
            depdll[dll] = true
          end
        end
      end
    end
  end
end)

on_load (function (target)
  target:add ("defines", target:name() .. "_EXPORTS")
end)

add_requires ("funchook", {configs = {vs_runtime = "MD"}})
add_requires ("boost", {configs = {vs_runtime = "MD", filesystem = false}})
add_requires ("sqlite3 3.31.0100", {configs = {vs_runtime = "MD"}})
add_requires ("sqlite_cpp", {configs = {vs_runtime = "MD"}})
add_requires ("yaml-cpp", {configs = {vs_runtime = "MD"}})
add_requires ("jsoncpp", {configs = {vs_runtime = "MD"}})
add_requires ("chakra_core", {configs = {vs_runtime = "MD"}})
add_requires ("msgpack", {configs = {vs_runtime = "MD"}})

includes "ModLoader"
includes "Base"
includes "BuiltinMods"