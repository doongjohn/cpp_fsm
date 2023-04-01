cmake_minimum_required(VERSION 3.25)

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp_fsm
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL ""
  LANGUAGES C CXX
)

set(SANITIZER_OPTIONS
  $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
  $<$<CONFIG:Debug>:-fno-sanitize-recover=all>
  $<$<CONFIG:Debug>:-fsanitize=address,undefined>
)

include(cmake/fsm.cmake)
include(cmake/example.cmake)
