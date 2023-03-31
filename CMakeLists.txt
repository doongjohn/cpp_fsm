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

file(GLOB_RECURSE SRC_LUA vendor/lua-5.4.4/src/*.c)
list(REMOVE_ITEM SRC_LUA
  ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/lua.c
  ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/luac.c)

file(GLOB_RECURSE SRC_FSM src/fsm/*.cpp)
add_library(cpp_fsm
  ${SRC_LUA}
  ${SRC_FSM})

target_compile_features(cpp_fsm
  PRIVATE cxx_std_20)

target_compile_options(cpp_fsm
  PRIVATE
  -Wall
  $<$<CONFIG:Release>:-flto>
  ${SANITIZER_OPTIONS})

target_link_options(cpp_fsm
  PRIVATE
  -Wall
  $<$<CONFIG:Release>:-flto>
  ${SANITIZER_OPTIONS})

target_include_directories(cpp_fsm
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor)

target_compile_definitions(cpp_fsm
  PRIVATE SOL_ALL_SAFETIES_ON=1)

# example
file(GLOB_RECURSE SRC_EXAMPLE
  vendor/backward/*.cpp # https://github.com/bombela/backward-cpp
  src/example/*.cpp)
add_executable(example
  ${SRC_EXAMPLE})

# cmake --build build -v --config Debug --target example
set_target_properties(example PROPERTIES EXCLUDE_FROM_ALL TRUE)

target_compile_features(example
  PRIVATE cxx_std_20)

target_compile_options(example
  PRIVATE
  -Wall
  $<$<CONFIG:Release>:-flto>
  ${SANITIZER_OPTIONS})

target_link_options(example
  PRIVATE
  -Wall
  $<$<CONFIG:Release>:-flto>
  ${SANITIZER_OPTIONS})

target_include_directories(example
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor)

target_compile_definitions(example
  PRIVATE SOL_ALL_SAFETIES_ON=1
  PRIVATE BACKWARD_HAS_DW=1)

target_link_libraries(example
  PRIVATE dw
  PRIVATE cpp_fsm)
