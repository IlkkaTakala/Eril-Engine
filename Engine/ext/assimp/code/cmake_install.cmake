# Install script for directory: A:/Delion/Git/erilengine/Engine/ext/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "A:/Delion/Git/erilengine")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/build/lib" TYPE STATIC_LIBRARY FILES "A:/Delion/Git/erilengine/lib/Debug/assimp-vc140-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/build/lib" TYPE STATIC_LIBRARY FILES "A:/Delion/Git/erilengine/lib/Release/assimp-vc140-mt.lib")
  elseif("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/build/lib" TYPE STATIC_LIBRARY FILES "A:/Delion/Git/erilengine/lib/RelWithDebInfo/assimp-vc140-mt.lib")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/build/include/assimp" TYPE FILE FILES
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/anim.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/ai_assert.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/camera.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/color4.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/color4.inl"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/config.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/defs.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/Defines.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/cfileio.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/light.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/material.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/material.inl"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/matrix3x3.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/matrix3x3.inl"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/matrix4x4.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/matrix4x4.inl"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/mesh.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/postprocess.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/quaternion.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/quaternion.inl"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/scene.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/metadata.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/texture.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/types.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/vector2.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/vector2.inl"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/vector3.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/vector3.inl"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/version.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/cimport.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/importerdesc.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/Importer.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/DefaultLogger.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/ProgressHandler.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/IOStream.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/IOSystem.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/Logger.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/LogStream.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/NullLogger.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/cexport.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/Exporter.hpp"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/DefaultIOStream.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/DefaultIOSystem.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/SceneCombiner.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/build/include/assimp/Compiler" TYPE FILE FILES
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/Compiler/poppack1.h"
    "A:/Delion/Git/erilengine/Engine/ext/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

