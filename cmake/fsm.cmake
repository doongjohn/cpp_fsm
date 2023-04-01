file(GLOB_RECURSE SRC_LUA vendor/lua-5.4.4/src/*.c)
list(REMOVE_ITEM SRC_LUA
  ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/lua.c
  ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/luac.c)

# object file
# https://stackoverflow.com/a/68406028
file(GLOB_RECURSE SRC_FSM src/fsm/*.cpp)
add_library(cpp_fsm_obj OBJECT
  ${SRC_LUA}
  ${SRC_FSM})

target_compile_features(cpp_fsm_obj
  PUBLIC cxx_std_20)

if (MSVC)
  # https://stackoverflow.com/a/13309747
  set(PIC_OPTION)
elseif (UNIX AND NOT APPLE)
  # https://stackoverflow.com/a/31509007
  set(PIC_OPTION -fPIC)
endif()

target_compile_options(cpp_fsm_obj
  PUBLIC
  ${PIC_OPTION}
  $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
  $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>)

target_link_options(cpp_fsm_obj
  PUBLIC
  ${PIC_OPTION}
  $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
  $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>)

target_include_directories(cpp_fsm_obj
  PUBLIC ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src
  PUBLIC ${PROJECT_SOURCE_DIR}/vendor)

target_compile_definitions(cpp_fsm_obj
  PUBLIC SOL_ALL_SAFETIES_ON=1)

# dynamic library
add_library(cpp_fsm_shared SHARED)
target_link_libraries(cpp_fsm_shared
  PRIVATE cpp_fsm_obj)

# static library
add_library(cpp_fsm_static STATIC)
target_link_libraries(cpp_fsm_static
  PRIVATE cpp_fsm_obj)
