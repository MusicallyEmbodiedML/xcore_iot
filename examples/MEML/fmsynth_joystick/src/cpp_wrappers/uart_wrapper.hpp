#ifndef __UART_WRAPPER_HPP__
#define __UART_WRAPPER_HPP__

extern "C" {
#include <xcore/chanend.h>

#include "uart.h"


/**
 * @brief Receive and break down messages from UART to a downstream
 * dispatcher.
 * 
 * @param uart_rx_ctx UART interface as declared in uart.h - MUST
 * be initialised.
 * @param uart_dispatcher Output channel to send processed data.
 */
void uart_rx_task(uart_rx_t* uart_rx_ctx, chanend_t uart_dispatcher);

/**
 * @brief Transmission task. Currently doesn nothing.
 * 
 * @param uart_tx_ctx UART interface as declared in uart.h - MUST
 * be initialised.
 */
void uart_tx_task(uart_tx_t* uart_tx_ctx);

}



#endif  // __UART_WRAPPER_HPP__
