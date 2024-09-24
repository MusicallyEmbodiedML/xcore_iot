// Copyright (c) 2021-2022 XMOS LIMITED. This Software is subject to the terms of the
// XMOS Public License: Version 1

/* App headers */
#include "../app_conf.h"
#include "platform_init.h"


void platform_init_tile_1(chanend_t c_other_tile)
{
    memset(tile1_ctx, 0, sizeof(tile1_ctx_t));

    tile1_ctx->c_from_gpio = soc_channel_establish(c_other_tile, soc_channel_input);
    tile1_ctx->c_to_gpio = soc_channel_establish(c_other_tile, soc_channel_output);
}
