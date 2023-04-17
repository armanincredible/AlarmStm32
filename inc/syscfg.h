#pragma once

#include <inttypes.h>

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

