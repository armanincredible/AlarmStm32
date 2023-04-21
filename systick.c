#include <stdint.h>
#include <inc\time.h>
#include <stdbool.h>
#include <inc\systick.h>
#include <inc\buzzer.h>
#include <inc\engine.h>
#include <inc\alarm.h>
#include <inc\gpio.h>
#include <inc\seg.h>

#include <string.h>

void systick_init(uint32_t period_us)
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

extern alarm_t alarm;

const int US_IN_S = 1e6;

void systick_handler(void)
{
    static uint64_t handler_ticks = 0U;

    int ticks_in_second = US_IN_S / SYSTICK_PERIOD_US;

    handler_ticks += 1U;
    static bool alarm_is_on = false;

    if (handler_ticks == ticks_in_second)
    {
        alarm.time.seconds += 1;
        handler_ticks = 0;
    }

    if (alarm.time.seconds == 60)
    {
        alarm.time.seconds = 0;
        alarm.time.minutes += 1;
    }

    if (alarm.time.minutes == 60)
    {
        alarm.time.minutes = 0;
        alarm.time.hours += 1;
    }

    if (!memcmp(&alarm.time, &alarm.time_alarm, sizeof(time_t)))
    {
        alarm_is_on = true;
    }

    get_info_from_gpio_pin(&alarm.button_analog);
    if (alarm.button_analog.arg)
    {
        GPIO_BRR_RESET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
        alarm_is_on = false;
        engine_off(&alarm.engine);
    }

    if (alarm_is_on)
    {
        GPIO_BSRR_SET_PIN(GPIOC, GREEN_LED_GPIOC_PIN);
        engine_on(&alarm.engine);
        buzzer_work(&alarm.buzzer, handler_ticks);
    }

    alarm.seg7.number = (alarm.time_alarm.hours - alarm.time.hours) * 100 + alarm.time_alarm.minutes - alarm.time.minutes;
    //alarm.seg7.number = (alarm.time.minutes) * 100 + alarm.time.seconds;
    seg7_select_digit(&alarm.seg7, (handler_ticks % 4));
    seg7_push_display_state_to_mc(&alarm.seg7);
}