cmake_minimum_required (VERSION 3.8)

project ("WineBDLauncher")

include (ExternalProject)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS ON)

ExternalProject_Add (BoostImport
    URL https://www.nuget.org/api/v2/package/boost/1.71.0
    URL_HASH SHA256=86A7DAFED44D6B45F26CAC862C0A4270348447AF253A3BD441746DD1012D625D
    DOWNLOAD_NAME boost_1.71.0.zip
    UPDATE_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE ON
    BUILD_COMMAND ""
    INSTALL_COMMAND cmake -E copy_directory lib/native <INSTALL_DIR>
)
ExternalProject_Get_Property (BoostImport INSTALL_DIR)
file (MAKE_DIRECTORY ${INSTALL_DIR}/include)
add_library (Boost INTERFACE IMPORTED)
add_dependencies (Boost BoostImport)
target_include_directories (Boost INTERFACE ${INSTALL_DIR}/include)

ExternalProject_Add (YAMLImport
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
    UPDATE_COMMAND ""
    CMAKE_ARGS ${CMAKE_ARGS} -DCMAKE_BUILD_TYPE=MinSizeRel -DYAML_CPP_BUILD_TESTS=OFF -DYAML_MSVC_SHARED_RT=ON -DYAML_CPP_INSTALL=ON -DYAML_CPP_BUILD_CONTRIB=OFF -DYAML_CPP_BUILD_TOOLS=OFF -DBUILD_SHARED_LIBS=ON -DYAML_BUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)
ExternalProject_Get_Property (YAMLImport INSTALL_DIR)
file (MAKE_DIRECTORY ${INSTALL_DIR}/include)
file (MAKE_DIRECTORY ${INSTALL_DIR}/lib)
install(FILES ${INSTALL_DIR}/bin/yaml-cpp.dll DESTINATION .)
add_library (YAML INTERFACE IMPORTED)
add_dependencies (YAML YAMLImport)
target_link_directories (YAML INTERFACE ${INSTALL_DIR}/lib)
target_link_libraries (YAML INTERFACE yaml-cpp)
target_include_directories (YAML INTERFACE ${INSTALL_DIR}/include)

include_directories (Public)
add_compile_definitions (_UNICODE UNICODE NDEBUG)
add_compile_options (-Wno-invalid-offsetof -Wno-deprecated-declarations)

add_library (MinecraftHeaders INTERFACE)
target_include_directories (MinecraftHeaders INTERFACE MinecraftHeaders)
target_link_directories (MinecraftHeaders INTERFACE Lib)
target_link_libraries (MinecraftHeaders INTERFACE bedrock_server_mod)

add_subdirectory ("ModLoader")
add_subdirectory ("Base")
add_subdirectory ("CommandSupport")
add_subdirectory ("Essentials")
