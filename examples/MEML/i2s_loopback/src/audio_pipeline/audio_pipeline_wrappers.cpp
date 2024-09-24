#include "maximilian.h"

extern "C" {
    float dl_left_process(float);
    float dl_right_process(float);
}


static maxiDelayline dl_left;
static maxiDelayline dl_right;

static int dl_size_left = 48000/4;
static int dl_size_right = static_cast<int>(
    static_cast<float>(dl_size_left) * (4.f/3.f));

float dl_left_process(float x)
{
    return dl_left.dl(x, dl_size_left, 0.3);
}

float dl_right_process(float x)
{
    return dl_right.dl(x, dl_size_right, 0.3);
}
