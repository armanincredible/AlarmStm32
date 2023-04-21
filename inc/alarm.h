#pragma once

#include "systick.h"
#include "buzzer.h"
#include "engine.h"
#include "gpio.h"
#include "time.h"
#include "seg.h"

typedef struct Alarm
{
    buzzer_t buzzer;
    engine_t engine;
    time_t   time;
    time_t   time_alarm;
    gpio_t   button_analog;
    seg7d_t  seg7;
}alarm_t;
