#ifndef _SINE_TONE_WRAPPER_HPP_
#define _SINE_TONE_WRAPPER_HPP_

extern "C" {

#include "app_conf.h"

#if SINE_TEST
    #include <stdint.h>

    void sine_tone_init(float sample_rate, float freq);
    int32_t sine_tone_generate(void);

#endif  // SINE_TEST

}

#endif  // _SINE_TONE_WRAPPER_HPP_
