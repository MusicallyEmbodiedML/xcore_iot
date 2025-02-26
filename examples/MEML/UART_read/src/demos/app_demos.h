// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#ifndef APP_DEMOS_H_
#define APP_DEMOS_H_

/* System headers */
#include <platform.h>
#include <xs1.h>
#include <xcore/parallel.h>
#include <xcore/chanend.h>

/* Platform headers */
#include "xcore_utils.h"
#include "uart.h"

DECLARE_JOB(uart_rx_demo, (uart_rx_t *));

#endif /* APP_CONF_H_ */
