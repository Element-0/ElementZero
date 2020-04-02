include(vcpkg_common_functions)

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO kubo/funchook
    REF v1.1.0
    SHA512 37036a1bd525df9b88ed8731ac874b1c65a998384ec15c786ee6adbc5f40c5513a416d62fa741c6bb9aa7b09e538dbb5085e190bf5e067299510ca8950c21602
    HEAD_REF master
    PATCHES distorm.patch
)

vcpkg_from_github(
    OUT_SOURCE_PATH DISTORM_SOURCE_PATH
    REPO gdabah/distorm
    REF v3.4.1
    SHA512 0e9f8b62bc190ef7d516f1902b6003adef9c7d5d4a5f985fb0bdfc5d4838b2805e2b8836b02d5eccdb3401e814417de615dec675aed9e606c93122ca8a0d2083
    HEAD_REF master
)

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" BUILD_STATIC)
string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "dynamic" BUILD_SHARED)

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS
        -DFUNCHOOK_BUILD_STATIC=${BUILD_STATIC}
        -DFUNCHOOK_BUILD_TESTS=OFF
        -DDISTORM_PATH=${DISTORM_SOURCE_PATH}
        ${OPTIONS}
)

vcpkg_install_cmake()
vcpkg_copy_pdbs()

file(REMOVE_RECURSE ${CURRENT_PACKAGES_DIR}/debug/include)

file(COPY ${SOURCE_PATH}/LICENSE DESTINATION ${CURRENT_PACKAGES_DIR}/share/funchook)
file(RENAME ${CURRENT_PACKAGES_DIR}/share/funchook/LICENSE ${CURRENT_PACKAGES_DIR}/share/funchook/copyright)