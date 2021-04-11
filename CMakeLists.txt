# CMakeList.txt : CMake project for GLFWWindow, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("GameEngine")

option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)

add_subdirectory(Engine/OpenGLRenderer/ext/glfw-3.3.2)
add_subdirectory(Engine/OpenGLRenderer/ext/glm-0.9.9.7)

include_directories("${GLFW_SOURCE_DIR}/deps")
include_directories("Engine")

set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" "${GLFW_SOURCE_DIR}/deps/glad_gl.c")

# add_executable(EngineGL "Engine/Launch.cpp" ${GLAD_GL} )

# target_link_libraries(EngineGL PUBLIC glfw PUBLIC glm)