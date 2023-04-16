#include "inc/buzzer.h"
#include <inc/pin.h>
#include <stdio.h>

void buzzer_init(buzzer_t* buzzer, int port, int pin, unsigned freq, unsigned tick_freq)
{
    if (!buzzer)
        return;
    
    buzzer->GPIOx = port;
    buzzer->pin   = pin;
    buzzer->tick_freq = tick_freq;
    buzzer->freq   = freq;

    default_output_pin_init(buzzer->GPIOx, buzzer->pin);
    
    return;
}

void buzzer_work(buzzer_t* buzzer, unsigned tick)
{
    if (buzzer == NULL)
        return;

    if (buzzer->freq == 0)
        return;

    unsigned rate = buzzer->tick_freq / (buzzer->freq * 2);

    if ((tick % rate) == 0)
    {
        if (buzzer->state == false)
        {
            GPIO_BSRR_SET_PIN(buzzer->GPIOx, buzzer->pin);
            buzzer->state = true;
        }
        else 
        {
            GPIO_BRR_RESET_PIN(buzzer->GPIOx, buzzer->pin);
            buzzer->state = false;
        }
    }

    return;
}