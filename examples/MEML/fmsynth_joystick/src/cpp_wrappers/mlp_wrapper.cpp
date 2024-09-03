#include "mlp_wrapper.hpp"

// C++ includes
#include <memory>
#include <algorithm>
#include <cmath>
// XMOS includes
extern "C" {
#include <xcore/triggerable.h>
#include <xcore/channel.h>
#include <xcore/select.h>
#include "xcore_utils.h"
#include <stdio.h>
}
// Local includes
#include "chans_and_data.h"
#include "MLP.h"
#include "FMSynth.hpp"


// Static dataset

//static constexpr unsigned int kN_examples = 10;

static std::vector<std::vector<float>> features;//(kN_examples);
static std::vector<std::vector<float>> labels;//(kN_examples);


void Dataset::Add(std::vector<float> &feature, std::vector<float> &label)
{
    auto feature_local = feature;
    auto label_local = label;
    features.push_back(feature_local);
    labels.push_back(label_local);
    debug_printf("MLP- Feature size %d, label size %d.\n", features.size(), labels.size());
}

// Static model

static const unsigned int kBias = 1;
static const std::vector<size_t> layers_nodes = {
    sizeof(ts_joystick_read)/sizeof(num_t) + kBias,
    10, 10, 14,
    kN_synthparams
};
static const std::vector<std::string> layers_activfuncs = {
    "relu", "relu", "relu", "sigmoid"
};
static const bool use_constant_weight_init = false;
static const num_t constant_weight_init = 0;
static MLP<num_t> mlp(
    layers_nodes,
    layers_activfuncs,
    "mse",
    use_constant_weight_init,
    constant_weight_init
);


void Dataset::Train()
{
    MLP<float>::training_pair_t dataset(features, labels);

    debug_printf("MLP- Feature size %d, label size %d.\n", dataset.first.size(), dataset.second.size());
    debug_printf("MLP- Feature dim %d, label dim %d.\n", dataset.first[0].size(), dataset.second[0].size());
    debug_printf("MLP- Training...\n");
    mlp.Train(dataset,
              1.,
              1000,
              0.0001,
              false);
    debug_printf("MLP- Trained.\n");
}

///
// Pretty printing debug helper
///


static constexpr unsigned int CHAR_BUFF_SIZE = 16;
static constexpr unsigned int kN_decimals = 6;


static char * _float_to_char(float x, char *p) {
    static const int ten_to_the_n_decimals = std::pow(10, kN_decimals);

    char *s = p + CHAR_BUFF_SIZE - 1; // go to end of buffer
    uint16_t decimals;  // variable to store the decimals
    int units;  // variable to store the units (part to left of decimal place)
    if (x < 0) { // take care of negative numbers
        decimals = (int)(x * -ten_to_the_n_decimals) % ten_to_the_n_decimals;
        units = (int)(-1 * x);
    } else { // positive numbers
        decimals = (int)(x * ten_to_the_n_decimals) % ten_to_the_n_decimals;
        units = (int)x;
    }

    for (unsigned int n = 0; n < kN_decimals; n++) {
        *--s = (decimals % 10) + '0';
        decimals /= 10; // repeat for as many decimal places as you need
    }
    *--s = '.';

    do {
        *--s = (units % 10) + '0';
        units /= 10;
    } while (units > 0);
    if (x < 0) *--s = '-'; // unary minus sign for negative numbers
    return s;
}



static void PrintVector(std::vector<float> &v) {
    debug_printf("[ ", v.size());
    unsigned int howmany = 0;
    for (auto &el : v) {
        char str_buffer[CHAR_BUFF_SIZE] { 0 };
        char *actual_buf = _float_to_char(el, str_buffer);
        debug_printf("%s, ", actual_buf);
        howmany++;
    }
    if (!howmany) {
        debug_printf("Empty vector, size %d ", v.size());
    };
    debug_printf("],\n");
}

static void PrintNode(Node<float> &n) {
    PrintVector(n.m_weights);
}

static void PrintLayer(Layer<float> &l) {
    debug_printf("[ ");
    for(auto &n : l.m_nodes) {
        PrintNode(n);
    }
    debug_printf("],\n");
}

static void PrintModel(MLP<float> &m) {
    debug_printf("\"model\": {");
    for(auto &l : m.m_layers) {
        PrintLayer(l);
    }
    debug_printf("},\n");
}

static void PrintMember(const char *name,
        std::vector<std::vector<float>  > &fvec) {
    debug_printf("\"%s\": {", name);
    for(auto &f : fvec) {
        PrintVector(f);
    }
    debug_printf("},\n");
}

static void PrintDataset() {
    debug_printf("\"dataset\": {");
    PrintMember("features", features);
    PrintMember("labels", labels);
    debug_printf("},\n");;
}

void DebugDumpJSON() {
    PrintDataset();
    PrintModel(mlp);
}

///
// \end Pretty printing debug helper
///


void mlp_task(chanend_t dispatcher_nn,
              chanend_t nn_paramupdate,
              chanend_t nn_data,
              chanend_t nn_train)
{
    // Init
    auto joystick_read = std::make_unique<ts_joystick_read>();

    // NN set/load weights
    //TODO AM
    //for (unsigned int l=0; l < mlp->GetNumLayers(); l++) {
    //    mlp->SetLayerWeights(<TODO>);
    //}
#if 0
    SELECT_RES(
        CASE_THEN(nn_paramupdate, on_nn_paramupdate)
        //CASE_THEN(nn_data, on_nn_data),
        //CASE_THEN(nn_train, on_nn_train) 
    ) {

        on_nn_paramupdate: {
            debug_printf("MLP- Bark\n");
            chan_in_buf_byte(
                dispatcher_nn,
                reinterpret_cast<uint8_t *>(joystick_read.get()),
                sizeof(ts_joystick_read)
            );
        } continue;
/*
        on_nn_data: {
            debug_printf("MLP- Bork\n");
        } continue;

        on_nn_train: {
            debug_printf("MLP- Woof\n");
        } continue;
*/
    }  // SELECT_RES

    // Events
#else
    while (true) {
        //debug_printf("NN- Waiting for data...\n");
        // Blocking acquisition of pot data from dispatcher_nn
        chan_in_buf_byte(
            dispatcher_nn,
            reinterpret_cast<uint8_t *>(joystick_read.get()),
            sizeof(ts_joystick_read)
        );
        //debug_printf("NN- Received joystick read in NN task.\n");

        // Instantiate data in/out
        std::vector<num_t> input{
            joystick_read->potX,
            joystick_read->potY,
            joystick_read->potRotate,
            1.f  // bias
        };
        std::vector<num_t> output(kN_synthparams);
        // Run model
        mlp.GetOutput(input, &output);

        // Send result
    #if 1
        chan_out_buf_byte(
            nn_paramupdate,
            reinterpret_cast<uint8_t *>(output.data()),
            sizeof(num_t) * kN_synthparams
        );
    #endif

    }  // while(true)
#endif

}
