CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(jarvisc)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${outputDir}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${outputDir}/lib/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${outputDir})

include_directories(${PROJECT_SOURCE_DIR}/../include)

set(SOURCE_FILES
    main.cpp
    phrase_data.cpp
    asr_handler.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
