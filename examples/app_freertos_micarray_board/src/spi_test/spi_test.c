// Copyright (c) 2019, XMOS Ltd, All rights reserved

#include <string.h>
#include "app_conf.h"

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Library headers */
#include "soc.h"

/* BSP/bitstream headers */
#include "bitstream_devices.h"
#include "spi_master_driver.h"

/* App headers */
#include "spi_test.h"
#include "debug_print.h"

    /* Test Params */
#define        test_interval_ms   100
#define        tx_len             8
static uint8_t test_msg[tx_len] = { 0xDE, 0xAD, 0xBE, 0xEF,
                                    0xFE, 0xED, 0xFA, 0xCE};

static void spi_test(void *arg)
{
    soc_peripheral_t spi_dev = arg;
    uint8_t *tx_buf;
    uint8_t *rx_buf;
    int test = 0;

    spi_master_device_init(spi_dev,
                           appconfSPI_CS_PORT_BIT,
                           appconfSPI_CPOL,
                           appconfSPI_CPHA,
                           appconfSPI_CLOCKDIV,
                           appconfSPI_CS_TO_DATA_DELAY_NS,
                           appconfSPI_BYTE_SETUP_NS);

    uint8_t* tmpbuf;
    soc_dma_ring_buf_t *tx_ring_buf = soc_peripheral_tx_dma_ring_buf(spi_dev);

    hwtimer_t timer;
    uint32_t time;
    uint32_t start;
    hwtimer_alloc(&timer);

    for(;;)
    {
        vTaskDelay( pdMS_TO_TICKS( test_interval_ms ) );

        switch(test++)
        {
        default:
        case 0:
            debug_printf("SPI Test Start\n");
            break;
        case 1: /* transmit only */
//            tx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );
//            memcpy(tx_buf, test_msg , sizeof(uint8_t)*tx_len);
//
//            hwtimer_get_time(timer, &start);
//            spi_transmit(spi_dev, tx_buf, tx_len );
//            hwtimer_get_time(timer, &time);
//
////            debug_printf("\tSent:\n");
////            for(int i=0; i<tx_len; i++)
////            {
////                debug_printf("\ttx[%d]:%x\n", i, tx_buf[i]);
////            }
//
//            /* Cleanup tx */
//            while( ( tmpbuf = soc_dma_ring_tx_buf_get( tx_ring_buf, NULL, NULL ) ) != tx_buf ) {;}
//            vPortFree(tmpbuf);
//
//            debug_printf("Transmit Test Complete: %d\n", time - start);
            break;
        case 2: /* receive only */
//            rx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );
//
//            hwtimer_get_time(timer, &start);
//            spi_request(spi_dev, rx_buf, tx_len );
//            hwtimer_get_time(timer, &time);
//
//            xQueueReceive(queue, &rx_buf, portMAX_DELAY);
//
////            debug_printf("\tReceived:\n");
////            for(int i=0; i<tx_len; i++)
////            {
////                debug_printf("\trx[%d]:%x\n", i, rx_buf[i]);
////            }
//
//            /* Cleanup rx */
//            if(rx_buf != NULL)
//            {
//                vPortFree(rx_buf);
//            }
//
//            debug_printf("Receive Test Complete: %d\n", time - start);
            break;
        case 3: /* transmit only blocking */
            tx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );
            memcpy(tx_buf, test_msg , sizeof(uint8_t)*tx_len);

            hwtimer_get_time(timer, &start);
//            spi_transmit_blocking(spi_dev, tx_buf, tx_len );
            spi_transaction(spi_dev,
                            NULL, 0,
                            tx_buf, tx_len);
            hwtimer_get_time(timer, &time);

//            debug_printf("\tSent:\n");
//            for(int i=0; i<tx_len; i++)
//            {
//                debug_printf("\ttx[%d]:%x\n", i, tx_buf[i]);
//            }

            /* Cleanup tx */
            if(tx_buf != NULL)
            {
                vPortFree(tx_buf);
            }

            debug_printf("Transmit Blocking Test Complete: %d\n", time - start);
            break;
        case 4: /* receive only blocking */
            rx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );

            hwtimer_get_time(timer, &start);
//            spi_request_blocking(spi_dev, rx_buf, tx_len );
            spi_transaction(spi_dev,
                            rx_buf, tx_len,
                            NULL, 0);
            hwtimer_get_time(timer, &time);

//            debug_printf("\tReceived:\n");
//            for(int i=0; i<tx_len; i++)
//            {
//                debug_printf("\trx[%d]:%x\n", i, rx_buf[i]);
//            }

            /* Cleanup rx */
            if(rx_buf != NULL)
            {
                vPortFree(rx_buf);
            }

            debug_printf("Receive Blocking Test Complete: %d\n", time - start);
            break;
        case 5: /* transaction */
//            tx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );
//            rx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );
//
//            memcpy(tx_buf, test_msg , sizeof(uint8_t)*tx_len);
//
//            hwtimer_get_time(timer, &start);
////            spi_transaction(spi_dev, rx_buf, tx_buf, tx_len );
//            spi_transaction(spi_dev,
//                            rx_buf, tx_len,
//                            tx_buf, tx_len);
//            hwtimer_get_time(timer, &time);
//
////            debug_printf("\tSent:\n");
////            for(int i=0; i<tx_len; i++)
////            {
////                debug_printf("\ttx[%d]:%x\n", i, tx_buf[i]);
////            }
//
//            xQueueReceive(queue, &rx_buf, portMAX_DELAY);
//
////            debug_printf("\tReceived:\n");
////            for(int i=0; i<tx_len; i++)
////            {
////                debug_printf("\trx[%d]:%x\n", i, rx_buf[i]);
////            }
//
//            /* Cleanup rx */
//            if(rx_buf != NULL)
//            {
//                vPortFree(rx_buf);
//            }
//
//            /* Cleanup tx */
//            while( ( tmpbuf = soc_dma_ring_tx_buf_get( tx_ring_buf, NULL, NULL ) ) != tx_buf ) {;}
//            vPortFree(tmpbuf);
//
//            debug_printf("Transaction Test Complete: %d\n", time - start);
            break;
        case 6: /* transaction blocking */
            tx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );
            rx_buf = (uint8_t*)pvPortMalloc( sizeof(uint8_t)*tx_len );

            memcpy(tx_buf, test_msg , sizeof(uint8_t)*tx_len);

            hwtimer_get_time(timer, &start);
