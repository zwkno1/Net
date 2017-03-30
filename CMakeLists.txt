cmake_minimum_required (VERSION 3.4)

project(test_net)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -g")

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(./include)

#aux_source_directory(./detail SRC_LIST)

######################## test_server

set (SERVER_SRC test_server.cpp)

add_executable(test_server
	${SERVER_SRC}
	)
target_link_libraries(test_server 
	${CMAKE_THREAD_LIBS_INIT}
	)

######################## test_client

set (CLIENT_SRC test_client.cpp)

add_executable(test_client
	${CLIENT_SRC}
	)

target_link_libraries(test_client
	${CMAKE_THREAD_LIBS_INIT}
	)
