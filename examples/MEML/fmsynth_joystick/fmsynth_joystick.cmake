#**********************
# Gather Sources
#**********************
file(GLOB_RECURSE APP_SOURCES
     ${CMAKE_CURRENT_LIST_DIR}/src/*.c
     ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
set(APP_INCLUDES
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/audio_pipeline
    ${CMAKE_CURRENT_LIST_DIR}/src/platform
    ${CMAKE_CURRENT_LIST_DIR}/src/misc
    ${CMAKE_CURRENT_LIST_DIR}/src/demos
)

#**********************
# Flags
#**********************
set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -fxscope
    -mcmodel=large
    -Wno-xcore-fptrgroup
    ${CMAKE_CURRENT_LIST_DIR}/src/config.xscope
    ${CMAKE_CURRENT_LIST_DIR}/XCORE-AI-EXPLORER.xn
)
set(APP_COMPILE_DEFINITIONS
    DEBUG_PRINT_ENABLE=1
    PLATFORM_SUPPORTS_TILE_0=1
    PLATFORM_SUPPORTS_TILE_1=1
    PLATFORM_SUPPORTS_TILE_2=0
    PLATFORM_SUPPORTS_TILE_3=0
    PLATFORM_USES_TILE_0=1
    PLATFORM_USES_TILE_1=1

    MIC_ARRAY_CONFIG_CLOCK_BLOCK_A=XS1_CLKBLK_1
    MIC_ARRAY_CONFIG_CLOCK_BLOCK_B=XS1_CLKBLK_2
    MIC_ARRAY_CONFIG_PORT_MCLK=PORT_MCLK_IN
    MIC_ARRAY_CONFIG_PORT_PDM_CLK=PORT_PDM_CLK
    MIC_ARRAY_CONFIG_PORT_PDM_DATA=PORT_PDM_DATA
    XUD_CORE_CLOCK=600
)

set(APP_LINK_OPTIONS
    -lquadflash
    -report
    ${CMAKE_CURRENT_LIST_DIR}/XCORE-AI-EXPLORER.xn
    ${CMAKE_CURRENT_LIST_DIR}/src/config.xscope
)

#**********************
# Tile Targets
#**********************
set(MEML_APP_NAME meml_fmsynth_joystick)
add_executable(${MEML_APP_NAME})
target_sources(${MEML_APP_NAME} PUBLIC ${APP_SOURCES})
target_include_directories(${MEML_APP_NAME} PUBLIC ${APP_INCLUDES})
target_compile_definitions(${MEML_APP_NAME} PRIVATE ${APP_COMPILE_DEFINITIONS})
target_compile_options(${MEML_APP_NAME} PRIVATE ${APP_COMPILER_FLAGS})
target_link_libraries(${MEML_APP_NAME} PUBLIC core::general io::all framework_core_multitile_support)
target_link_options(${MEML_APP_NAME} PRIVATE ${APP_LINK_OPTIONS})

# MCLK_FREQ,  PDM_FREQ, MIC_COUNT,  SAMPLES_PER_FRAME
mic_array_vanilla_add( ${MEML_APP_NAME}
    24576000  3072000   2           256 )

#**********************
# Create run and debug targets
#**********************
create_run_target(${MEML_APP_NAME})
create_debug_target(${MEML_APP_NAME})
create_flash_app_target(${MEML_APP_NAME})
create_install_target(${MEML_APP_NAME})
