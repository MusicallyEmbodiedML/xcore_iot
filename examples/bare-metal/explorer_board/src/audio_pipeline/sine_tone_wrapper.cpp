#include "sine_tone_wrapper.hpp"

#if SINE_TEST


#include "SineTone.hpp"
#include <cmath>
#include <new>

static SineTone *sine_tone_ = nullptr;
static char sine_tone_mem_[sizeof(SineTone)];


//#pragma stackfunction 0
void sine_tone_init(float sample_rate, float freq) {
    sine_tone_ = new (sine_tone_mem_) SineTone(sample_rate, freq);
}


//#pragma stackfunction 2
int32_t sine_tone_generate(void) {
    int32_t y = 0;
    if (sine_tone_ != nullptr) {
        y = sine_tone_->processInt();
    }
    return y;
}


#endif  // SINE_TEST