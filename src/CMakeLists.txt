CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
PROJECT(jarvisc)

set(outputDir ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${bitness})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${outputDir}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${outputDir}/lib/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${outputDir})

include_directories(${PROJECT_SOURCE_DIR}/../include)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/main.cpp)
