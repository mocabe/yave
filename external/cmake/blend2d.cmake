# blend2d
find_library(blend2d_LIBRARIES blend2d REQUIRED)
find_path(blend2d_INCLUDES blend2d.h REQUIRED)

add_library(blend2d INTERFACE)
target_link_libraries(blend2d INTERFACE ${blend2d_LIBRARIES})
target_include_directories(blend2d INTERFACE ${blend2d_INCLUDES})
target_compile_options(blend2d INTERFACE -DBL_STATIC)