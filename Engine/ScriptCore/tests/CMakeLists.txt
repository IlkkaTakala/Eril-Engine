
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include_directories("../include")

ADD_EXECUTABLE( test_assignment test_assignment.cpp )
target_link_libraries(test_assignment PUBLIC ScriptCore )
ADD_TEST( assignment test_assignment )