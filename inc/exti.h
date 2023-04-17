#pragma once
#include "config.h"
#include "modregs.h"
#include <inttypes.h>


/*Interrupt mask register*/
#ifdef STM32F05X
#define EXTI_IMR (volatile uint32_t*)(uintptr_t)0x0F940000
#elif STM32F04X
#define EXTI_IMR (volatile uint32_t*)(uintptr_t)0x7FF40000
#elif STM32F03X 
#define EXTI_IMR (volatile uint32_t*)(uintptr_t)0x0FF40000
#elif STM32F07X || STM32F09X
#define EXTI_IMR (volatile uint32_t*)(uintptr_t)0x7F840000
#endif



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
#define EXTI_INTERRUP_LINE_ON(line) SET_BIT(EXTI_IMR, line)
#define IS_EXTI_INTERRUPT_LINE_ON(line) CHECK_BIT(EXTI_IMR, line)

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

#define SYSCFG         (volatile uint32_t*)(uintptr_t)0x40010000
#define SYSCFG_EXTICR  (volatile uint32_t*)(uintptr_t)(SYSCFG + 0x08)

typedef struct {
    uint32_t CFGR1;    /* SYSCFG configuration register 1 */
    uint32_t RESERVED0;
    uint32_t EXTICR[4];  /* External interrupt configuration registers */
    uint32_t CFGR2;    /* SYSCFG configuration register 2 */
} SYSCFG_T;

//#define MODIFY_REG(REG, MASK, VAL) \
//  ((REG) = ((REG) & ~(MASK)) | ((VAL) & (MASK)))

#define POSITION_VAL(VAL) (__CLZ(__RBIT(VAL)))

void LL_SYSCFG_SetEXTISource(uint32_t Port, uint32_t Line)    
{
  uint32_t cr_num, cr_offset;
  
  /* Calculate the EXTI_CR register number and offset based on the EXTI line */
    cr_num = Line / 4;
    cr_offset = (Line % 4) * 4;

    SYSCFG_T* syscfg = (SYSCFG_T*)SYSCFG;
    syscfg->EXTICR[cr_num] &= ~(0xF << cr_offset);
    syscfg->EXTICR[cr_num] |= Port << cr_offset;
}
