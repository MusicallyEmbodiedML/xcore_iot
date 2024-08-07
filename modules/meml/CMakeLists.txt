add_library(meml_lib STATIC)

# Variable for source files
set(MEML_SOURCES)
# Variable for header paths
set(MEML_HEADERS)

##
# Add submodules here
##
add_subdirectory(test_tones)
add_subdirectory(mlp)
add_subdirectory(fmsynth)
##
# /submodules
##

# Add sources to system
target_sources(meml_lib
    PRIVATE
        ${MEML_SOURCES}
)
target_include_directories(meml_lib
    PUBLIC
        ${MEML_HEADERS}
)

target_compile_definitions(meml_lib
    PRIVATE
)

target_compile_options(meml_lib
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>
)

message("")
message("-----")
message("MEML modules being included in the build system!")
message("")
message("Include directories = " "${MEML_HEADERS}")
message("")
message("Include sources = " "${MEML_SOURCES}")
message("-----")
message("")
