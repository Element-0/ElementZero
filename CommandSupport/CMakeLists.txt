cmake_minimum_required (VERSION 3.8)

add_library(CommandSupport SHARED dllmain.cpp)
target_link_libraries (CommandSupport Boost YAML ModLoader MinecraftHeaders)
install (TARGETS CommandSupport RUNTIME DESTINATION Mods)