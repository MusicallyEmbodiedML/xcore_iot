#include "sine_tone.h"
#include "sine_tone.hpp"
#define _USE_MATH_DEFINES
#include <cmath>
#include <new>

SineTone::SineTone(float sample_rate, float freq) :
    sample_rate_(sample_rate),
    freq_(freq)
{
    w_ = 2.f * M_PI * freq_ / sample_rate;
    phase_ = 0;
}

float SineTone::process()
{
    float y = std::sin(phase_);
    phase_ += w_;
    if (phase_ > 2 * M_PI) {
        phase_ -= 2 * M_PI;
    }
    return y;
}

static SineTone *sine_tone_ = nullptr;
static char sine_tone_mem_[sizeof(SineTone)];

void sine_tone_init(float sample_rate, float freq) {
    sine_tone_ = new (sine_tone_mem_) SineTone(sample_rate, freq);
}

int32_t sine_tone_generate(void) {
    constexpr float scaling = std::pow(2.f, 31.f) - 1.f;
    float y = sine_tone_->process();
    return static_cast<int32_t>(y * scaling);
}
