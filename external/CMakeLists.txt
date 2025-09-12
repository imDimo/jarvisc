CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(jarvisc)

set(APRILASR april-asr)
set(PORTAUDIO PortAudio)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/shared)

include_directories(${PROJECT_SOURCE_DIR}/source/*)

if(APRILASR_FOUND)

    message("-- Found aprilasr: ${APRILASR_FOUND}")

    add_library(${APRILASR} SHARED IMPORTED)
    set_property(TARGET ${APRILASR} PROPERTY IMPORTED_LOCATION ${APRILASR_FOUND})

else() # Build and use our own april-asr library

    add_subdirectory(${PROJECT_SOURCE_DIR}/source/${APRILASR} ${PROJECT_SOURCE_DIR}/source/${APRILASR}/build)

    link_directories(${PROJECT_SOURCE_DIR}/lib/shared)
    link_directories(${PROJECT_SOURCE_DIR}/lib/static)

    add_custom_target(
        ${APRILASR}
        COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source/${APRILASR}/build
    )

endif()

if(PORTAUDIO_FOUND)

    message("-- Found portaudio: ${PORTAUDIO_FOUND}")

    add_library(${PORTAUDIO}_static STATIC IMPORTED)
    set_property(TARGET ${PORTAUDIO} PROPERTY IMPORTED_LOCATION ${PORTAUDIO_FOUND})

else()

    execute_process(COMMAND bash ./configure --enable-static WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source/${PORTAUDIO})

    # add_subdirectory(${PROJECT_SOURCE_DIR}/source/${PORTAUDIO} ${PROJECT_SOURCE_DIR}/source/${PORTAUDIO}/build)

    link_directories(${PROJECT_SOURCE_DIR}/lib/shared)
    link_directories(${PROJECT_SOURCE_DIR}/lib/static)

    add_custom_target(
        ${PORTAUDIO}_static
        COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source/${PORTAUDIO}
    )

endif()
