#include <stdint.h>
#include <stdbool.h>

//---------------------------------------------------------

#include <inc/modregs.h>
#include <inc/gpio.h>
#include <inc/rcc.h>
#include <inc/systick.h>
#include <inc/sleep.h>
#include <inc/engine.h>
#include <inc/time.h>
#include <inc/buzzer.h>
#include <inc/exti.h>
#include <inc/nvic.h>


void board_clocking_init()
{
    // (1) Clock HSE and wait for oscillations to setup.
    SET_BIT(REG_RCC_CR, REG_RCC_CR_HSEON);
    while (CHECK_BIT(REG_RCC_CR, REG_RCC_CR_HSERDY))
        continue;

    // (2) Configure PLL:
    // PREDIV output: HSE/2 = 4 MHz
    SET_REG_RCC_CFGR2_PREDIV(2);

    // (3) Select PREDIV output as PLL input (4 MHz):
    SET_REG_RCC_CFGR_PLLSRC(REG_RCC_CFGR_PLLSRC_HSE_PREDIV);

    // (4) Set PLLMUL to 12:
    // SYSCLK frequency = 48 MHz
    SET_REG_RCC_CFGR_PLLMUL(12);

    // (5) Enable PLL:
    SET_BIT(REG_RCC_CR, REG_RCC_CR_PLLON);
    while(CHECK_BIT(REG_RCC_CR, REG_RCC_CR_PLLRDY))
        continue;

    // (6) Configure AHB frequency to 48 MHz:
    SET_REG_RCC_CFGR_HPRE_NOT_DIV();

    // (7) Select PLL as SYSCLK source:
    SET_REG_RCC_CFGR_SW(REG_RCC_CFGR_SW_PLL);

    while(GET_REG_RCC_CFGR_SWS() != REG_RCC_CFGR_SWS_PLL)
        continue;

    // (8) Set APB frequency to 24 MHz
    SET_REG_RCC_CFGR_PPRE(REG_RCC_CFGR_PPRE_DIV_2);
}

//--------------------
// GPIO configuration
//--------------------

static void board_gpio_init()
{
    // (1) Enable GPIOC clocking:
    SET_BIT(REG_RCC_AHBENR, REG_RCC_AHBENR_IOPCEN);

    // (2) Configure PC8 & PC9 mode:
    SET_GPIO_IOMODE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);
    SET_GPIO_IOMODE(GPIOC, BLUE_LED_GPIOC_PIN, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);

    // (3) Configure PC8 & PC9 type - output push/pull 
    SET_GPIO_OTYPE(GPIOC, GREEN_LED_GPIOC_PIN, GPIO_OTYPE_PUSH_PULL);
    SET_GPIO_OTYPE(GPIOC, BLUE_LED_GPIOC_PIN, GPIO_OTYPE_PUSH_PULL);
}

static void interrupts_init()
{
    uint32_t line = 0;
    LL_SYSCFG_SetEXTISource(0, line);
    EXTI_INTERRUP_LINE_ON(line);
    EXTI_ENABLE_RISING_TRIGGER(line);

    NVIC_ENABLE_IRQ(0);
    NVIC_SET_PRIORITY(0, 0);

    return;
}

buzzer_t buzzer = {};
engine_t engine = {};

void EXTI0_1_HANDLER()
{
    if (IS_INTERRUPT_FROM_LINE(0))
    {
        buzzer_off(&buzzer);
        engine_off(&engine);
        buzzer.is_need = false;
        engine.is_need = false;
        CONTINUE_GET_INTERRUPT_FROM_LINE(0);
    }
}

int main(void)
{
    board_clocking_init();
    board_gpio_init();
    systick_init(SYSTICK_PERIOD_US);
    interrupts_init();

    engine_init(&engine, GPIOC, 11);

    buzzer_init(&buzzer, GPIOC, 5, 1000 /*freq*/, SYSTICK_FREQ);

    while (1)
    {
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
    }
}
