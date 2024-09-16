# Collect all source files
file(GLOB_RECURSE TEST_TONE_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
# Collect all library paths
set(TEST_TONE_HEADERS
    ${CMAKE_CURRENT_LIST_DIR}/include)

# Append in local scope
list(APPEND MEML_HEADERS ${TEST_TONE_HEADERS})
list(APPEND MEML_SOURCES ${TEST_TONE_SOURCES})

# Propagate in parent scope
set(MEML_HEADERS ${MEML_HEADERS} PARENT_SCOPE)
set(MEML_SOURCES ${MEML_SOURCES} PARENT_SCOPE)
