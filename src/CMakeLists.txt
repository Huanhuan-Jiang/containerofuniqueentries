set(LIBRARY_NAME dequeofunique)

set(SOURCE_FILES
    dequeofunique.h
)

add_library(${LIBRARY_NAME} ${SOURCE_FILES})

target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})