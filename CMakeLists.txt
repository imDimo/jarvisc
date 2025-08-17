CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
PROJECT(jarvisc)

set(APRILASR april-asr)

set(bitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(bitness 64)
endif()

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/external/source/*)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/external/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/external/lib/shared)

find_library(APRILASR_FOUND aprilasr)
if(APRILASR_FOUND)

    message("-- Found aprilasr: ${APRILASR_FOUND}")

    add_library(${APRILASR} SHARED IMPORTED)
    set_property(TARGET ${APRILASR} PROPERTY IMPORTED_LOCATION ${APRILASR_FOUND})

else() # Build and use our own april-asr library

    add_subdirectory(${PROJECT_SOURCE_DIR}/external/source/${APRILASR} ${PROJECT_SOURCE_DIR}/external/source/${APRILASR}/build)

    link_directories(${PROJECT_SOURCE_DIR}/external/lib/*)

    add_custom_target(
        ${APRILASR}
        COMMAND make
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/external/source/${APRILASR}/build
    )

endif()

set(outputDir ${PROJECT_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${bitness})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${outputDir}/lib/static)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${outputDir}/lib/shared)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${outputDir})

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/src/main.cpp)

if(APRILASR_FOUND)
    target_link_libraries(${PROJECT_NAME} ${APRILASR_FOUND})
else()
    target_link_libraries(${PROJECT_NAME} ${PROJECT_SOURCE_DIR}/external/lib/shared/libaprilasr.so)
    add_dependencies(${PROJECT_NAME} ${APRILASR})
endif()
