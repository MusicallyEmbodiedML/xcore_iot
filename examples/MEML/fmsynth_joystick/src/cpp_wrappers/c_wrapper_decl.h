/**
 * C++ to C wrapper extern declarations
 */

#ifndef __C_WRAPPER_DECL_H__
#define __C_WRAPPER_DECL_H__

#include <stdint.h>
#include <xcore/chanend.h>
#include <xcore/parallel.h>

// FMSynth
extern void fmsynth_init(float sample_rate);
extern int32_t fmsynth_generate(void);
// MLP Wrapper
//extern void mlp_task(chanend_t dispatcher_nn, chanend_t nn_paramupdate);
DECLARE_JOB(mlp_task, (chanend_t, chanend_t));


#endif  // __C_WRAPPER_DECL_H__
