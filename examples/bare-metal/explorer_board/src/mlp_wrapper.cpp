#include "mlp_wrapper.hpp"

#if MLP_TEST

#include "MLP.h"
#include "app_conf.h"
#include <xcore/hwtimer.h>
#include <stdio.h>
#include <vector>


void run_mlp_test(void) {
    printf("===\n");
    printf("MLP test\n");
    printf("===\n");
    {
        // Test set up
        std::vector<TrainingSample<float>> training_set =
        {
            { { 0, 0 },{ 0.0 } },
            { { 0, 1 },{ 1.0 } },
            { { 1, 0 },{ 0.0 } },
            { { 1, 1 },{ 1.0 } }
        };
        bool bias_already_in = false;
        std::vector<TrainingSample<float>> training_sample_set_with_bias(training_set);
        //set up bias
        if (!bias_already_in) {
            for (auto & training_sample_with_bias : training_sample_set_with_bias) {
            training_sample_with_bias.AddBiasValue(1);
            }
        }

        size_t num_features = training_sample_set_with_bias[0].GetInputVectorSize();
        size_t num_outputs = training_sample_set_with_bias[0].GetOutputVectorSize();
        MLP<float> my_mlp({ num_features, 2 ,num_outputs }, { "sigmoid", "linear" });
        unsigned int epochs = 500;

        // Timer init
        auto timer = hwtimer_alloc();
        printf("Benchmark starting (%d epochs)...\n", epochs);
        auto now = hwtimer_get_time(timer);

        // Benchmarked code
        my_mlp.Train(training_sample_set_with_bias, 0.5, epochs, 0, false);

        // Timer benchmark
        auto t2 = hwtimer_get_time(timer);
        printf("start time: %lf\n", now * 1e-8);
        printf("end time: %lf\n", t2 * 1e-8);
        printf("total time: %lf\n", (t2-now) * 1e-8);
    }
    printf("===\n");

#if MLP_PARALLEL
    while(1) {;}
#endif 
}

#endif  // MLP_TEST
