cmake_minimum_required (VERSION 3.15)

if (NOT DEFINED ChakraCore)
    set (ChakraCore ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin/ChakraCore.dll)
endif ()

if (NOT EXISTS ${ChakraCore})
    message (FATAL_ERROR "Failed to found ChakraCore.dll, plase install via vcpkg install chakracore:x64-windows")
endif ()

install (FILES ${ChakraCore} DESTINATION .)

option (PREBUILT_FUNCHOOK "Use prebuilt funchook" "")
if (NOT PREBUILT_FUNCHOOK)
    ExternalProject_Add (FuncHook
        GIT_REPOSITORY https://github.com/kubo/funchook
        UPDATE_COMMAND ""
	CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DCMAKE_BUILD_TYPE=Release -DFUNCHOOK_BUILD_STATIC=OFF -DFUNCHOOK_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    )

    ExternalProject_Get_Property (FuncHook INSTALL_DIR)
else ()
    set (INSTALL_DIR ${PREBUILT_FUNCHOOK})
endif ()

install (FILES ${INSTALL_DIR}/bin/FuncHook.dll DESTINATION .)

add_library (ModLoader SHARED dllmain.cpp HealthCheck.cpp hook_impl.cpp forward.cpp)
set_target_properties (ModLoader PROPERTIES OUTPUT_NAME "Chakra")
if (NOT PREBUILT_FUNCHOOK)
    add_dependencies (ModLoader FuncHook)
endif ()
target_include_directories (ModLoader PRIVATE ${INSTALL_DIR}/include)
target_link_directories (ModLoader PRIVATE ${INSTALL_DIR}/lib)
target_link_libraries (ModLoader PRIVATE funchook_dll)
install (TARGETS ModLoader RUNTIME DESTINATION . ARCHIVE DESTINATION Lib)
install_pdb (ModLoader)
set_target_properties (ModLoader PROPERTIES FOLDER Base)