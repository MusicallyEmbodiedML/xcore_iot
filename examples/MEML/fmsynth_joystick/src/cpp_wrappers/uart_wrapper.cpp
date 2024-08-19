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
#include <cstring>

///
// C++ HELPER CLASSES
///

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
    std::vector< std::string > token_buffer_;
    bool buffer_available_;
    

    void _PrintBufferState();
    void _Split(char *s, const char *delim);
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
        _Split(reinterpret_cast<char*>(buffer_.data()), ",");

        // Reset
        Reset();
   } else {
        buffer_available_ = 0;
        // Keep populating buffers
        if (buffer_idx_ >= kBuffer_size-1) {
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
    if (buffer_available_) {
        // This flag is NOT thread-safe - upgrade to atomic and
        // std::binary_semaphore if required

        for (auto t : token_buffer_) {
            message.push_back(t);
        }
    }
    return buffer_available_;
}


void MEML_UART::_PrintBufferState()
{
    if (buffer_idx_ < kBuffer_size-1) {
        buffer_[buffer_idx_] = '\0';
    } else {
        buffer_[buffer_idx_-1] = '\0';
    }
    debug_printf("Buffer: %s\n", buffer_);
}


void MEML_UART::_Split(char *s, const char *delim)
{
    token_buffer_.clear();

    char *pch = std::strtok(s, delim);
    unsigned int pch_counter = 0;

    while (pch != nullptr)
    {
        token_buffer_.push_back(std::string(pch));
        pch = std::strtok(nullptr, delim);
        pch_counter++;
    }

    // If something is found, signal buffer available
    if (pch_counter > 0) {
        buffer_available_ = true;
    }
}

///
// C WRAPPER TASK
///

void uart_rx_task(uart_rx_t* uart_rx_ctx, chanend_t uart_dispatcher)
{
    auto uart_if = std::make_unique<MEML_UART>(uart_rx_ctx);
    debug_printf("Initialised UART RX\n");

    while(1) {
        std::vector<std::string> message;
        uart_if->Process();
        if (uart_if->GetMessage(message)) {
            for (auto s : message) {
                debug_printf(s.c_str());
                debug_printf(" ");
            }
            debug_printf("\n");
            //debug_printf("Sending UART message down via channel.\n");
            //chan_out_buf_byte(
            //    uart_dispatcher,
            //    reinterpret_cast<unsigned char *>(message.data()),
            //    message.size() * sizeof(std::string)
            //);
        }
    }
}


void uart_tx_task(uart_tx_t* uart_tx_ctx)
{
    return;
}
