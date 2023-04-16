#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct Buzzer
{
    uint32_t GPIOx;
    uint8_t pin;

    unsigned freq;
    unsigned tick_freq;
    bool state;

}buzzer_t;

void buzzer_init(buzzer_t* buzzer, int port, int pin, unsigned freq, unsigned tick_freq);
void buzzer_work(buzzer_t* buzzer, unsigned tick);
