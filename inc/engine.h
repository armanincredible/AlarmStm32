#pragma once
#include <stdint.h>


typedef struct Engine
{
    uint32_t GPIOx;
    uint8_t pin;
}engine_t;

void engine_init(engine_t* engine, int port, int pin);
void engine_off(engine_t* engine);
void engine_on(engine_t* engine);
