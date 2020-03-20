function mod ()
  target (path.filename(os.curdir()))
    set_values ("prefix", "Mods")
    set_kind ("shared")
    add_files ("*.cpp")
    add_deps ("ModLoader", "Base")
    add_links ("bedrock_server_mod")
    add_packages ("boost", "sqlite3", "sqlite_cpp", "yaml-cpp", "jsoncpp")
end

for _, filepath in ipairs(os.files("*/xmake.lua")) do
  local mod = path.filename(path.directory(filepath))
  includes (mod)
end