// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* System headers */
#include <platform.h>
#include <xs1.h>
#include <stdbool.h>
#include <string.h>

/* Platform headers */
#include "soc.h"
#include "xcore_utils.h"
#include "uart.h"

/* App headers */
#include "app_conf.h"
#include "app_demos.h"


#define MESSAGE_SIZE 20
char buffer[MESSAGE_SIZE];


void uart_rx_demo(uart_rx_t* uart_rx_ctx)
{
    debug_printf("Initialised UART RX\n");
    int i = 0;
    while(1) {
        uint8_t rx = uart_rx(uart_rx_ctx);
        debug_printf("Received: %u\n", rx);
        if (rx == 10) {
            debug_printf("Buffer: %s\n", buffer);
            char * p;
            p = strtok(buffer, ",");
            while (p != NULL) {
                int coord = atoi(p);
                debug_printf("Coordinate: %u\n", coord);
                p = strtok(NULL, ",");
            }
            i = 0;
        } else {
            if (i >= MESSAGE_SIZE)
                i = 0;
            buffer[i] = (char)rx;
            i++;
        }

    }
}


void uart_tx_demo(uart_tx_t* uart_tx_ctx)
{
    return;
}
