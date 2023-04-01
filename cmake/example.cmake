# backward (stack trace printer)
# https://github.com/bombela/backward-cpp
if (UNIX AND NOT APPLE)
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

# example
file(GLOB_RECURSE SRC_EXAMPLE src/example/*.cpp)
add_executable(example ${SRC_EXAMPLE})

# cmake --build build -v --config Debug --target example
set_target_properties(example PROPERTIES EXCLUDE_FROM_ALL true)

target_compile_features(example
  PRIVATE cxx_std_20)

target_compile_options(example
  PRIVATE
  $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
  $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>)

target_link_options(example
  PRIVATE
  $<$<CONFIG:Release>:${RELEASE_OPTIONS}>
  $<$<CONFIG:Debug>:${DEBUG_OPTIONS}>)

target_include_directories(example
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor/lua-5.4.4/src
  PRIVATE ${PROJECT_SOURCE_DIR}/vendor)

target_compile_definitions(example
  PRIVATE SOL_ALL_SAFETIES_ON=1)

if (MSVC)
  target_link_libraries(example
    PRIVATE cpp_fsm_shared)
elseif (UNIX AND NOT APPLE)
  target_link_libraries(example
    PRIVATE cpp_fsm_shared
    PRIVATE $<$<CONFIG:Debug>:backward>)
endif()
