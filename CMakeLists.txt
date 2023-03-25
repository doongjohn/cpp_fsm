cmake_minimum_required(VERSION 3.25)

# generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
  cpp_fsm
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL ""
  LANGUAGES CXX
)

set(TARGET_NAME cpp_fsm)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(${TARGET_NAME} ${SRC_FILES})

target_compile_features(${TARGET_NAME}
  PRIVATE cxx_std_20)

target_compile_options(${TARGET_NAME}
  PRIVATE
    -Wall
    $<$<CONFIG:Release>:-flto>
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    $<$<CONFIG:Debug>:-fno-sanitize-recover=all>
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

target_link_options(${TARGET_NAME}
  PRIVATE
    -Wall
    $<$<CONFIG:Release>:-flto>
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    $<$<CONFIG:Debug>:-fno-sanitize-recover=all>
    $<$<CONFIG:Debug>:-fsanitize=address,undefined>)

target_compile_definitions(${TARGET_NAME}
  # https://github.com/bombela/backward-cpp
  PRIVATE BACKWARD_HAS_DW=1)

# target_include_directories(${TARGET_NAME}
#   PRIVATE ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(${TARGET_NAME}
  PRIVATE dw)
