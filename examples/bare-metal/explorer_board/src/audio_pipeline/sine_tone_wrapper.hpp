#ifndef _SINE_TONE_WRAPPER_HPP_
#define _SINE_TONE_WRAPPER_HPP_

extern "C" {

    #include <stdint.h>

    void sine_tone_init(float sample_rate, float freq);
    int32_t sine_tone_generate(void);

}

#endif  // _SINE_TONE_WRAPPER_HPP_
