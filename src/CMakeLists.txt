CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(jarvisc)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${outputDir}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${outputDir}/lib/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${outputDir})

include_directories(${PROJECT_SOURCE_DIR}/../include)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp)

add_custom_command(
    TARGET ${PROJECT_NAME} COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/scripts/pipe-parec
    ${outputDir}/pipe-parec
)
