cmake_minimum_required(VERSION 2.6)
project(addCameraControl)
set(CMAKE_BUILD_TYPE Debug)
set(cpp_flags -std=c++0x  )
add_definitions(${cpp_flags})

set(glsdk_root $ENV{glsdk_root})
add_definitions(-DSHADER_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")
add_definitions(-DDATA_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/")
link_directories(${glsdk_root}/freeglut/lib ${glsdk_root}/glload/lib ${glsdk_root}/glutil/lib)
include_directories(${glsdk_root}/freeglut/include ${glsdk_root}/glload/include
  ${glsdk_root}/glutil/include ${glsdk_root}/glm/ )
add_executable(main main.cpp misc.cpp)
target_link_libraries(main  glutil glload freeglut GL GLU X11 Xrandr pthread)
