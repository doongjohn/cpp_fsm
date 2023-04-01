file(GLOB_RECURSE SRC_LUA vendor/lua-5.4.4/src/*.c)
list(REMOVE_ITEM SRC_LUA
  ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/lua.c
  ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/luac.c)

# dynamic library
file(GLOB_RECURSE SRC_FSM src/fsm/*.cpp)
add_library(cpp_fsm SHARED
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

# static library
add_library(cpp_fsm_static
  ${SRC_LUA}
  ${SRC_FSM})

target_compile_features(cpp_fsm_static
  PRIVATE cxx_std_20)

target_compile_options(cpp_fsm_static
  PRIVATE
  -Wall
  $<$<CONFIG:Release>:-flto>
  ${SANITIZER_OPTIONS})

target_link_options(cpp_fsm_static
  PRIVATE
  -Wall
  $<$<CONFIG:Release>:-flto>
  ${SANITIZER_OPTIONS})

target_include_directories(cpp_fsm_static
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor)

target_compile_definitions(cpp_fsm_static
  PRIVATE SOL_ALL_SAFETIES_ON=1)
