cmake_minimum_required (VERSION 3.8)

add_library (Base SHARED dllmain.cpp)
target_link_libraries (Base Boost YAML ModLoader)
install (TARGETS Base RUNTIME DESTINATION .)