#include "fmsynth_wrapper.hpp"

// #if SINE_TEST


#include "FMSynth.hpp"
#include <cmath>
#include <new>

static FMSynth *fmsyn = nullptr;
static char fmsyn_mem_[sizeof(FMSynth)];


//#pragma stackfunction 0
void fmsynth_init(float sample_rate) {
    fmsyn = new (fmsyn_mem_) FMSynth(sample_rate);
}


//#pragma stackfunction 2
int32_t fmsynth_generate(void) {
    int32_t y = 0;
    if (fmsyn != nullptr) {
        y = fmsyn->processInt();
    }
    return y;
}


// #endif  // SINE_TEST