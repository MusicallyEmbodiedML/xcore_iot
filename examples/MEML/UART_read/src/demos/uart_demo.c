// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* System headers */
#include <platform.h>
#include <xs1.h>
#include <stdbool.h>

/* Platform headers */
#include "soc.h"
#include "xcore_utils.h"
#include "uart.h"

/* App headers */
#include "app_conf.h"
#include "app_demos.h"

void uart_rx_demo(uart_rx_t* uart_rx_ctx)
{
    debug_printf("Initialised UART RX\n");
    while(1) {
        uint8_t rx = uart_rx(uart_rx_ctx);
        debug_printf("Received: %u\n", rx);
    }
}
