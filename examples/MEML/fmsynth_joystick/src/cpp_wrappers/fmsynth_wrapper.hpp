#ifndef _FMNSYNTH_WRAPPER_HPP_
#define _FMNSYNTH_WRAPPER_HPP_

#define TEST_WITH_SINE    1

#include "FMSynth.hpp"

extern "C" {

#include <xcore/chanend.h>
#include "app_conf.h"

#include <stdint.h>

void fmsynth_init(float sample_rate);
int32_t fmsynth_generate(void);
#if TEST_WITH_SINE
int32_t sinetone_generate(void);
#endif

void fmsynth_paramupdate_task(chanend_t nn_paramupdate);

}

extern FMSynth *fmsyn;

#endif  // _FMNSYNTH_WRAPPER_HPP_
