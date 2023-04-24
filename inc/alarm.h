#pragma once

#include "systick.h"
#include "buzzer.h"
#include "engine.h"
#include "gpio.h"
#include "time.h"
#include "seg.h"

struct List;

typedef struct Alarm
{
    buzzer_t buzzer;
    engine_t engine;
    uint64_t   time;
    uint64_t   time_alarm;
    struct List *time_head; /* list head, first time is time_head->next*/
    gpio_t   button_analog;
    seg7d_t  seg7;
}alarm_t;
