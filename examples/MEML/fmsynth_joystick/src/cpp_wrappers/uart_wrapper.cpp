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

#include "chans_and_data.h"

///
// C++ HELPER CLASSES
///

class MEML_UART {
 public:

    MEML_UART(uart_rx_t *uart_rx_ctx);
    void Process();
    void Reset();
    bool GetMessage(std::vector<std::string> &message);
    bool Parse(std::vector<std::string> buffer, ts_joystick_read *read);

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
    debug_printf("UART- Buffer: %s\n", buffer_);
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

bool MEML_UART::Parse(std::vector<std::string> buffer, ts_joystick_read *read)
{
    static constexpr float u16_float_scaling = 1.f/65535.f;

    if (buffer.size() != 3) {
        debug_printf("UART- Wrong buffer for joystick parse!\n");
        return false;
    }

    read->potX = std::stof(buffer[1]) * u16_float_scaling;
    read->potY = std::stof(buffer[2]) * u16_float_scaling;
    read->potRotate = std::stof(buffer[0]) * u16_float_scaling;

    return true;
}

///
// C WRAPPER TASK
///

void uart_rx_task(uart_rx_t* uart_rx_ctx, chanend_t uart_dispatcher)
{
    auto read = std::make_unique<ts_joystick_read>();
    auto uart_if = std::make_unique<MEML_UART>(uart_rx_ctx);
    debug_printf("UART- Initialised UART RX\n");

    while(1) {
        std::vector<std::string> message;

        // Process characters from the UART interface
        uart_if->Process();

        // ...until a message is received in full
        if (uart_if->GetMessage(message)) {
            
            // Parse message
            if (uart_if->Parse(message, read.get())) {

                // Send message down correct UART channel
                debug_printf("UART- Sending UART message down via channel.\n");
                chan_out_buf_byte(
                    uart_dispatcher,
                    reinterpret_cast<unsigned char *>(read.get()),
                    sizeof(ts_joystick_read)
                );
            }
        }
    }
}


void uart_tx_task(uart_tx_t* uart_tx_ctx)
{
    return;
}
