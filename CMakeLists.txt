cmake_minimum_required(VERSION 3.20)

project(cgParse VERSION 0.1.0 LANGUAGES CXX)

option(BUILD_UNIT_TESTS "Build cgParse unit tests" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Network WebSockets Test REQUIRED)

if(BUILD_SHARED_LIBS)
	add_compile_definitions(CGPARSE_DLL)
endif()

add_subdirectory(src)

if (BUILD_UNIT_TESTS)
	add_subdirectory(test)
endif()
