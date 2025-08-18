CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
PROJECT(jarvisc)

set(APRILASR april-asr)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/shared)

include_directories(${PROJECT_SOURCE_DIR}/source/*)

if(APRILASR_FOUND)

    message("-- Found aprilasr: ${APRILASR_FOUND}")

    add_library(${APRILASR} SHARED IMPORTED)
    set_property(TARGET ${APRILASR} PROPERTY IMPORTED_LOCATION ${APRILASR_FOUND})

else() # Build and use our own april-asr library

    add_subdirectory(${PROJECT_SOURCE_DIR}/source/${APRILASR} ${PROJECT_SOURCE_DIR}/source/${APRILASR}/build)

    link_directories(${PROJECT_SOURCE_DIR}/lib/*)

    add_custom_target(
        ${APRILASR}
        COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/source/${APRILASR}/build
    )

endif()


