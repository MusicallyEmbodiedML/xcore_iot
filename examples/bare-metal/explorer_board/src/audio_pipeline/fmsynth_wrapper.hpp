#ifndef _FMNSYNTH_WRAPPER_HPP_
#define _FMNSYNTH_WRAPPER_HPP_

extern "C" {

#include "app_conf.h"

// #if SINE_TEST
    #include <stdint.h>

    void fmsynth_init(float sample_rate);
    int32_t fmsynth_generate(void);

// #endif  // SINE_TEST

}

#endif  // _FMNSYNTH_WRAPPER_HPP_
