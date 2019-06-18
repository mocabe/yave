# yave-imgui-application

add_executable(yave-imgui-application yave-imgui.cpp)
target_include_directories(yave-imgui-application PRIVATE ${YAVE_INCLUDE_DIR})
target_compile_options(yave-imgui-application PRIVATE ${YAVE_COMPILE_FLAGS})
target_link_options(yave-imgui-application PRIVATE ${YAVE_LINK_FLAGS})
target_link_libraries(yave-imgui-application PRIVATE yave::imgui)
target_link_libraries(yave-imgui-application PRIVATE Boost::boost)
target_link_libraries(yave-imgui-application PRIVATE Boost::filesystem)
target_link_libraries(yave-imgui-application PRIVATE Boost::disable_autolinking)
target_link_libraries(yave-imgui-application PRIVATE fmt)