#ifndef _MLP_WRAPPER_HPP_
#define _MLP_WRAPPER_HPP_



extern "C" {
#include "app_conf.h"

#if MLP_TEST
    void run_mlp_test(void);

#if MLP_PARALLEL
    #include <xcore/parallel.h>

    DECLARE_JOB(run_mlp_test, (void));
#endif  // MLP_PARALLEL

#endif
}


#endif  // _MLP_WRAPPER_HPP_
