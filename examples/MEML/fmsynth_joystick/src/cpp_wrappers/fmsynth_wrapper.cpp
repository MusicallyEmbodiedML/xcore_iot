#define TEST_WITH_SINE    0

#include "fmsynth_wrapper.hpp"

#include "chans_and_data.h"
#include "FMSynth.hpp"
#if TEST_WITH_SINE
#include "SineTone.hpp"
#endif
#include <cmath>
#include <new>

extern "C" {
// XMOS includes
#include <xcore/channel.h>
//#include <xscope.h>
//#include "xcore_utils.h"
}  // extern "C"

#if TEST_WITH_SINE
static SineTone *sinetone = nullptr;
static char sinetone_mem_[sizeof(SineTone)];
#else
FMSynth *fmsyn = nullptr;
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

void fmsynth_paramupdate_task(chanend_t nn_paramupdate)
{
    std::vector<num_t> params(kN_synthparams);

    while (true) {
#if 1
        chan_in_buf_byte(
            nn_paramupdate,
            reinterpret_cast<unsigned char *>(params.data()),
            sizeof(num_t) * kN_synthparams
        );

        //debug_printf("FMSynth- Something is received.\n");

        //xscope_float(3, params[0]);
        //xscope_float(4, params[kN_synthparams-1]);

        if (fmsyn != nullptr) {
            fmsyn->mapParameters(params);
            //debug_printf("FMSynth- Params are mapped.\n");
        }
#endif
    }
}

// #endif  // SINE_TEST