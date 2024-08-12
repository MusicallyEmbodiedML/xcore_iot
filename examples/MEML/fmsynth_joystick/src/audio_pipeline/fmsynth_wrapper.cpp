#define TEST_WITH_SINE    0

#include "fmsynth_wrapper.hpp"


#include "FMSynth.hpp"
#if TEST_WITH_SINE
#include "SineTone.hpp"
#endif
#include <cmath>
#include <new>

#if TEST_WITH_SINE
static SineTone *sinetone = nullptr;
static char sinetone_mem_[sizeof(SineTone)];
#else
static FMSynth *fmsyn = nullptr;
static char fmsyn_mem_[sizeof(FMSynth)];
#endif

//#pragma stackfunction 0
void fmsynth_init(float sample_rate) {
#if TEST_WITH_SINE
    sinetone = new (sinetone_mem_) SineTone(sample_rate, 440.);
#else
    fmsyn = new (fmsyn_mem_) FMSynth(sample_rate);
#endif
}


//#pragma stackfunction 2
int32_t fmsynth_generate(void) {
    int32_t y = 0;
#if TEST_WITH_SINE
    if (sinetone != nullptr) {
        y = sinetone->processInt();
    }
#else
    if (fmsyn != nullptr) {
        y = fmsyn->processInt();
    }
#endif
    return y;
}


// #endif  // SINE_TEST