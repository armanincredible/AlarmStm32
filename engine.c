#include <inc/engine.h>
#include <inc/pin.h>
#include <stdbool.h>

void engine_init(engine_t* engine, int port, int pin)
{
    if (!engine)
        return;
    
    engine->GPIOx = port;
    engine->pin   = pin;
    engine->is_need   = true;

    default_output_pin_init(engine->GPIOx, engine->pin);
    
    return;
}

void engine_off(engine_t* engine)
{
    if (!engine->is_need)
        return;

    GPIO_BRR_RESET_PIN(engine->GPIOx, engine->pin);
    return;
}

void engine_on(engine_t* engine)
{
    if (!engine->is_need)
        return;

    GPIO_BSRR_SET_PIN(engine->GPIOx, engine->pin);
    return;
}
