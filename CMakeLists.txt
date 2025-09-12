CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(jarvisc)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)

set(outputDir ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${bitness})

set(bitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(bitness 64)
endif()

set(APRILASR april-asr)
set(PORTAUDIO PortAudio)

find_library(APRILASR_FOUND aprilasr)
find_library(PORTAUDIO_FOUND portaudio)

if(APRILASR_FOUND)
    add_compile_definitions(APRILASR_INSTALLED)
endif()

if(PORTAUDIO_FOUND)
    add_compile_definitions(PORTAUDIO_INSTALLED)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/external)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

if(APRILASR_FOUND)
    target_link_libraries(${PROJECT_NAME} ${APRILASR_FOUND})
else()
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/external/lib/shared/libaprilasr.so)
    add_dependencies(${PROJECT_NAME} ${APRILASR})
endif()

if(PORTAUDIO_FOUND)
    target_link_libraries(${PROJECT_NAME} ${PORTAUDIO_FOUND})
else()
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/external/lib/static/libportaudio.a)
    add_dependencies(${PROJECT_NAME} ${PORTAUDIO}_static)
endif()
