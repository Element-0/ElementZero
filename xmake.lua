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
        if depdll == nil then
          depdll = {}
        end
        if not target:values("prefix") then
          print ("Install global dep " .. path.filename(dll))
          os.cp (dll, target:installdir())
          depdll[dll] = true
        else
          if not depdll[dll] then
            print ("Install " .. name .. " dep " .. path.filename(dll))
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

includes "deps.lua"

includes "ModLoader"
includes "Base"
includes "BuiltinMods"