// Copyright (c) 2021 XMOS LIMITED. This Software is subject to the terms of the
// XMOS Public License: Version 1

/* App headers */
#include "platform_init.h"
#include <xs1.h>

static void tile0_setup_mclk(void);
static void tile0_uart_init(void);

void platform_init_tile_0(chanend_t c_other_tile)
{
    memset(tile0_ctx, 0, sizeof(tile0_ctx_t));

    tile0_ctx->c_from_gpio = soc_channel_establish(c_other_tile, soc_channel_output);
    tile0_ctx->c_to_gpio = soc_channel_establish(c_other_tile, soc_channel_input);

    tile0_setup_mclk();
    tile0_uart_init();
}

static void tile0_setup_mclk(void)
{
    port_enable(PORT_MCLK_IN);
    clock_enable(MCLK_CLKBLK);
    clock_set_source_port(MCLK_CLKBLK, PORT_MCLK_IN);
    port_set_clock(PORT_MCLK_IN, MCLK_CLKBLK);
    clock_start(MCLK_CLKBLK);
}


HIL_UART_RX_CALLBACK_ATTR
void uart_rx_error_callback(uart_callback_code_t callback_code, void *app_data){
    //debug_printf("uart_rx_error: 0x%x\n", callback_code);
}


static void tile0_uart_init(void)
{
    const unsigned baud_rate = 115200;

    port_enable(PORT_MCLK_IN);

    hwtimer_t tmr_rx = hwtimer_alloc();
    uart_rx_init(
        &tile0_ctx->uart_rx_ctx,
        PORT_UART_RX, //X0D11
        baud_rate,
        8,
        UART_PARITY_NONE,
        2,
        tmr_rx,
        NULL, // No buffer
        0,
        NULL, // No rx complete callback
        uart_rx_error_callback,
        &tile0_ctx->uart_rx_ctx
        );
}
