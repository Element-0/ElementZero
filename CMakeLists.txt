cmake_minimum_required (VERSION 3.8)

project ("WineBDLauncher")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include (ExternalProject)
include (EZToolchain)
include (Deps)

generate_git_version (git_version)

add_subdirectory ("Deps")
add_subdirectory ("ModLoader")
add_subdirectory ("Base")
add_subdirectory ("BuiltinMods")
add_subdirectory ("CustomMods")
