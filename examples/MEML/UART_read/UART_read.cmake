#**********************
# Gather Sources
#**********************
file(GLOB_RECURSE APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.c )
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
    ${CMAKE_CURRENT_LIST_DIR}/XK-EVK-XU316.xn
)
set(APP_COMPILE_DEFINITIONS
    DEBUG_PRINT_ENABLE=1
    PLATFORM_SUPPORTS_TILE_0=1
    PLATFORM_SUPPORTS_TILE_1=1
    PLATFORM_SUPPORTS_TILE_2=0
    PLATFORM_SUPPORTS_TILE_3=0
    PLATFORM_USES_TILE_0=1
    PLATFORM_USES_TILE_1=1

    XUD_CORE_CLOCK=600
)

set(APP_LINK_OPTIONS
    -lquadflash
    -report
    ${CMAKE_CURRENT_LIST_DIR}/XK-EVK-XU316.xn
    ${CMAKE_CURRENT_LIST_DIR}/src/config.xscope
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#**********************
# Tile Targets
#**********************
add_executable(meml_uart_read)
target_sources(meml_uart_read PUBLIC ${APP_SOURCES})
target_include_directories(meml_uart_read PUBLIC ${APP_INCLUDES})
target_compile_definitions(meml_uart_read PRIVATE ${APP_COMPILE_DEFINITIONS})
target_compile_options(meml_uart_read PRIVATE ${APP_COMPILER_FLAGS})
target_link_libraries(meml_uart_read PUBLIC core::general io::all framework_core_multitile_support)
target_link_options(meml_uart_read PRIVATE ${APP_LINK_OPTIONS})

#**********************
# Create run and debug targets
#**********************
create_run_target(meml_uart_read)
create_debug_target(meml_uart_read)
create_flash_app_target(meml_uart_read)
create_install_target(meml_uart_read)
