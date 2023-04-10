#include <stdint.h>
#include <stdbool.h>

//---------------------------------------------------------

#include <inc/modregs.h>
#include <inc/gpio.h>
#include <inc/rcc.h>
#include <inc/systick.h>
#include <inc/sleep.h>
//=========================================================

#define CPU_FREQENCY 48000000U // CPU frequency: 48 MHz
#define REF_FREQUENCY_DIV 8 // SysTick reference clock frequency: 6MHz
#define ONE_MILLISECOND 48000U

#define BLUE_LED_GPIOC_PIN  8U
#define GREEN_LED_GPIOC_PIN 9U

#define SYSTICK_PERIOD_US 100U
#define SYSTICKS_PER_ONE_SECOND (1000000U / SYSTICK_PERIOD_US)

//=========================================================

static void board_clocking_init(void);
static void board_gpio_init(void);
static void systick_init(uint32_t period_us);

//=========================================================

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

//-----------------------
// SysTick configuration
//-----------------------

static void systick_init(uint32_t period_us)
{
    uint32_t systick_src_freq = CPU_FREQENCY;
    bool ref_freq_avail = false;

    if (!SYSTICK_GET_NOREF())
    {
        systick_src_freq /= REF_FREQUENCY_DIV;
        ref_freq_avail = true;
    }

    uint32_t reload_value = 0;

    if (!SYSTICK_GET_SKEW())
    {
        // TENMS value is exact

        /*
            NOTE: 
            The SysTick calibration value is set to 6000, which 
            gives a reference time base of 1 ms with
            the SysTick clock set to 6 MHz (max fHCLK/8).
        */

        uint32_t calib_value  = SYSTICK_GET_SKEW();
        reload_value = (ref_freq_avail)? calib_value * period_us :
                                         calib_value * period_us * REF_FREQUENCY_DIV;
    }
    else 
    {
        // 1 = TENMS value is inexact, or not given
        reload_value = period_us * (systick_src_freq / 1000000U);
    }

    // Program the reload value:
    // SYSTICK_SET_RELOAD((reload_value - 1U));
    *SYSTICK_RVR = (reload_value - 1U);

    // // Clear the current value:
    // SYSTICK_SET_CURRENT(0);
    *SYSTICK_CVR = 0;

    // // Program the CSR:

    if (ref_freq_avail == true)
        SYSTICK_SET_SRC_REF();
    else 
        SYSTICK_SET_SRC_CPU();

    SYSTICK_EXC_ENABLE();
    SYSTICK_ENABLE();
}

void systick_handler(void)
{

}

typedef struct Engine
{
    uint32_t GPIOx;
    uint8_t pin;
}engine_t;

int default_output_pin_init(unsigned int port, unsigned int pin)
{
    if (port < GPIOA || port > GPIOF)
        return -1;

    if (pin > 15)
        return -1;

    uint8_t bit = REG_RCC_AHBENR_IOPAEN + (port - GPIOA) / GPIO_offs;
    SET_BIT(REG_RCC_AHBENR, bit);

    SET_GPIO_IOMODE(port, pin, GPIO_IOMODE_ANALOG_MODE);
    SET_GPIO_OTYPE(port, pin, GPIO_OTYPE_PUSH_PULL);

    return 0;
}

void engine_init(engine_t* engine, int port, int pin)
{
    if (!engine)
        return;
    
    engine->GPIOx = port;
    engine->pin   = pin;

    default_output_pin_init(engine->GPIOx, engine->pin);
    
    return;
}

void engine_on(engine_t* engine)
{
    GPIO_BSRR_SET_PIN(engine->GPIOx, engine->pin);
    return;
}
//------
// Main
//------

int main(void)
{
    board_clocking_init();
    board_gpio_init();
    //systick_init(SYSTICK_PERIOD_US);

    //engine_t engine = {};
    //engine_init(&engine, GPIOC, 11);
    //engine_on(&engine);
    uint8_t bit = REG_RCC_AHBENR_IOPAEN + (GPIOC - GPIOA) / GPIO_offs;
    SET_BIT(REG_RCC_AHBENR, bit);
    SET_GPIO_IOMODE(GPIOC, 11, GPIO_IOMODE_GEN_PURPOSE_OUTPUT);
    //SET_GPIO_OTYPE(GPIOC, 11, GPIO_OTYPE_OPEN_DRAIN);
    SET_GPIO_OTYPE(GPIOC, 11, GPIO_OTYPE_PUSH_PULL);
    GPIO_BSRR_SET_PIN(GPIOC, 11);
    //GPIO_BRR_RESET_PIN(GPIOC, 11);

    while (1)
    {
        //GPIO_BSRR_SET_PIN(GPIOC, 11);

        //GPIO_BSRR_SET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();
        more_precise_delay_forbidden_by_quantum_mechanics_1000ms();

        //GPIO_BRR_RESET_PIN(GPIOC, 11);
        //engine_on(&engine);
        //GPIO_BRR_RESET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
    }
}
