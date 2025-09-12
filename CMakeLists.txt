CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(jarvisc)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)

set(outputDir ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${bitness})

set(bitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(bitness 64)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/external/source/PortAudio/cmake/modules")

set(APRILASR april-asr)
find_library(APRILASR_FOUND aprilasr)
if(APRILASR_FOUND)
    add_compile_definitions(APRILASR_INSTALLED)
endif()

set(PORTAUDIO PortAudio)
find_library(PORTAUDIO_FOUND portaudio)
if(PORTAUDIO_FOUND)
    add_compile_definitions(PORTAUDIO_INSTALLED)
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/external)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)

find_path(PULSEAUDIO_INCLUDE_DIR NAMES pulse/pulseaudio.h)
find_library(PULSEAUDIO_LIBRARY NAMES pulse)
include_directories(${PULSEAUDIO_INCLUDE_DIRS})

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

find_package(PulseAudio)
if(PulseAudio_FOUND)
    target_link_libraries(${PROJECT_NAME} pulse-simple pulse)
endif()

find_package(ALSA)
if(ALSA_FOUND)
    target_link_libraries(${PROJECT_NAME} ALSA::ALSA)
endif()

find_package(sndio)
target_link_libraries(${PROJECT_NAME} sndio)
