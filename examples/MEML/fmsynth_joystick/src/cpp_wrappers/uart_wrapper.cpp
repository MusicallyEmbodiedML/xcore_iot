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
}  // extern "C"

// STL and other C++ includes
#include <array>
#include <vector>
#include <memory>
#include <string>
#include <cstring>
#include <cmath>
#include <cstdlib>

#include "chans_and_data.h"
#include "interface_wrapper.hpp"

/****** Alternate float_to_char */

#if 0

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

#endif

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
    bool ParseAndSend(std::vector<std::string> &buffer);

 protected:

    static constexpr unsigned int kBuffer_size = 64;
    uart_rx_t *uart_rx_ctx_;
    std::array<unsigned char, kBuffer_size> buffer_;
    unsigned int buffer_idx_;
    std::vector< std::string > token_buffer_;
    bool buffer_available_;
    bool button_states_[button_nButtons];
    

    void _PrintBufferState();
    void _Split(char *s, const char *delim);
    bool _ParseJoystick(std::vector<std::string> &buffer);
    bool _ParseButton(std::vector<std::string> &buffer);
};


MEML_UART::MEML_UART(uart_rx_t *uart_rx_ctx) :
        uart_rx_ctx_(uart_rx_ctx),
        buffer_idx_(0),
        button_states_{false}
{
}


void MEML_UART::Process()
{
    unsigned char rx = uart_rx(uart_rx_ctx_);
    if (!rx) {
        debug_printf("\\0");
    } else {
        debug_printf("%c", rx);
    }

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
    // Terminate buffer with \0!!!
    if (buffer_idx_ < kBuffer_size-1) {
        buffer_[buffer_idx_] = '\0';
    } else {
        buffer_[buffer_idx_-1] = '\0';
    }
    //debug_printf("UART- Buffer: %s\n", buffer_);
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

bool MEML_UART::_ParseJoystick(std::vector<std::string> &buffer)
{
    static constexpr float u16_float_scaling = 1.f/65535.f;

    if (buffer.size() != 2) {
        debug_printf("UART- Wrong buffer for joystick parse!\n");
        return false;
    }

    unsigned int pot_index = std::atoi(buffer[0].c_str());
    if (pot_index >= joystick_nPots) {
        debug_printf("UART- Wrong joystick index %s!\n", buffer[0].c_str());
        return false;
    }

    num_t pot_value = std::atof(buffer[1].c_str()) * u16_float_scaling;
    if (pot_value > 1.00001f || pot_value < -0.00001f) {
        debug_printf("UART- Wrong joystick value %s!\n", buffer[1].c_str());
        return false;
    }

    meml_interface->SetPot(static_cast<te_joystick_pot>(pot_index), pot_value);

    //xscope_float(pot_index, pot_value);

    return true;
}

bool MEML_UART::_ParseButton(std::vector<std::string> &buffer)
{
    if (buffer.size() != 2) {
        debug_printf("UART- Wrong buffer for button parse!\n");
        return false;
    }

    unsigned int btn_index = std::atoi(buffer[0].c_str());
    if (btn_index >= button_nButtons) {
        debug_printf("UART- Wrong buttom index %s!\n", buffer[0].c_str());
        return false;
    }

    unsigned int btn_value = std::atoi(buffer[1].c_str());
    if (btn_value != 0 && btn_value != 1) {
        debug_printf("UART- Wrong button value %s!\n", buffer[1].c_str());
        return false;
    }
    bool btn_value_bool = !static_cast<bool>(btn_value);

    if (btn_index == 0) { // Toggle
        meml_interface->SetToggleButton(static_cast<te_button_idx>(btn_index), btn_value_bool);
    } else {  // Buttons
        if (btn_value_bool && !button_states_[btn_index]) {
            // Pressed
            meml_interface->SetToggleButton(static_cast<te_button_idx>(btn_index), btn_value_bool);
        }
    }
    button_states_[btn_index] = btn_value_bool;

    return true;
}

bool MEML_UART::ParseAndSend(std::vector<std::string> &buffer)
{
    if (!buffer.size()) {
        debug_printf("UART- buffer empty\n");
        return false;
    }
    std::string first_token = buffer[0];
    std::vector<std::string> payload(buffer.begin()+1, buffer.end());

    const char switch_token = first_token.back();
    switch (switch_token) {
        case 'j': {
            _ParseJoystick(payload);
        } break;
        case 'b': {
            _ParseButton(payload);
        } break;
        default: {
            debug_printf("UART- message type %c unknown", switch_token);
            return false;
        }
    }

    return true;
}

///
// C WRAPPER TASK
///

void uart_rx_task(uart_rx_t* uart_rx_ctx)
{
    auto uart_if = std::make_unique<MEML_UART>(uart_rx_ctx);
    debug_printf("UART- Initialised UART RX\n");

    while(1) {
        std::vector<std::string> message;

        // Process characters from the UART interface
        uart_if->Process();

        // ...until a message is received in full
        if (uart_if->GetMessage(message)) {
            
            // Parse message (and displatch to interface internally)
            if (uart_if->ParseAndSend(message)) {
                //debug_printf("UART- Message passed to Interface.\n");
            }
        }
    }
}


void uart_tx_task(uart_tx_t* uart_tx_ctx)
{
    return;
}
