#pragma once
#include "config.h"
#include "modregs.h"
#include <inttypes.h>

/*Interrupt mask register*/
#define EXTI_IMR_REGISTER (volatile uint32_t*)(uintptr_t)0x40010400
#define EXTI_IMR 0x40010400

/*
    PA0 --------|
    PB0 --------|
    PC0 --------|
                |-------> EXTI0
    PD0 --------|
    PE0 --------|
    PF0 --------|

    PA1 --------|
    PB1 --------|
    PC1 --------|
                |-------> EXTI1
    PD1 --------|
    PE1 --------|
    PF1 --------|

    ...

    PA15 --------|
    PB15 --------|
    PC15 --------|
                |-------> EXTI15
    PD15 --------|
    PE15 --------|
    PF15 --------|

*/


/*Falling trigger selection register*/
#define EXTI_FTSR (volatile uint32_t*)(uintptr_t)(EXTI_IMR + 0x0C)

/*Rising trigger selection register*/
#define EXTI_RTSR (volatile uint32_t*)(uintptr_t)(EXTI_IMR + 0x08)

/*Event mask register*/
#define EXTI_EMR (volatile uint32_t*)(uintptr_t)(EXTI_IMR + 0x04)

/*Pending register*/
#define EXTI_PR (volatile uint32_t*)(uintptr_t)(EXTI_IMR + 0x14)

//Software interrupt event register
#define EXTI_SWIER (volatile uint32_t*)(uintptr_t)(EXTI_IMR + 0x10)


/*enable interrupt on line (32 lines and only first 16 with programming handler)*/
#define EXTI_INTERRUP_LINE_ON(line) SET_BIT(EXTI_IMR_REGISTER, line)
#define IS_EXTI_INTERRUPT_LINE_ON(line) CHECK_BIT(EXTI_IMR_REGISTER, line)

/*enable event*/
#define EXTI_EVENT_LINE_ON(line) SET_BIT(EXTI_EMR, line)

/*If the interrupt is enabled on this line in the EXTI_IMR, writing a ‘1’ to this bit when it is at ‘0’
sets the corresponding pending bit in EXTI_PR resulting in an interrupt request generation.
This bit is cleared by clearing the corresponding bit of EXTI_PR (by writing a ‘1’ to the bit)*/
#define SOFTWARE_INTERRUPT(line) SET_BIT(EXTI_SWIER, line)

/*This bit is set when the selected edge event arrives on the external interrupt line. 
This bit is cleared by writing a 1 to the bit. */
#define IS_INTERRUPT_FROM_LINE(line) CHECK_BIT(EXTI_PR, line)
#define CONTINUE_GET_INTERRUPT_FROM_LINE(line) SET_BIT(EXTI_PR, line)

// TODO DELETE COPYPASTE
#define EXTI_ENABLE_RISING_TRIGGER(line)                            \
        do{                                                         \
            if (!(line == 18 || (line <= 30 && line >= 23)))        \
                SET_BIT(EXTI_RTSR, line);                           \
        }while(0)

#define EXTI_DISABLE_RISING_TRIGGER(line)                           \
        do{                                                         \
            if (!(line == 18 || (line <= 30 && line >= 23)))        \
                CLEAR_BIT(EXTI_RTSR, line);                         \
        }while(0)

#define EXTI_ENABLE_Falling_TRIGGER(line)                           \
        do{                                                         \
            if (!(line == 18 || (line <= 30 && line >= 23)))        \
                SET_BIT(EXTI_FTSR, line);                           \
        }while(0)

#define EXTI_DISABLE_Falling_TRIGGER(line)                          \
        do{                                                         \
            if (!(line == 18 || (line <= 30 && line >= 23)))        \
                CLEAR_BIT(EXTI_FTSR, line);                         \
        }while(0)

