cmake_minimum_required (VERSION 3.15)

ExternalProject_Add (ChakraCore
    URL https://hertz.services/proxy/aka.ms/chakracore/cc_windows_all_1_11_15 https://aka.ms/chakracore/cc_windows_all_1_11_15
    URL_HASH SHA256=1A38DEBC83608CF640D1782A61C49F21A73D182DFCB7163821AC714BDBFC6398
    DOWNLOAD_NAME cc_windows_1_11_15.zip
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE ON
    BUILD_COMMAND ""
    INSTALL_COMMAND cmake -E copy_if_different x64_release/ChakraCore.dll <INSTALL_DIR>
)
ExternalProject_Get_Property (ChakraCore INSTALL_DIR)
install (FILES ${INSTALL_DIR}/ChakraCore.dll DESTINATION .)

ExternalProject_Add (FuncHook
    GIT_REPOSITORY https://github.com/kubo/funchook
    UPDATE_COMMAND ""
    CMAKE_ARGS ${CMAKE_ARGS} -DFUNCHOOK_BUILD_STATIC=OFF -DFUNCHOOK_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property (FuncHook INSTALL_DIR)

install (FILES ${INSTALL_DIR}/bin/FuncHook.dll DESTINATION .)

add_library (ModLoader SHARED dllmain.cpp HealthCheck.cpp hook_impl.cpp forward.cpp)
set_target_properties (ModLoader PROPERTIES OUTPUT_NAME "Chakra")
add_dependencies (ModLoader ChakraCore FuncHook)
target_include_directories (ModLoader PRIVATE ${INSTALL_DIR}/include)
target_link_directories (ModLoader PRIVATE ${INSTALL_DIR}/lib)
target_link_libraries (ModLoader PRIVATE funchook_dll)
install (TARGETS ModLoader RUNTIME DESTINATION .)