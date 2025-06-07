cmake_minimum_required(VERSION 3.20)

add_executable(cgParseTest
    parsetest.cpp
    parsetest.h
)

target_include_directories(cgParseTest PRIVATE "${PROJECT_SOURCE_DIR}/include")
	
target_link_libraries(cgParseTest cgParse Qt6::Network Qt6::WebSockets Qt6::Test)

set(DEBUG_PATH "PATH=%PATH%" "${CMAKE_PREFIX_PATH}/bin")
set_target_properties(cgParseTest PROPERTIES VS_DEBUGGER_ENVIRONMENT "${DEBUG_PATH}")