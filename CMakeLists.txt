cmake_minimum_required(VERSION 2.8)
project(tstr)
set(tstr_version 0.0.1)
set(out_dir "${PROJECT_SOURCE_DIR}/bin")

file(GLOB sources *.c)
file(GLOB headers *.h)

add_executable(tstr ${headers} ${sources})
