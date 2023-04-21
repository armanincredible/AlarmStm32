// Define base addresses for peripherals

#include "stdint.h"
#include <inc/modregs.h>
#include <inc/gpio.h>
#include <inc/exti.h>
#include <inc/nvic.h>
#include <inc/buzzer.h>
#include <inc/engine.h>
#include <inc/syscfg.h>
#include <inc/sleep.h>
#include <inc/alarm.h>

void interrupts_init()
{
    __asm__ volatile ("cpsie i");

    default_input_pin_init(GPIOA, 0);

    SET_BIT(REG_RCC_APB2ENR, REG_RCC_APB2ENR_SYSCFGCOMPEN);

    uint32_t line = 0;
    LL_SYSCFG_SetEXTISource(0, line);
    EXTI_INTERRUP_LINE_ON(line);

    SET_BIT(EXTI_RTSR, line);
    SET_BIT(EXTI_FTSR, line);
    //EXTI_ENABLE_RISING_TRIGGER(line);

    NVIC_ENABLE_IRQ(0);
    NVIC_SET_PRIORITY(0, 0);
    CONTINUE_GET_INTERRUPT_FROM_LINE(0);

    return;

}

extern alarm_t alarm;

void EXTI0_1_HANDLER()
{
    while(1)
    {
        GPIO_BSRR_SET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
        engine_on(&alarm.engine);
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        
        GPIO_BRR_RESET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
        engine_off(&alarm.engine);
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
    }
    GPIO_BSRR_SET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
    if (IS_INTERRUPT_FROM_LINE(0))
    {
        //buzzer_off(&buzzer);
        //engine_off(&engine);
        //buzzer.is_need = false;
        //engine.is_need = false;
        //CONTINUE_GET_INTERRUPT_FROM_LINE(0);
    }
}