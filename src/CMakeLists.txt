cmake_minimum_required(VERSION 3.15.0)

# Librerias a linkear e includes
set(INCLUDES)
set(LIBRARIES)

set(INCLUDES ${INCLUDES} .)

# ZLib
find_package(ZLIB REQUIRED)
set(LIBRARIES ${LIBRARIES} "$<$<CONFIG:Debug>:${ZLIB_LIBRARY_DEBUG}>$<$<CONFIG:Release>:${ZLIB_LIBRARY_RELEASE}>")
set(INCLUDES ${INCLUDES} ${ZLIB_INCLUDE_DIR})

# LibPng
find_package(PNG REQUIRED)
set(LIBRARIES ${LIBRARIES} "$<$<CONFIG:Debug>:${PNG_LIBRARY_DEBUG}>$<$<CONFIG:Release>:${PNG_LIBRARY_RELEASE}>")
set(INCLUDES ${INCLUDES} ${PNG_PNG_INCLUDE_DIR})

include_directories(${INCLUDES})

# Proyecto de ejecutable a generar con codigos
file(GLOB SOFTWARE_RENDERER_SOURCES *.cpp *.h *.hpp)
add_executable(SoftwareRenderer ${SOFTWARE_RENDERER_SOURCES})
target_link_libraries(SoftwareRenderer ${LIBRARIES})

# Capetas de salida
set_target_properties(SoftwareRenderer PROPERTIES PDB_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<CONFIG>")
set_target_properties(SoftwareRenderer PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/$<CONFIG>")