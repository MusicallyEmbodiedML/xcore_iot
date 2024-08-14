// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include "uart_wrapper.hpp"

// C platform includes
extern "C" {
/* System headers */
#include <platform.h>
#include <xs1.h>
#include <xcore/channel.h>

/* Platform headers */
#include "soc.h"
#include "xcore_utils.h"

/* App headers */
#include "app_conf.h"
}

// STL and other C++ includes
#include <array>
#include <vector>
#include <memory>
#include <string>


class MEML_UART {
 public:

    MEML_UART(uart_rx_t *uart_rx_ctx);
    void Process();
    void Reset();
    bool GetMessage(std::vector<std::string> &message);

 protected:

    static constexpr unsigned int kBuffer_size = 64;
    uart_rx_t *uart_rx_ctx_;
    std::array<unsigned char, kBuffer_size> buffer_;
    unsigned int buffer_idx_;

    void _PrintBufferState();
};


MEML_UART::MEML_UART(uart_rx_t *uart_rx_ctx) :
        uart_rx_ctx_(uart_rx_ctx),
        buffer_idx_(0)
{
}


void MEML_UART::Process()
{
    unsigned char rx = uart_rx(uart_rx_ctx_);

    if (rx == '\n') {
        // Terminated line of parameters
        _PrintBufferState();
        // Tokenise and make package
        /*
        char * p;
        p = strtok(buffer, ",");
        while (p != NULL) {
            int coord = atoi(p);
            debug_printf("Coordinate: %u\n", coord);
            p = strtok(NULL, ",");
        }
        */
        // Reset
        Reset();
   } else {
        // Keep populating buffers
        if (buffer_idx_ >= kBuffer_size) {
            // Wrap around
            buffer_idx_ = 0;
        }
        buffer_[buffer_idx_] = rx;
        buffer_idx_++;
    }
}


void MEML_UART::Reset()
{
    buffer_idx_ = 0;
}


bool MEML_UART::GetMessage(std::vector<std::string> &message)
{
    return false;
}


void MEML_UART::_PrintBufferState()
{
    if (buffer_idx_ < kBuffer_size-1) {
        buffer_[buffer_idx_] = '\0';
    }
    debug_printf("Buffer: %s\n", buffer_);
}


void uart_rx_task(uart_rx_t* uart_rx_ctx, chanend_t uart_dispatcher)
{
    auto uart_if = std::make_unique<MEML_UART>(uart_rx_ctx);
    std::vector<std::string> message;
    debug_printf("Initialised UART RX\n");

    while(1) {
        uart_if->Process();
        if (uart_if->GetMessage(message)) {
            debug_printf("Sending UART message down via channel.\n");
            chan_out_buf_byte(
                uart_dispatcher,
                reinterpret_cast<unsigned char *>(message.data()),
                message.size() * sizeof(std::string)
            );
        }
    }
}


void uart_tx_task(uart_tx_t* uart_tx_ctx)
{
    return;
}