//            spi_transaction_blocking(spi_dev, rx_buf, tx_buf, tx_len );
            spi_transaction(spi_dev,
                            rx_buf, tx_len,
                            tx_buf, tx_len);
            hwtimer_get_time(timer, &time);

//            debug_printf("\tSent:\n");
//            for(int i=0; i<tx_len; i++)
//            {
//                debug_printf("\ttx[%d]:%x\n", i, tx_buf[i]);
//            }
//
//            debug_printf("\tReceived:\n");
//            for(int i=0; i<tx_len; i++)
//            {
//                debug_printf("\trx[%d]:%x\n", i, rx_buf[i]);
//            }

            /* Cleanup rx */
            if(rx_buf != NULL)
            {
                vPortFree(rx_buf);
            }

            /* Cleanup tx */
            if(tx_buf != NULL)
            {
                vPortFree(tx_buf);
            }

            debug_printf("Transaction Blocking Test Complete: %d\n", time - start);
            break;
        case 7: /* end */
            test = 0;
            debug_printf("SPI Test Finished\n");
            break;
        }
    }
}



void spi_test_create( UBaseType_t priority )
{

    soc_peripheral_t dev;

    dev = spi_master_driver_init(
            BITSTREAM_SPI_DEVICE_A,  /* Initializing SPI device A */
            1,                       /* Only need 1 DMA buffer for each direction */
            0);                      /* This device's interrupts should happen on core 0 */

    xTaskCreate(spi_test, "spi_test", portTASK_STACK_DEPTH(spi_test), dev, priority, NULL);
}
