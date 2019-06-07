function(init_lib_cmake NAME)
  message(STATUS "Initializing submodule: ${NAME}")
  execute_process(COMMAND git submodule update --init --jobs 4 -- external/${NAME}
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endfunction()

function(init_and_add_lib_cmake NAME)
  init_lib_cmake(${NAME})
  message(STATUS "Adding library target: ${NAME}")
  add_subdirectory(external/${NAME})
endfunction()

# fmt
init_and_add_lib_cmake(fmt)

# spdlog 
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)
init_and_add_lib_cmake(spdlog)

# glm
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
init_and_add_lib_cmake(glm)

# Cathc2
init_and_add_lib_cmake(Catch2)

# glfw
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC ON CACHE BOOL "" FORCE)
init_and_add_lib_cmake(glfw)

# Vulkan
# ${Vulkan_INCLUDE_DIRS}
# ${Vulkan_LIBRARIES}
find_package(Vulkan REQUIRED)
add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(vulkan INTERFACE ${Vulkan_LIBRARIES})

# imgui
init_lib_cmake(imgui)

add_library(imgui 
  external/imgui/imgui.cpp
  external/imgui/imgui_widgets.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_demo.cpp
)

target_include_directories(imgui PUBLIC 
  external/imgui
)

add_library(imgui-glfw-vulkan 
  external/imgui/examples/imgui_impl_vulkan.cpp
  external/imgui/examples/imgui_impl_glfw.cpp
)
target_include_directories(imgui-glfw-vulkan PUBLIC 
  external/imgui/examples
  ${Vulkan_INCLUDE_DIRS}
)
target_link_libraries(imgui-glfw-vulkan PUBLIC 
  imgui
  glfw
  ${Vulkan_LIBRARIES}
)
target_compile_definitions(imgui-glfw-vulkan PUBLIC IMGUI_VULKAN_DEBUG_REPORT)

add_executable(imgui_demo external/imgui/examples/example_glfw_vulkan/main.cpp)
target_link_libraries(imgui_demo imgui-glfw-vulkan)

# boost
message(STATUS "Initializing submodule: boost")
execute_process(COMMAND git submodule update --init --recursive --jobs 8 external/boost
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

message(STATUS "Building boost library")

# bootstrap
if((NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/bjam.exe OR 
    NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/b2.exe) 
    AND
   (NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/bjam OR
    NOT EXISTS ${YAVE_EXTERNAL_DIR}/boost/b2))
  execute_process(COMMAND sh bootstrap.sh 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
endif()

# Build Boost library
if(MINGW)
  # Use default "mgw" toolset. which uses gcc.
  # Use workaround for clang to link to gcc binary.
  execute_process(COMMAND ./b2 -j 8 link=static address-model=64 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
elseif(YAVE_COMPILER_MSVC)
  # Should link Boost::disable_autolinking on all targets to avoid link errors.
  execute_process(COMMAND ./b2 -j 8 toolset=msvc link=static address-model=64 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
else()
  execute_process(COMMAND ./b2 -j 8 toolset=gcc link=static address-model=64 
                  WORKING_DIRECTORY ${YAVE_EXTERNAL_DIR}/boost)
endif()

# MinGW clang workaround
if(MINGW)
  # get current gcc version
  execute_process(COMMAND gcc -dumpversion 
                  OUTPUT_VARIABLE YAVE_GCC_VERSION 
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.[0-9]+)?" "\\1" 
    YAVE_GCC_VERSION_MAJOR "${YAVE_GCC_VERSION}")
  string(REGEX REPLACE "([0-9]+)\\.([0-9]+)(\\.[0-9]+)?" "\\2" 
    YAVE_GCC_VERSION_MINOR "${YAVE_GCC_VERSION}")

  # set compiler version
  set(YAVE_GCC_VERSION_TAG "${YAVE_GCC_VERSION_MAJOR}${YAVE_GCC_VERSION_MINOR}")
  set(Boost_COMPILER "-mgw${YAVE_GCC_VERSION_TAG}")
endif()

# Let FindBoost create Boost targets
message(STATUS "Find package: boost")
set(BOOST_ROOT "${YAVE_EXTERNAL_DIR}/boost" CACHE PATH "" FORCE)
set (Boost_ARCHITECTURE "-x64")
set(Boost_USE_STATIC_LIBS ON)
set(Boost_NO_SYSTEM_PATHS TRUE)
find_package(Boost COMPONENTS random program_options REQUIRED)