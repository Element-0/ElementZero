target "Base"
  set_kind  "shared"
  add_files "*.cpp"
  add_deps  "ModLoader"
  add_links "bedrock_server_mod"
  add_rules "extract_version"
  add_packages ("boost", "sqlite3", "sqlite_cpp", "yaml-cpp", "jsoncpp")