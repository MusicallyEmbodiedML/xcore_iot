#ifndef _FMNSYNTH_WRAPPER_HPP_
#define _FMNSYNTH_WRAPPER_HPP_

#include "FMSynth.hpp"

extern "C" {

#include <xcore/chanend.h>
#include "app_conf.h"

#include <stdint.h>

void fmsynth_init(float sample_rate);
int32_t fmsynth_generate(void);

void fmsynth_paramupdate_task(chanend_t nn_paramupdate);

}

extern FMSynth *fmsyn;

#endif  // _FMNSYNTH_WRAPPER_HPP_
