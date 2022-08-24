cmake_minimum_required(VERSION 3.20)

add_executable(cgParseTest
    parsetest.cpp
    parsetest.h
)

add_compile_definitions(CGPARSE_DLL)

target_include_directories(cgParseTest PRIVATE "${PROJECT_SOURCE_DIR}/include")
	
target_link_libraries(cgParseTest cgParse Qt::Network Qt::WebSockets Qt::Test)