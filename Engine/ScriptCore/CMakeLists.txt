cmake_minimum_required(VERSION 3.10)

# set the project name
project(ScriptCore)

option(SCRIPT_TESTS "Include tests" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

IF( CMAKE_CXX_COMPILER_ID MATCHES "g++" )
	ENABLE_IF_SUPPORTED( CMAKE_CXX_FLAGS "-fno-implicit-templates" )
	ENABLE_IF_SUPPORTED( CMAKE_CXX_FLAGS "-Wall" )
ENDIF()

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}"/include")

file(GLOB PROJECT_SOURCE_FILES "*.h" "*.hpp" "*.cpp")

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

include_directories()

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})