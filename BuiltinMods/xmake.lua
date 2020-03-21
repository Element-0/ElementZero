function mod (name)
  target (name or path.filename(os.curdir()))
    add_files ("*.cpp")
end

function mod_end () 
  target_end ()
end

set_values ("prefix", "Mods")
set_kind ("shared")
add_deps ("ModLoader", "Base")
add_links ("bedrock_server_mod")
add_packages ("boost", "sqlite3", "sqlite_cpp", "yaml-cpp", "jsoncpp")

for _, filepath in ipairs(os.files("*/xmake.lua")) do
  local mod = path.filename(path.directory(filepath))
  includes (mod)
end