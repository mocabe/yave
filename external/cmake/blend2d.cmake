# blend2d
find_path(blend2d_INCLUDES blend2d.h REQUIRED)

set(blend2d_INSTALL_DIR ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})
find_library(blend2d_LIBRARIES_RELEASE blend2d PATHS ${blend2d_INSTALL_DIR}/lib/       NO_DEFAULT_PATH REQUIRED)
find_library(blend2d_LIBRARIES_DEBUG   blend2d PATHS ${blend2d_INSTALL_DIR}/debug/lib/ NO_DEFAULT_PATH REQUIRED)

add_library(blend2d STATIC IMPORTED)
set_target_properties(blend2d PROPERTIES
    IMPORTED_LOCATION_RELEASE ${blend2d_LIBRARIES_RELEASE}
    IMPORTED_LOCATION_DEBUG ${blend2d_LIBRARIES_DEBUG})
target_include_directories(blend2d INTERFACE ${blend2d_INCLUDES})
target_compile_definitions(blend2d INTERFACE BL_STATIC)