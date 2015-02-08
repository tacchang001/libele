#pragma once

#include <stdint.h>
#include <time.h>

#ifndef ELE_MES_DATA_BLOCKS
#define ELE_MES_DATA_BLOCKS 5
#endif

typedef struct {
	struct timespec lap_time;
	uint32_t v[ELE_MES_DATA_BLOCKS];
} ele_perf_packet_t;

/**
 *
 */
void measure_clock(const struct timespec, uint32_t[ELE_MES_DATA_BLOCKS]);
