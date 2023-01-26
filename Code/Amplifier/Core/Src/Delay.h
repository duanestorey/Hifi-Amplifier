/*
 * Delay.h
 *
 *  Created on: Jan. 26, 2023
 *      Author: duane
 */

#ifndef SRC_DELAY_H_
#define SRC_DELAY_H_

#include "stm32f1xx_hal.h"
extern uint32_t DWT_Delay_Init(void);

__STATIC_INLINE void DWT_Delay_us(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;

  /* Go to number of cycles for system */
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

  /* Delay till end */
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}


#endif /* SRC_DELAY_H_ */
