// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* App headers */
#include "app_conf.h"
#include "app_demos.h"
#include "burn.h"
#include "audio_pipeline.h"
#include "platform_init.h"
#include "c_wrapper_decl.h"


void main_tile0(chanend_t c_gpio, chanend_t c_nn_paramupdate)
{
    // Joystick coordinates channel
    channel_t chan_dispatcher_nn = chan_alloc();
    // Data points channel
    channel_t chan_interface_nn_data = chan_alloc();
    // Training event channel
    channel_t chan_interface_nn_train = chan_alloc();

    platform_init_tile_0(c_gpio, c_nn_paramupdate);

    interface_init(
        chan_dispatcher_nn.end_a,
        tile0_ctx->c_nn_paramupdate,
        chan_interface_nn_data.end_a,
        chan_interface_nn_train.end_a);

    PAR_JOBS (
        PJOB(gpio_server, (tile0_ctx->c_from_gpio, tile0_ctx->c_to_gpio)),
        PJOB(uart_rx_task, (&tile0_ctx->uart_rx_ctx)),
        PJOB(mlp_task, (
            chan_dispatcher_nn.end_b,
            tile0_ctx->c_nn_paramupdate,
            chan_interface_nn_data.end_b,
            chan_interface_nn_train.end_b))
    );
}

void main_tile1(chanend_t c_gpio, chanend_t c_nn_paramupdate)
{
    platform_init_tile_1(c_gpio, c_nn_paramupdate);

    streaming_channel_t s_chan_ac = s_chan_alloc();
    //streaming_channel_t s_chan_bc = s_chan_alloc();
    streaming_channel_t s_chan_output = s_chan_alloc();
    channel_t chan_decoupler = chan_alloc();

    tile1_ctx->c_i2s_to_dac = s_chan_output.end_b;

    PAR_JOBS (
        PJOB(ma_vanilla_task, (chan_decoupler.end_a)),
        PJOB(ap_stage_a, (chan_decoupler.end_b, s_chan_ac.end_a)),
        //PJOB(ap_stage_b, (s_chan_ab.end_b, s_chan_bc.end_a, tile1_ctx->c_from_gpio)),
        PJOB(ap_stage_c, (s_chan_ac.end_b, s_chan_output.end_a, tile1_ctx->c_to_gpio)),
        PJOB(i2s_master, (&tile1_ctx->i2s_cb_group, tile1_ctx->p_i2s_dout, 1, NULL, 0, tile1_ctx->p_bclk, tile1_ctx->p_lrclk, tile1_ctx->p_mclk, tile1_ctx->bclk)),
        PJOB(fmsynth_paramupdate_task, (tile1_ctx->c_nn_paramupdate))
    );
}
