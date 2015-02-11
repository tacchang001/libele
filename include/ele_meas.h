#pragma once

#include <stdint.h>
#include <time.h>

#include "ele_error.h"

#ifndef ELE_MES_DATA_BLOCKS
#define ELE_MES_DATA_BLOCKS 4
#endif

/**
 *
 */
void ele_meas_clock(uint32_t[ELE_MES_DATA_BLOCKS]);
