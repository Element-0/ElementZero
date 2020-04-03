cmake_minimum_required (VERSION 3.8)

project ("WineBDLauncher")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

include (ExternalProject)
include (Utility)

generate_git_version (git_version)

option (NO_INLINE off)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS ON)
set (VCPKG_TARGET_TRIPLET "x64-windows-rel")

if (NO_INLINE)
    add_compile_options (/Ob0)
endif ()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" AND "x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC")
    add_compile_options (-Wno-c++2a-extensions)
endif ()
add_compile_options (/EHsc /utf-8)
add_definitions (-DWIN32_LEAN_AND_MEAN=1)

include_directories (Public)
add_compile_definitions (_UNICODE UNICODE NDEBUG)
add_compile_options (-Wno-invalid-offsetof -Wno-deprecated-declarations)

file (GLOB_RECURSE MinecraftHeadersFiles CONFIGURE_DEPENDS MinecraftHeaders/*)
add_custom_target (MinecraftHeaders SOURCES ${MinecraftHeadersFiles})
install (DIRECTORY MinecraftHeaders DESTINATION Include)

file (GLOB_RECURSE PublicHeadersFiles CONFIGURE_DEPENDS Public/*)
add_custom_target (PublicHeaders SOURCES ${PublicHeadersFiles})
install (DIRECTORY Public DESTINATION Include)

add_library (BedrockServer INTERFACE)
target_include_directories (BedrockServer INTERFACE MinecraftHeaders)
target_link_directories (BedrockServer INTERFACE Lib)
target_link_libraries (BedrockServer INTERFACE bedrock_server_mod)

find_package (SQLiteCpp CONFIG REQUIRED)
find_package (Boost 1.72.0 REQUIRED)
find_package (jsoncpp CONFIG REQUIRED)
find_and_install_package (sqlite3 CONFIG REQUIRED)
find_and_install_package (yaml-cpp CONFIG REQUIRED)
find_package (msgpack CONFIG REQUIRED)
find_package (Flatbuffers CONFIG REQUIRED)

function (def_mod name)
    cmake_parse_arguments(ARG "BASEMOD" "" "LINK;DELAY_LINK" ${ARGN})
    file (GLOB_RECURSE srcs
        CONFIGURE_DEPENDS *.cpp)
    set (IS_BASEMOD $<BOOL:${ARG_BASEMOD}>)
    add_library (${name} SHARED ${srcs})
    target_compile_definitions (${name}
        PRIVATE MODNAME=${name} $<${IS_BASEMOD}:EZVERSION=\"${git_version}\">)
    target_link_libraries (${name}
        Boost::headers
        yaml-cpp
        ModLoader
        BedrockServer
        $<$<NOT:${IS_BASEMOD}>:Base>
        ${ARG_LINK}
        ${ARG_DELAY_LINK})
    install (TARGETS ${name}
        RUNTIME DESTINATION Mods
        ARCHIVE DESTINATION Lib)
    install (FILES $<TARGET_PDB_FILE:${name}> DESTINATION Lib)
    set_target_properties (${name}
        PROPERTIES FOLDER $<IF:${IS_BASEMOD},Base,Mods>)
    if (ARG_DELAY_LINK)
        target_link_libraries (${name} delayimp)
        foreach (target ${ARGN})
            target_link_options (${name} PRIVATE /DELAYLOAD:${target}.dll)
        endforeach ()
    endif ()
endfunction ()


add_subdirectory ("Deps")
add_subdirectory ("ModLoader")
add_subdirectory ("Base")
add_subdirectory ("BuiltinMods")
