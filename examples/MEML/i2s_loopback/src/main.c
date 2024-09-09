// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* App headers */
#include "app_conf.h"
#include "app_demos.h"
#include "burn.h"
#include "audio_pipeline.h"
#include "platform_init.h"


#define AP_SINGLE_STAGE    1


void main_tile0(chanend_t c0, chanend_t c1, chanend_t c2, chanend_t c3)
{
    (void)c0;
    (void)c2;
    (void)c3;

    platform_init_tile_0(c1);

    PAR_JOBS (
        PJOB(spi_demo, (&tile0_ctx->spi_device_ctx)),
        PJOB(gpio_server, (tile0_ctx->c_from_gpio, tile0_ctx->c_to_gpio)),
        PJOB(flash_demo, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ())
    );
}

void main_tile1(chanend_t c0, chanend_t c1, chanend_t c2, chanend_t c3)
{
    (void)c1;
    (void)c2;
    (void)c3;

    platform_init_tile_1(c0);

#if !(AP_SINGLE_STAGE)
    streaming_channel_t s_chan_ab = s_chan_alloc();
    streaming_channel_t s_chan_bc = s_chan_alloc();
#endif
    streaming_channel_t s_chan_i2s_out = s_chan_alloc();
    streaming_channel_t s_chan_i2s_in = s_chan_alloc();

    tile1_ctx->c_i2s_to_dac = s_chan_i2s_out.end_b;
    tile1_ctx->c_adc_to_i2s = s_chan_i2s_in.end_a;

    PAR_JOBS (
        PJOB(burn, ()),
#if !(AP_SINGLE_STAGE)
        PJOB(ap_stage_a, (s_chan_i2s_in.end_b, s_chan_ab.end_a)),
        PJOB(ap_stage_b, (s_chan_ab.end_b, s_chan_bc.end_a, tile1_ctx->c_from_gpio)),
        PJOB(ap_stage_c, (s_chan_bc.end_b, s_chan_i2s_out.end_a, tile1_ctx->c_to_gpio)),
#else
        PJOB(ap_single_stage, (s_chan_i2s_in.end_b, s_chan_i2s_out.end_a)),
#endif
        PJOB(i2s_master, (&tile1_ctx->i2s_cb_group, tile1_ctx->p_i2s_dout, 1, tile1_ctx->p_i2s_din, 1, tile1_ctx->p_bclk, tile1_ctx->p_lrclk, tile1_ctx->p_mclk, tile1_ctx->bclk)),
        PJOB(burn, ()),
        PJOB(burn, ()),
        PJOB(burn, ())
    );
}
