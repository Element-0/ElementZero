include (${CMAKE_CURRENT_LIST_DIR}/Utility.cmake)

if (EZ_CHAINLOAD_TOOLCHAIN_FILE)
    include ("${EZ_CHAINLOAD_TOOLCHAIN_FILE}")
endif ()

get_filename_component(EZ_ROOT ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)

option (NO_INLINE off)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS ON)
set (VCPKG_TARGET_TRIPLET "elementzero")
if (NO_INLINE)
    add_compile_options (/Ob0)
endif ()

add_compile_options (
    -Wno-c++2a-extensions
    -Wno-invalid-offsetof
    -Wno-deprecated-declarations
    -Wno-gnu-string-literal-operator-template)
add_compile_options (/EHsc /utf-8)
add_definitions (-DWIN32_LEAN_AND_MEAN=1 -DNOMINMAX -D_WIN32_WINNT=0x0A00)

include_directories (${EZ_ROOT}/Public)
add_compile_definitions (_UNICODE UNICODE NDEBUG)

file (GLOB_RECURSE MinecraftHeadersFiles CONFIGURE_DEPENDS ${EZ_ROOT}/MinecraftHeaders/*)
add_custom_target (MinecraftHeaders SOURCES ${MinecraftHeadersFiles})

file (GLOB_RECURSE PublicHeadersFiles CONFIGURE_DEPENDS ${EZ_ROOT}/Public/*)
add_custom_target (PublicHeaders SOURCES ${PublicHeadersFiles})

add_library (BedrockServer INTERFACE)
target_include_directories (BedrockServer INTERFACE ${EZ_ROOT}/MinecraftHeaders)
target_link_directories (BedrockServer INTERFACE ${EZ_ROOT}/Lib)
target_link_libraries (BedrockServer INTERFACE bedrock_server_mod)

if ("${STANDALONE_MODE}" OR ${CMAKE_TOOLCHAIN_FILE} STREQUAL ${CMAKE_CURRENT_LIST_FILE})
    link_directories (${EZ_ROOT}/Dist/Lib)
    function (import_library name location)
        add_library (${name} UNKNOWN IMPORTED)
        set_target_properties (${name} PROPERTIES IMPORTED_LOCATION ${location})
    endfunction ()
    import_library (ModLoader ${EZ_ROOT}/Dist/Lib/Chakra.lib)
endif ()
