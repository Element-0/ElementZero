find_package (SQLiteCpp CONFIG REQUIRED)
find_file (SQLITE3DLL
    NAMES sqlite3.dll
    HINTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
install (FILES SQLITE3DLL DESTINATION .)
find_and_install_package (yaml-cpp yaml-cpp)
find_package (msgpack CONFIG REQUIRED)
find_package (Flatbuffers CONFIG REQUIRED)
get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    set (PKG_FOUND 1 PARENT_SCOPE)
else()
    set (PKG_FOUND 1)
endif()