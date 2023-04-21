#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "gpio.h"

typedef struct Engine
{
    gpio_t gpio;

    bool is_need;
}engine_t;

void engine_init(engine_t* engine, int port, int pin);
void engine_off(engine_t* engine);
void engine_on(engine_t* engine);
