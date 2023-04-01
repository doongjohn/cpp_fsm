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
