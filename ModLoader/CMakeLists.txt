cmake_minimum_required (VERSION 3.15)

if (NOT DEFINED ChakraCore)
    find_file (ChakraCore ChakraCore.dll)
endif ()

if (NOT EXISTS ${ChakraCore})
    message (FATAL_ERROR "Failed to found ChakraCore.dll, plase install via vcpkg install chakracore:x64-windows-rel")
endif ()

install (FILES ${ChakraCore} DESTINATION .)

if (NOT DEFINED FUNCHOOK_INC)
    find_path (FUNCHOOK_INC funchook.h)
endif ()
if (NOT DEFINED FUNCHOOK_DLL)
    find_file (FUNCHOOK_DLL funchook.dll)
endif ()
if (NOT DEFINED FUNCHOOK_LIB)
    find_library (FUNCHOOK_LIB funchook_dll)
endif ()

if (NOT EXISTS ${FUNCHOOK_DLL})
    message (FATAL_ERROR "Failed to found funchook.dll, plase install via vcpkg install funchook:x64-windows-rel")
endif ()

if (NOT EXISTS ${FUNCHOOK_LIB})
    message (FATAL_ERROR "Failed to found funchook_dll.lib, plase install via vcpkg install funchook:x64-windows-rel")
endif ()

install (FILES ${FUNCHOOK_DLL} DESTINATION .)

add_library (ModLoader SHARED dllmain.cpp HealthCheck.cpp hook_impl.cpp forward.cpp)
set_target_properties (ModLoader PROPERTIES OUTPUT_NAME "Chakra")
target_include_directories (ModLoader PRIVATE ${FUNCHOOK_INC})
target_link_libraries (ModLoader PRIVATE Boost::headers ${FUNCHOOK_LIB})
install (TARGETS ModLoader RUNTIME DESTINATION . ARCHIVE DESTINATION Lib)
install_pdb (ModLoader)
set_target_properties (ModLoader PROPERTIES FOLDER Base)