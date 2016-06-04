cmake_minimum_required(VERSION 2.8)
project(tstr)
set(tstr_version 0.0.1)

option (BUILD_TESTS "Build unit tests." TRUE)

file(GLOB parse_sources tsparse/*.c)
file(GLOB parse_headers tsparse/*.h)

include_directories(jsmn)
include_directories(tsparse)

add_executable(tstr ${parse_headers} ${parse_sources} tstran.c jsmn/jsmn.c jsmn/jsmn.h)

if (BUILD_TESTS)
	include_directories(jsmn/test)
	add_executable(tests ${parse_headers} ${parse_sources} test/tests.c jsmn/test/test.h)
endif(BUILD_TESTS)

message(STATUS "BUILD_TESTS     = ${BUILD_TESTS}")
