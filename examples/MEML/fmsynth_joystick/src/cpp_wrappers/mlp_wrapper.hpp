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
void mlp_task(chanend_t dispatcher_nn,
              chanend_t nn_paramupdate,
              chanend_t nn_data,
              chanend_t nn_train);

}  // extern "C"


#include <vector>

class Dataset {
 public:
    static void Add(std::vector<float> &feature, std::vector<float> &label);
    static void Train();
    static void Dump();
};

void DebugDumpJSON();

#endif  // __MLP_WRAPPER_HPP__