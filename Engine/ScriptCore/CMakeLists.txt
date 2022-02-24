cmake_minimum_required(VERSION 3.10)

# set the project name
project(ScriptCore)

option(SCRIPT_TESTS "Include tests" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_definitions(SCRIPTCORE_EXPORTS)

INCLUDE( CheckCXXCompilerFlag )

MACRO( ENABLE_IF_SUPPORTED variable flag )
  STRING( REGEX REPLACE "^-" "" flagName "${flag}" )
  CHECK_CXX_COMPILER_FLAG( "${flag}"
    HAVE_FLAG_${flagName}
    )
  IF( HAVE_FLAG_${flagName} )
    SET( ${variable} "${${variable}} ${flag}")
  ENDIF()
ENDMACRO()

IF( CMAKE_CXX_COMPILER_ID MATCHES "GNU" )
	#ENABLE_IF_SUPPORTED( CMAKE_CXX_FLAGS "-fno-implicit-templates" )
	#ENABLE_IF_SUPPORTED( CMAKE_CXX_FLAGS "-Wall" )
ENDIF()

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}"/include")

file(GLOB PROJECT_INCLUDE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
file(GLOB PROJECT_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/private/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/private/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/private/*.h")

if(SCRIPT_TESTS)
	enable_testing()
	add_subdirectory("tests/")
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_library(${PROJECT_NAME} ${PROJECT_INCLUDE_FILES} ${PROJECT_SOURCE_FILES})

# if (WIN32)
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        # "${CMAKE_BINARY_DIR}/$<CONFIG>/${PROJECT_NAME}.dll"      # <--this is in-file
        # "${CMAKE_BINARY_DIR}/tests/$<CONFIG>/${PROJECT_NAME}.dll")                 # <--this is out-file path
# endif (WIN32)

# if (UNIX)
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        # "${CMAKE_BINARY_DIR}/lib${PROJECT_NAME}.a"      # <--this is in-file
        # "${CMAKE_BINARY_DIR}/tests/lib${PROJECT_NAME}.a")                 # <--this is out-file path
# endif (UNIX)