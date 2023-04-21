#pragma once

#include <stdint.h>

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define REF_FREQUENCY_DIV 8 // SysTick reference clock frequency: 6MHz
#define ONE_MILLISECOND 48000U

typedef struct time
{
    uint64_t seconds;
    uint64_t minutes;
    uint64_t hours;
    
}time_t;

