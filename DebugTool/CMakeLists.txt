cmake_minimum_required (VERSION 3.8)

add_library(DebugTool SHARED dllmain.cpp)
target_link_libraries (DebugTool Boost YAML CommandSupport ModLoader MinecraftHeaders)
install (TARGETS DebugTool RUNTIME DESTINATION Mods)