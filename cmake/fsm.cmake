# object file
# https://stackoverflow.com/a/68406028
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
add_library(cpp_fsm_obj OBJECT "")

# file(GLOB_RECURSE SRC_LUA vendor/lua-5.4.4/src/*.c)
# list(REMOVE_ITEM SRC_LUA
#   ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/lua.c
#   ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src/luac.c)

file(GLOB_RECURSE SRC_FSM src/fsm/*.cpp)
target_sources(cpp_fsm_obj
  PRIVATE
    ${SRC_LUA}
    ${SRC_FSM})

target_compile_features(cpp_fsm_obj
  PUBLIC cxx_std_20)

target_link_libraries(cpp_fsm_obj
  PUBLIC libluajit)

if (MSVC)
  # https://stackoverflow.com/a/13309747
  set(PIC_OPTION)
elseif (UNIX AND NOT APPLE)
  # https://stackoverflow.com/a/31509007
  set(PIC_OPTION -fPIC)
endif()

target_compile_options(cpp_fsm_obj
  PRIVATE
  ${PIC_OPTION}
  $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
  $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>)

target_link_options(cpp_fsm_obj
  PRIVATE
  ${PIC_OPTION}
  $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
  $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>)

target_include_directories(cpp_fsm_obj
  # PUBLIC ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src
  PUBLIC ${PROJECT_SOURCE_DIR}/vendor/LuaJIT-2.1/src
  PUBLIC ${PROJECT_SOURCE_DIR}/vendor)

target_compile_definitions(cpp_fsm_obj
  PUBLIC SOL_ALL_SAFETIES_ON=1)

# dynamic library
add_library(cpp_fsm_shared SHARED)
target_link_libraries(cpp_fsm_shared
  PUBLIC cpp_fsm_obj)

# static library
add_library(cpp_fsm_static STATIC)
target_link_libraries(cpp_fsm_static
  PUBLIC cpp_fsm_obj)
