// Copyright (c) 2021 XMOS LIMITED. This Software is subject to the terms of the
// XMOS Public License: Version 1

#ifndef PLATFORM_INIT_H_
#define PLATFORM_INIT_H_

/* System headers */
#include <platform.h>
#include <xcore/chanend.h>

/* Platform headers */
#include "soc.h"
#include "xcore_utils.h"

/* App headers */
#include "tile_support.h"
#include "app_pll_ctrl.h"
#include "aic3204.h"

/** TILE 0 Clock Blocks */
#define MCLK_CLKBLK     XS1_CLKBLK_3
// #define UNUSED_CLKBLK     XS1_CLKBLK_4
// #define UNUSED_CLKBLK   XS1_CLKBLK_5

/** TILE 1 Clock Blocks */
// #define UNUSED_CLKBLK   XS1_CLKBLK_4
// #define UNUSED_CLKBLK   XS1_CLKBLK_5

void platform_init_tile_0(chanend_t c_other_tile);
void platform_init_tile_1(chanend_t c_other_tile);

#endif /* PLATFORM_INIT_H_ */
