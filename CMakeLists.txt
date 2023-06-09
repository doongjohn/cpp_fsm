cmake_minimum_required(VERSION 3.25)

# enable compiler diagnostic color
add_compile_options(
  $<$<C_COMPILER_ID:GNU>:-fdiagnostics-color>
  $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color>
  $<$<C_COMPILER_ID:Clang>:-fcolor-diagnostics>
  $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>)

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(cpp_fsm
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL ""
  LANGUAGES C CXX
)

if (MSVC)
  set(RELEASE_OPTIONS)
elseif (UNIX AND NOT APPLE)
  set(RELEASE_OPTIONS
    -Wall
    -Wextra
    # -flto
  )
endif()

if (MSVC)
  set(DEBUG_OPTIONS)
elseif (UNIX AND NOT APPLE)
  set(DEBUG_OPTIONS
    -Wall
    -Wextra
    -fno-omit-frame-pointer
    -fno-sanitize-recover=all
    -fsanitize=address,undefined
  )
endif()

if (UNIX AND NOT APPLE)
  # backward (stack trace pretty printer)
  # https://github.com/bombela/backward-cpp
  add_library(backward STATIC
    vendor/backward/backward.cpp)

  target_compile_features(backward
    PRIVATE cxx_std_20)

  target_compile_options(backward
    PRIVATE ${DEBUG_OPTIONS})

  target_link_options(backward
    PRIVATE ${DEBUG_OPTIONS})

  target_compile_definitions(backward
    PRIVATE BACKWARD_HAS_DW=1)
endif()

# LuaJIT
add_subdirectory(vendor/LuaJIT)

add_compile_definitions(SOL_PRINT_ERRORS=1)
include(cmake/fsm.cmake)
include(cmake/example.cmake)
