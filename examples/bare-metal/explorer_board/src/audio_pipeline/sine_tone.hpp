#ifndef _SINE_TONE_H_
#define _SINE_TONE_H_

#include <cstdint>

extern "C" {
    void sine_tone_init(float sample_rate, float freq);
    int32_t sine_tone_generate(void);
}


class SineTone {
 public:
    SineTone(float sample_rate, float freq);
    float process();

 private:
    float sample_rate_;
    float freq_;
    float phase_;
    float w_;

};

#endif  // _SINE_TONE_H_