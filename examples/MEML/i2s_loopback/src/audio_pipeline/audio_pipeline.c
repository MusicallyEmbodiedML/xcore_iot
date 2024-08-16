// Copyright 2021-2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* System headers */
#include <platform.h>
#include <xs1.h>
#include <string.h>
#include <xcore/triggerable.h>
#include <xcore/hwtimer.h>
#include <xcore/interrupt.h>

#include <stdbool.h>

/* Platform headers */
#include "xcore_utils.h"
#include "mic_array.h"
#include "xmath/xmath.h"

/* App headers */
#include "app_conf.h"
#include "audio_pipeline.h"

static bool triggered_stage_a = false;
static bool triggered_stage_b = false;
static bool triggered_stage_c = false;
static bool triggered_send = false;

//#include <hwtimer.h>
void ap_stage_a(chanend_t c_input, chanend_t c_output) {
    // initialise the array which will hold the data
    int32_t DWORD_ALIGNED input [appconfAUDIO_FRAME_LENGTH][appconfMIC_COUNT];
    int32_t DWORD_ALIGNED output [appconfMIC_COUNT][appconfAUDIO_FRAME_LENGTH];

    triggerable_disable_all();
    // initialise event
    TRIGGERABLE_SETUP_EVENT_VECTOR(c_input, input_frames);

    triggerable_enable_trigger(c_input);    

    while(1)
    {
        TRIGGERABLE_WAIT_EVENT(input_frames);
        {
            input_frames:
            {
                if (!triggered_stage_a){
                    debug_printf("triggered stage a\n");
                    triggered_stage_a = true;
                }        
                // get the frame from the i2s input
                s_chan_in_buf_word(c_input, (uint32_t*) input, appconfFRAMES_IN_ALL_CHANS);

                // change the frame format to [channel][sample]
                for(int ch = 0; ch < appconfMIC_COUNT; ch ++){
                    for(int smp = 0; smp < appconfAUDIO_FRAME_LENGTH; smp ++){
                        output[ch][smp] = input[smp][ch];
                    }
                }
                // send the frame to the next stage
                s_chan_out_buf_word(c_output, (uint32_t*) output, appconfFRAMES_IN_ALL_CHANS);
            }
            continue;
        }
    }
}

void ap_stage_b(chanend_t c_input, chanend_t c_output, chanend_t c_from_gpio) {

    // initialise the array which will hold the data
    int32_t DWORD_ALIGNED output[appconfMIC_COUNT][appconfAUDIO_FRAME_LENGTH];
    // initialise block floating point structures for both channels
    bfp_s32_t ch0, ch1;
    bfp_s32_init(&ch0, output[0], appconfEXP, appconfAUDIO_FRAME_LENGTH, 0);
    bfp_s32_init(&ch1, output[1], appconfEXP, appconfAUDIO_FRAME_LENGTH, 0);

    int gain_db = appconfINITIAL_GAIN;

    triggerable_disable_all();
    // initialise events
    TRIGGERABLE_SETUP_EVENT_VECTOR(c_input, input_frames);
    TRIGGERABLE_SETUP_EVENT_VECTOR(c_from_gpio, gpio_request);

    triggerable_enable_trigger(c_input);
    triggerable_enable_trigger(c_from_gpio);

    while(1)
    {
        TRIGGERABLE_WAIT_EVENT(input_frames, gpio_request);
        {
            input_frames:
            {
                if (!triggered_stage_b){
                    debug_printf("triggered stage b\n");
                    triggered_stage_b = true;
                }
                // recieve frame over the channel
                s_chan_in_buf_word(c_input, (uint32_t*) output, appconfFRAMES_IN_ALL_CHANS);
                // calculate the headroom of the new frames
                bfp_s32_headroom(&ch0);
                bfp_s32_headroom(&ch1);
                // update the gain
                float power = (float)gain_db / 20.0;
                float gain_fl = powf(10.0, power);
                float_s32_t gain = f32_to_float_s32(gain_fl);
                // scale both channels 
                bfp_s32_scale(&ch0, &ch0, gain);
                bfp_s32_scale(&ch1, &ch1, gain);
                // normalise exponent
                bfp_s32_use_exponent(&ch0, appconfEXP);
                bfp_s32_use_exponent(&ch1, appconfEXP);
                // send frame over the channel
                s_chan_out_buf_word(c_output, (uint32_t*) output, appconfFRAMES_IN_ALL_CHANS);
            }
            continue;
        }
        {
            gpio_request:
            {
                char msg = chanend_in_byte(c_from_gpio);
                switch(msg)
                {
                default:
                    break;
                case 0x01:  /* Btn A */
                    gain_db = (gain_db >= appconfAUDIO_PIPELINE_MAX_GAIN) ? gain_db : gain_db + appconfAUDIO_PIPELINE_GAIN_STEP;
                    break;
                case 0x02:  /* Btn B */
                    gain_db = (gain_db <= appconfAUDIO_PIPELINE_MIN_GAIN) ? gain_db : gain_db - appconfAUDIO_PIPELINE_GAIN_STEP;
                    break;
                }
                debug_printf("Gain set to %d\n", gain_db);
            }
            continue;
        }
    }
}

