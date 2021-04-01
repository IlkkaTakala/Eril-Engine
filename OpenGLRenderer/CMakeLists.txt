cmake_minimum_required (VERSION 3.8)

project ("OpenGLRenderer")

option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_INSTALL "" OFF)

add_compile_definitions(OPENGLRENDERER_EXPORTS)

add_subdirectory(ext/glfw-3.3.2)
add_subdirectory(ext/glm-0.9.9.7)

include_directories("${GLFW_SOURCE_DIR}/deps")

set(GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" "${GLFW_SOURCE_DIR}/deps/glad_gl.c")

add_library(OpenGLRenderer SHARED "dllmain.cpp" "framework.h" "pch.h" "pch.cpp" "Renderer.h" "Renderer.cpp" "Material.h" "Material.cpp" "Texture.h" "Texture.cpp" ${GLAD_GL} )

add_custom_command(TARGET OpenGLRenderer POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_SOURCE_DIR}/out/build/x64-debug/OpenGLRenderer.dll"      # <--this is in-file
        "${PROJECT_SOURCE_DIR}/../x64/Release/OpenGLRenderer.dll")                 # <--this is out-file path

target_link_libraries(OpenGLRenderer PUBLIC glfw PUBLIC glm)