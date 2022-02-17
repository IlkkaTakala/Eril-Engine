
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories("${PROJECT_SOURCE_DIR}/include")
link_directories(${CMAKE_BINARY_DIR})




# IF( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	# SET_TARGET_PROPERTIES(test_assignment PROPERTIES LINK_FLAGS_DEBUG "/WHOLEARCHIVE:ScriptCore.lib")
	# SET_TARGET_PROPERTIES(test_assignment PROPERTIES LINK_FLAGS_RELEASE "/WHOLEARCHIVE:ScriptCore.lib")
# ENDIF()

ADD_EXECUTABLE( test_assignment test_assignment.cpp )
target_link_libraries(test_assignment PUBLIC ScriptCore )
ADD_TEST( assignment test_assignment )

ADD_EXECUTABLE( test_math test_math.cpp )
target_link_libraries(test_math PUBLIC ScriptCore )
ADD_TEST( math test_math )
