cmake_minimum_required(VERSION 3.10)

# set the project name
project(ScriptCore)

option(SCRIPT_TESTS "Include tests" ON)

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
	ENABLE_IF_SUPPORTED( CMAKE_CXX_FLAGS "-fno-implicit-templates" )
	ENABLE_IF_SUPPORTED( CMAKE_CXX_FLAGS "-Wall" )
ENDIF()

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}"/include")

file(GLOB PROJECT_INCLUDE_FILES "include/*.h")
file(GLOB PROJECT_SOURCE_FILES "private/*.hpp" "private/*.cpp" "private/*.h")

macro(GroupSources curdir)
   file(GLOB children RELATIVE ${PROJECT_SOURCE_DIR}/${curdir}
${PROJECT_SOURCE_DIR}/${curdir}/*)
   foreach(child ${children})
          if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/${curdir}/${child})
                  GroupSources(${curdir}/${child})
          else()
                  string(REPLACE "/" "\\" groupname ${curdir})
          # I would like to call the src root folder in a different name, only in visual studio (not mandatory requirement)
                   string(REPLACE "src" "Common" groupname ${groupname})
            source_group(${groupname} FILES
${PROJECT_SOURCE_DIR}/${curdir}/${child})
          endif()
   endforeach()
endmacro()
   
# Execute the macro
GroupSources(src)

if(SCRIPT_TESTS)
	add_subdirectory("tests/")
	enable_testing()
endif()

include_directories("include")

add_library(${PROJECT_NAME} SHARED ${PROJECT_INCLUDE_FILES} ${PROJECT_SOURCE_FILES})

if (WIN32)
set(TARGET_FILE_DIR /tests/$<CONFIGURATION>)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${CMAKE_BINARY_DIR}/$<CONFIG>/${PROJECT_NAME}.dll"      # <--this is in-file
        "${CMAKE_BINARY_DIR}/tests/$<CONFIG>/${PROJECT_NAME}.dll")                 # <--this is out-file path
endif (WIN32)