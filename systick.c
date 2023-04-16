#include <stdint.h>
#include <inc\time.h>
#include <stdbool.h>
#include <inc\systick.h>
#include <inc\buzzer.h>
#include <inc\engine.h>

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

extern buzzer_t buzzer;
extern engine_t engine;

void systick_handler(void)
{
    static unsigned handler_ticks = 0U;
    handler_ticks += 1U;
    static bool engine_is_on = false;


    if (handler_ticks % 1000000 * SYSTICK_FREQ)
    {
        if (!engine_is_on)
        {
            engine_on(&engine);
            engine_is_on = true;
        }
        else
        {
            //engine_off(&engine);
            engine_is_on = false;
        }
    }

    //buzzer_work(&buzzer, handler_ticks);
}