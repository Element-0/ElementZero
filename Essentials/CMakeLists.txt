cmake_minimum_required (VERSION 3.8)

add_library(Essentials SHARED dllmain.cpp)
target_link_libraries (Essentials Boost YAML CommandSupport ModLoader MinecraftHeaders)
install (TARGETS Essentials RUNTIME DESTINATION Mods)