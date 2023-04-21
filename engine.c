#include <inc/engine.h>
#include <inc/gpio.h>
#include <stdbool.h>

void engine_init(engine_t* engine, int port, int pin)
{
    if (!engine)
        return;
    
    engine->gpio.GPIOx = port;
    engine->gpio.pin   = pin;
    engine->is_need   = true;

    default_output_pin_init(engine->gpio.GPIOx, engine->gpio.pin);
    
    return;
}

void engine_off(engine_t* engine)
{
    if (!engine->is_need)
        return;

    GPIO_BRR_RESET_PIN(engine->gpio.GPIOx, engine->gpio.pin);
    return;
}

void engine_on(engine_t* engine)
{
    if (!engine->is_need)
        return;

    GPIO_BSRR_SET_PIN(engine->gpio.GPIOx, engine->gpio.pin);
    return;
}
