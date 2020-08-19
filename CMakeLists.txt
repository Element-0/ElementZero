cmake_minimum_required (VERSION 3.8)

project ("ElementZero")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include (ExternalProject)
include (EZToolchain)
include (Deps)

include_directories ("Deps/SQLiteCpp/sqlite3")
generate_git_version (git_version)

add_subdirectory ("Deps")
add_subdirectory ("ModLoader")
add_subdirectory ("Base")
add_subdirectory ("BuiltinMods")
add_subdirectory ("CustomMods")

install (FILES Lib/addition_symbols.db DESTINATION .)
install (FILES Lib/bedrock_server_mod.lib DESTINATION Lib)
install (FILES Lib/bedrock_server_mod.exe DESTINATION .)

set (CPACK_PACKAGE_VERSION ${git_version})
set (CPACK_PACKAGE_DIRECTORY ${PROJECT_SOURCE_DIR}/pack)
set (CPACK_PACKAGE_CHECKSUM SHA256)
include (CPack)