void ap_stage_c(chanend_t c_input, chanend_t c_output, chanend_t c_to_gpio) {
    bool new_frame = true;
    int32_t DWORD_ALIGNED input[appconfMIC_COUNT][appconfAUDIO_FRAME_LENGTH];
    int32_t DWORD_ALIGNED output[appconfAUDIO_FRAME_LENGTH][appconfMIC_COUNT];
    // initialise block floating point structures for both channels
    bfp_s32_t ch0, ch1;
    bfp_s32_init(&ch0, input[0], appconfEXP, appconfAUDIO_FRAME_LENGTH, 0);
    bfp_s32_init(&ch1, input[1], appconfEXP, appconfAUDIO_FRAME_LENGTH, 0);
    
    // Create a dummy frame to initialise i2s on first run
    for(int ch = 0; ch < appconfMIC_COUNT; ch ++){
        for(int smp = 0; smp < appconfAUDIO_FRAME_LENGTH; smp ++){
            output[smp][ch] = 0;
        }
    }    
    uint32_t time_now = get_reference_time();
    while(get_reference_time() < (time_now + 100000)); // Wait for a millisecond
    // send frame over the channel
    s_chan_out_buf_word(c_output, (uint32_t*) output, appconfFRAMES_IN_ALL_CHANS);
    if (!triggered_send){
        debug_printf("triggered sending to i2s\n");
        triggered_send = true;
    }    

    triggerable_disable_all();
    // initialise event
    TRIGGERABLE_SETUP_EVENT_VECTOR(c_input, input_frames);

    triggerable_enable_trigger(c_input);

    while(1)
    {
        TRIGGERABLE_WAIT_EVENT(input_frames);
        {
            input_frames:
            {
                if (!triggered_stage_c){
                    debug_printf("triggered stage c\n");
                    triggered_stage_c = true;
                }
                uint8_t led_byte = 0;
                // recieve frame over the channel
                s_chan_in_buf_word(c_input, (uint32_t*) input, appconfFRAMES_IN_ALL_CHANS);
                // calculate the headroom of the new frames
                bfp_s32_headroom(&ch0);
                bfp_s32_headroom(&ch1);
                // calculate the frame energy
                float_s32_t frame_energy_ch0 = float_s64_to_float_s32(bfp_s32_energy(&ch0));
                float_s32_t frame_energy_ch1 = float_s64_to_float_s32(bfp_s32_energy(&ch1));
                // calculate the frame power
                float frame_pow0 = float_s32_to_float(frame_energy_ch0) / (float)appconfAUDIO_FRAME_LENGTH;
                float frame_pow1 = float_s32_to_float(frame_energy_ch1) / (float)appconfAUDIO_FRAME_LENGTH;
                if((frame_pow0 > appconfPOWER_THRESHOLD) || (frame_pow1 > appconfPOWER_THRESHOLD)){
                    led_byte = 1;
                }
                // send led value to gpio
                chanend_out_byte(c_to_gpio, led_byte);
                // change the array format to [sample][channel]
                for(int ch = 0; ch < appconfMIC_COUNT; ch ++){
                    for(int smp = 0; smp < appconfAUDIO_FRAME_LENGTH; smp ++){
                        output[smp][ch] = input[ch][smp];
                    }
                }
                // send frame over the channel
                s_chan_out_buf_word(c_output, (uint32_t*) output, appconfFRAMES_IN_ALL_CHANS);                
                //new_frame = true;
            }
            continue;
        }
    }
}