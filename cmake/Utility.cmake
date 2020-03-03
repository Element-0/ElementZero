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

function (find_and_install_package name)
    find_package (${name} CONFIG REQUIRED)
    get_target_property (dll_files ${name} IMPORTED_LOCATION_RELEASE)
    install (FILES ${dll_files} DESTINATION .)
endfunction ()