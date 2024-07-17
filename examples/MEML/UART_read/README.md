# MEML UART read example

This example application demonstrates UART reading on the XK-EVK-XU316 (no TX, no loopback, no other feature demos).

In order for the demo to work, you must connect Pin 36 on J10 to the UART TX of your tranmitting device.

The demo is set up to read UART with the following configuration:

- Baud: 9600
- Data bits: 8
- Parity: None
- Stop bits: 2

## CMake Targets

The following CMake targets are provided:

- meml_uart_read
- run_meml_uart_read
- debug_meml_uart_read

## Deploying the Firmware

See the Programming Guide for information on building and running the application.
