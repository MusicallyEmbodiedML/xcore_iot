message("")
message("-----")
message("MEML modules being included in the build system!")
message("Include directory = " "${CMAKE_CURRENT_LIST_DIR}")
message("-----")
message("")

add_library(meml_lib STATIC)

file(GLOB_RECURSE MEML_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.c ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

# Add sources to system

target_sources(meml_lib
    PRIVATE
        ${MEML_SOURCES}
)
target_include_directories(meml_lib
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/test_tones/include
)
