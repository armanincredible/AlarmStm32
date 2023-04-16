#pragma once

#include "gpio.h"
#include "rcc.h"
#include "modregs.h"
#include <stdint.h>

static inline int default_output_pin_init(unsigned int port, unsigned int pin)
{
    if (port < GPIOA || port > GPIOF)
        return -1;

    if (pin > 15)
        return -1;

    uint8_t bit = REG_RCC_AHBENR_IOPAEN + (port - GPIOA) / GPIO_offs;
    SET_BIT(REG_RCC_AHBENR, bit);

    SET_GPIO_IOMODE(port, pin, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);
    SET_GPIO_OTYPE(port, pin, GPIO_OTYPE_PUSH_PULL);

    return 0;
}
