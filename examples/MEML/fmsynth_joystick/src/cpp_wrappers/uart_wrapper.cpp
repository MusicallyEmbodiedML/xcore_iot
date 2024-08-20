// Copyright 2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include "uart_wrapper.hpp"

// C platform includes
extern "C" {
/* System headers */
#include <platform.h>
#include <xs1.h>
#include <xcore/channel.h>
#include <xscope.h>

/* Platform headers */
#include "soc.h"
#include "xcore_utils.h"

/* App headers */
#include "app_conf.h"
}  // extern "C"

// STL and other C++ includes
#include <array>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <cmath>

#include "chans_and_data.h"


/****** Alternate float_to_char */

#define CHAR_BUFF_SIZE 10

static char * _float_to_char(float x, char *p) {
    char *s = p + CHAR_BUFF_SIZE; // go to end of buffer
    uint16_t decimals;  // variable to store the decimals
    int units;  // variable to store the units (part to left of decimal place)
    if (x < 0) { // take care of negative numbers
        decimals = (int)(x * -100) % 1000; // make 1000 for 3 decimals etc.
        units = (int)(-1 * x);
    } else { // positive numbers
        decimals = (int)(x * 100) % 100;
        units = (int)x;
    }

    *--s = (decimals % 10) + '0';
    decimals /= 10; // repeat for as many decimal places as you need
    *--s = (decimals % 10) + '0';
    *--s = '.';

    while (units > 0) {
        *--s = (units % 10) + '0';
        units /= 10;
    }
    if (x < 0) *--s = '-'; // unary minus sign for negative numbers
    return s;
}

/****************************** */


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

#if 0
    if (std::isinf(read->potX)) {
        debug_printf("UART- Inf!\n");
    }
    else
    {
        static char potX_s[CHAR_BUFF_SIZE] = { 0 };
        static char potY_s[CHAR_BUFF_SIZE] = { 0 };
        static char potRotate_s[CHAR_BUFF_SIZE] = { 0 };

        _float_to_char(read->potX, potX_s);
        _float_to_char(read->potY, potY_s);
        _float_to_char(read->potRotate, potRotate_s);
        
        debug_printf("UART- Message values: ");
        debug_printf(potX_s); debug_printf(", ");
        debug_printf(potY_s); debug_printf(", ");
        debug_printf(potRotate_s); debug_printf("\n");
    }
#else
    xscope_float(0, read->potX);
    xscope_float(1, read->potY);
    xscope_float(2, read->potRotate);
#endif

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
                //debug_printf("UART- Sending UART message down via channel.\n");
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
