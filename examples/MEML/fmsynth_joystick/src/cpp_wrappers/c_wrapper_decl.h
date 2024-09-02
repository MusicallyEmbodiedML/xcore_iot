/**
 * C++ to C wrapper extern declarations
 */

#ifndef __C_WRAPPER_DECL_H__
#define __C_WRAPPER_DECL_H__

#include <stdint.h>
#include <xcore/chanend.h>
#include <xcore/parallel.h>
#include "uart.h"

// FMSynth
extern void fmsynth_init(float sample_rate);
extern int32_t fmsynth_generate(void);
// Interface
extern void interface_init(chanend_t, chanend_t, chanend_t, chanend_t);
// Parallel tasks
DECLARE_JOB(mlp_task, (chanend_t, chanend_t, chanend_t, chanend_t));
DECLARE_JOB(uart_rx_task, (uart_rx_t*));
DECLARE_JOB(fmsynth_paramupdate_task, (chanend_t));


#endif  // __C_WRAPPER_DECL_H__
