cmake_minimum_required(VERSION 3.10)

# set the project name
project(ErilEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_SCRIPTCORE "Use Scripts" ON)
option(OPENGL "Use OpenGL" ON)

IF(OPENGL)
add_compile_definitions(GLM_FORCE_XYZW_ONLY)
ENDIF()

IF(USE_SCRIPTCORE)
add_compile_definitions(USE_SCRIPTCORE)
ENDIF()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(_engine_root_path "${ErilEngine_SOURCE_DIR}/Engine")
set(_game_root_path "${ErilEngine_SOURCE_DIR}/Game")
set(_src_root_path "${_engine_root_path}/Source")
file(
    GLOB_RECURSE _source_list 
    LIST_DIRECTORIES false
    "${_src_root_path}/*.cpp*"
    "${_src_root_path}/*.h*"
    "${_game_root_path}/*.cpp*"
    "${_game_root_path}/*.h*"
    "${_engine_root_path}/external/RapidXML/rapidxml.cpp"
    "${_engine_root_path}/external/glad/glad_gl.c"
    "${_engine_root_path}/external/tinyfiledialogs/tinyfiledialogs.c"
)

include_directories("${_src_root_path}")
include_directories("${_game_root_path}")
include_directories("${_src_root_path}/Core")
include_directories("${_engine_root_path}/external/RapidXML")
include_directories("${_engine_root_path}/external/stb-image")
include_directories("${_engine_root_path}/external/bullet3/src")

IF(OPENGL)
    include_directories("${_src_root_path}/RenderCore/OpenGL/include")
    include_directories("${_engine_root_path}/external/glad")
ENDIF()

function(get_all_targets _result _dir)
    get_property(_subdirs DIRECTORY "${_dir}" PROPERTY SUBDIRECTORIES)
    foreach(_subdir IN LISTS _subdirs)
        get_all_targets(${_result} "${_subdir}")
    endforeach()
    get_property(_sub_targets DIRECTORY "${_dir}" PROPERTY BUILDSYSTEM_TARGETS)
    set(${_result} ${${_result}} ${_sub_targets} PARENT_SCOPE)
endfunction()

function(add_subdirectory_with_folder _folder_name _folder)
    add_subdirectory(${_folder} ${ARGN})
    get_all_targets(_targets "${_folder}")
    foreach(_target IN LISTS _targets)
        set_target_properties(
            ${_target}
            PROPERTIES FOLDER "${_folder_name}"
        )
    endforeach()
endfunction()

add_subdirectory_with_folder(Requirements "${_engine_root_path}/external/assimp")
add_subdirectory_with_folder(Requirements "${_engine_root_path}/external/bullet3")
add_subdirectory_with_folder(Requirements "${_engine_root_path}/external/glfw")
add_subdirectory_with_folder(Requirements "${_engine_root_path}/external/glm")
add_subdirectory_with_folder(Requirements "${_engine_root_path}/external/openal-soft")

set(BUILD_SHARED_LIBS OFF)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)

add_executable(ErilEngine WIN32 ${_source_list})

target_link_libraries(ErilEngine d2d1 Dwrite glfw glm assimp BulletDynamics BulletCollision Bullet3Common Bullet3Geometry Bullet3Dynamics LinearMath OpenAL)

foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    file(RELATIVE_PATH _source_path_rel "${_src_root_path}" "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()