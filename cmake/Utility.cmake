macro (generate_git_version _out_var)
    find_package (Git QUIET)
    if (GIT_FOUND)
        execute_process (
            COMMAND ${GIT_EXECUTABLE} describe --abbrev=6 --always --tags
            OUTPUT_VARIABLE ${_out_var}
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )
        if (${_out_var} STREQUAL "")
            set (${_out_var} "unknown")
        endif ()
    endif ()
endmacro ()

function (find_and_install_package name target)
    find_package (${name} CONFIG REQUIRED)
    get_target_property (dll_files ${target} IMPORTED_LOCATION_RELEASE)
    install (FILES ${dll_files} DESTINATION .)
endfunction ()

function (install_pdb name)
    install (FILES $<TARGET_PDB_FILE:${name}> DESTINATION Lib)
endfunction ()

function (def_mod name)
    if (NOT PKG_FOUND)
        include (Deps)
    endif ()

    cmake_parse_arguments(ARG "BASEMOD" "" "LINK;DELAY_LINK" ${ARGN})
    file (GLOB_RECURSE srcs
        CONFIGURE_DEPENDS *.cpp)
    set (IS_BASEMOD $<BOOL:${ARG_BASEMOD}>)
    add_library (${name} SHARED ${srcs})
    if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_include_directories (${name} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    endif ()
    target_compile_definitions (${name}
        PRIVATE MODNAME=${name} $<${IS_BASEMOD}:EZVERSION=\"${git_version}\">)
    target_link_libraries (${name}
        Boost::system
        yaml-cpp
        ModLoader
        BedrockServer
        $<$<NOT:${IS_BASEMOD}>:Base>
        ${ARG_LINK}
        ${ARG_DELAY_LINK})
    install (TARGETS ${name}
        RUNTIME DESTINATION $<IF:${IS_BASEMOD},.,Mods>
        ARCHIVE DESTINATION Lib)
    install_pdb (${name})
    set_target_properties (${name}
        PROPERTIES FOLDER $<IF:${IS_BASEMOD},Base,Mods>)
    if (ARG_DELAY_LINK)
        target_link_libraries (${name} delayimp)
        foreach (target ${ARG_DELAY_LINK})
            target_link_options (${name} PRIVATE /DELAYLOAD:${target}.dll)
        endforeach ()
    endif ()
endfunction ()

function (load_subfolders)
    file (GLOB cmakefiles
        LIST_DIRECTORIES true
        RELATIVE ${CMAKE_CURRENT_LIST_DIR}
        CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_LIST_DIR}/*/CMakeLists.txt)
    foreach (file ${cmakefiles})
        get_filename_component (filep ${file} DIRECTORY)
        add_subdirectory (${filep})
    endforeach ()
endfunction ()