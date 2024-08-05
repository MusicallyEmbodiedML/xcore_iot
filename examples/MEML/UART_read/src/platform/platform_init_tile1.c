// Copyright (c) 2021-2022 XMOS LIMITED. This Software is subject to the terms of the
// XMOS Public License: Version 1

/* App headers */
#include "../app_conf.h"
#include "platform_init.h"

static void tile1_uart_init(void);

void platform_init_tile_1(chanend_t c_other_tile)
{
    memset(tile1_ctx, 0, sizeof(tile1_ctx_t));

    tile1_ctx->c_from_gpio = soc_channel_establish(c_other_tile, soc_channel_input);
    tile1_ctx->c_to_gpio = soc_channel_establish(c_other_tile, soc_channel_output);
    tile1_uart_init();
}


HIL_UART_RX_CALLBACK_ATTR
void uart_rx_error_callback(uart_callback_code_t callback_code, void *app_data){
    //debug_printf("uart_rx_error: 0x%x\n", callback_code);
}

static void tile1_uart_init(void)
{
    const unsigned baud_rate = 115200;

    hwtimer_t tmr_rx = hwtimer_alloc();
    uart_rx_init(
        &tile1_ctx->uart_rx_ctx,
        XS1_PORT_1M, //X1D36
        baud_rate,
        8,
        UART_PARITY_NONE,
        2,
        tmr_rx,
        NULL, // No buffer
        0,
        NULL, // No rx complete callback
        uart_rx_error_callback,
        &tile1_ctx->uart_rx_ctx
        );
}

