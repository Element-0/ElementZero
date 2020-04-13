cmake_minimum_required (VERSION 3.8)

file (GLOB cmakefiles
    LIST_DIRECTORIES true
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/*/CMakeLists.txt)
foreach (file ${cmakefiles})
    get_filename_component (filep ${file} DIRECTORY)
    add_subdirectory (${filep})
endforeach ()