cmake_minimum_required (VERSION 3.8)

add_base_mod (Base dllmain.cpp settings.hpp)
mod_link (Base SQLiteCpp sqlite3)