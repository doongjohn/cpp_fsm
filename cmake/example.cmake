# example
file(GLOB_RECURSE SRC_EXAMPLE example/*.cpp)
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
  PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_compile_definitions(example
  PRIVATE SOL_ALL_SAFETIES_ON=1)

if (MSVC)
  target_link_libraries(example
    PRIVATE cpp_fsm_shared)
elseif (UNIX AND NOT APPLE)
  target_compile_options(example
    PRIVATE -Wno-unused-parameter)
  target_link_libraries(example
    PRIVATE cpp_fsm_shared)
endif()
