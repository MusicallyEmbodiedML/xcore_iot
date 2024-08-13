/**
 * @file mlp_wrapper
 * Wrapper for the MLP C++ library. Maps joystick to FM synth
 * parameters.
 */

#ifndef __MLP_WRAPPER_HPP__
#define __MLP_WRAPPER_HPP__

extern "C" {

#include <xcore/chanend.h>

/**
 * @brief Task to handle pot position to FMsynth parameters.
 * 
 * @param dispatcher_nn Chanend receiving joystick reads from dispatcher.
 * Expects ts_joystick_read being sent to it.
 * @param nn_paramupdate Chanend sending FM parameters to
 * paramupdate. Sends ts_fmsynth_params.
 */
void mlp_task(chanend_t dispatcher_nn, chanend_t nn_paramupdate);

}


#endif  // __MLP_WRAPPER_HPP__