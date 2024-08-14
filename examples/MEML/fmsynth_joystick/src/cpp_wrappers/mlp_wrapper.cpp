#include "mlp_wrapper.hpp"

// C++ includes
#include <memory>
// XMOS includes
extern "C" {
#include <xcore/triggerable.h>
#include <xcore/channel.h>
}
// Local includes
#include "Data.h"
#include "MLP.h"
#include "FMSynth.hpp"


/**
 * @brief Inserts
 * 
 */
typedef struct {
    num_t potX;
    num_t potY;
    num_t potRotate;
} ts_joystick_read;


void mlp_task(chanend_t dispatcher_nn, chanend_t nn_paramupdate)
{
    // Constants/parameters
    const unsigned int kBias = 1;
    // Init
    auto joystick_read = std::make_unique<ts_joystick_read>();

    // NN init
    const std::vector<unsigned int> layers_nodes = {
        sizeof(ts_joystick_read)/sizeof(num_t) + kBias,
        10, 10, 14,
        kN_synthparams
    };
    const std::vector<std::string> layers_activfuncs = {
        "relu", "relu", "relu", "tanh"
    };
    const bool use_constant_weight_init = false;
    const num_t constant_weight_init = 0;
    auto mlp = std::make_unique< MLP<num_t> >(
        layers_nodes,
        layers_activfuncs,
        "mse",
        use_constant_weight_init,
        constant_weight_init
    );
    // NN set/load weights
    //TODO AM
    //for (unsigned int l=0; l < mlp->GetNumLayers(); l++) {
    //    mlp->SetLayerWeights(<TODO>);
    //}

    // Events
    while (true) {
        // Blocking acquisition of pot data from dispatcher_nn
        chan_in_buf_byte(
            dispatcher_nn,
            reinterpret_cast<uint8_t *>(joystick_read.get()),
            sizeof(ts_joystick_read)
        );

        // Instantiate data in/out
        std::vector<num_t> input{
            joystick_read->potX,
            joystick_read->potY,
            joystick_read->potRotate,
            1.f  // bias
        };
        std::vector<num_t> output(sizeof(kN_synthparams));
        // Run model
        mlp->GetOutput(input, &output);

        // Send result
        chan_out_buf_byte(
            nn_paramupdate,
            reinterpret_cast<uint8_t *>(output.data()),
            sizeof(num_t) * output.size()
        );
    }
}