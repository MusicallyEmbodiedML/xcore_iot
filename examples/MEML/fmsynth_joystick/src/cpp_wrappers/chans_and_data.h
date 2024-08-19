#ifndef __CHANS_AND_DATA_H__
#define __CHANS_AND_DATA_H__

// MLP types
#include "Data.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Inserts
 * 
 */
typedef struct {
    num_t potX;
    num_t potY;
    num_t potRotate;
} ts_joystick_read;



#ifdef __cplusplus
}
#endif

#endif //__CHANS_AND_DATA_H__