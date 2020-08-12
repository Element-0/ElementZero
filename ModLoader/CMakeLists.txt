cmake_minimum_required (VERSION 3.15)

if (NOT DEFINED ChakraCore)
    find_file (ChakraCore
        NAMES ChakraCore.dll
        HINTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
endif ()

if (NOT EXISTS ${ChakraCore})
    message (FATAL_ERROR "Failed to found ChakraCore.dll, plase install via vcpkg install chakracore:elementzero")
endif ()

install (FILES ${ChakraCore} DESTINATION .)

if (NOT DEFINED FUNCHOOK_INC)
    find_path (FUNCHOOK_INC
        NAMES funchook.h
        HINTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")
endif ()
if (NOT DEFINED FUNCHOOK_DLL)
    find_file (FUNCHOOK_DLL
        NAMES funchook.dll
        HINTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
endif ()
if (NOT DEFINED FUNCHOOK_LIB)
    find_library (FUNCHOOK_LIB
        NAMES funchook_dll
        HINTS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
endif ()

if (NOT EXISTS ${FUNCHOOK_DLL})
    message (FATAL_ERROR "Failed to found funchook.dll, plase install via vcpkg install funchook:elementzero")
endif ()

if (NOT EXISTS ${FUNCHOOK_LIB})
    message (FATAL_ERROR "Failed to found funchook_dll.lib, plase install via vcpkg install funchook:elementzero")
endif ()

install (FILES ${FUNCHOOK_DLL} DESTINATION .)

add_library (ModLoader SHARED dllmain.cpp HealthCheck.cpp hook_impl.cpp forward.cpp)
set_target_properties (ModLoader PROPERTIES OUTPUT_NAME "Chakra")
target_include_directories (ModLoader PRIVATE ${FUNCHOOK_INC})
target_link_libraries (ModLoader PRIVATE Boost::system SQLiteCpp ${FUNCHOOK_LIB})
install (TARGETS ModLoader RUNTIME DESTINATION . ARCHIVE DESTINATION Lib)
install_pdb (ModLoader)
set_target_properties (ModLoader PROPERTIES FOLDER Base)