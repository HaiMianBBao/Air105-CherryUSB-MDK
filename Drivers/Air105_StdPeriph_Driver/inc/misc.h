// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __MISC_H
#define __MISC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"



typedef struct
{
  uint32_t NVIC_IRQChannel;                    /*!< Specifies the IRQ channel to be enabled or disabled.
                                                   This parameter can be a value of @ref IRQn_Type 
                                                   (For the complete SCPU Devices IRQ Channels list, please
                                                    refer to air105.h file) */

  uint32_t NVIC_IRQChannelPreemptionPriority;  /*!< Specifies the pre-emption priority for the IRQ channel
                                                   specified in NVIC_IRQChannel. This parameter can be a value
                                                   between 0 and 15 as described in the table @ref NVIC_Priority_Table */

  uint32_t NVIC_IRQChannelSubPriority;         /*!< Specifies the subpriority level for the IRQ channel specified
                                                   in NVIC_IRQChannel. This parameter can be a value
                                                   between 0 and 15 as described in the table @ref NVIC_Priority_Table */

  FunctionalState NVIC_IRQChannelCmd;         /*!< Specifies whether the IRQ channel defined in NVIC_IRQChannel
                                                   will be enabled or disabled. 
                                                   This parameter can be set either to ENABLE or DISABLE */   
} NVIC_InitTypeDef;
 


#define NVIC_VectTab_RAM             ((uint32_t)0x20000000)
#define NVIC_VectTab_FLASH           ((uint32_t)0x01000000)
#define IS_NVIC_VECTTAB(VECTTAB) (((VECTTAB) == NVIC_VectTab_RAM) || \
                                  ((VECTTAB) == NVIC_VectTab_FLASH))


#define NVIC_PriorityGroup_0         ((uint32_t)0x07) /*!< 0 bits for pre-emption priority
                                                            3 bits for subpriority */
#define NVIC_PriorityGroup_1         ((uint32_t)0x06) /*!< 1 bits for pre-emption priority
                                                            2 bits for subpriority */
#define NVIC_PriorityGroup_2         ((uint32_t)0x05) /*!< 2 bits for pre-emption priority
                                                            1 bits for subpriority */
#define NVIC_PriorityGroup_3         ((uint32_t)0x04) /*!< 3 bits for pre-emption priority
                                                            0 bits for subpriority */

#define IS_NVIC_PRIORITY_GROUP(GROUP) (((GROUP) == NVIC_PriorityGroup_0) || \
                                       ((GROUP) == NVIC_PriorityGroup_1) || \
                                       ((GROUP) == NVIC_PriorityGroup_2) || \
                                       ((GROUP) == NVIC_PriorityGroup_3))

#define IS_NVIC_PREEMPTION_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

#define IS_NVIC_SUB_PRIORITY(PRIORITY)  ((PRIORITY) < 0x10)

#define IS_NVIC_OFFSET(OFFSET)  ((OFFSET) < 0x000FFFFF)


/** @defgroup MISC_SysTick_clock_source 
  * @{
  */

#define SysTick_CLKSource_HCLK_Div8    ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK         ((uint32_t)0x00000004)
#define IS_SYSTICK_CLK_SOURCE(SOURCE) (((SOURCE) == SysTick_CLKSource_HCLK) || \
                                       ((SOURCE) == SysTick_CLKSource_HCLK_Div8))


//void NVIC_PriorityGroupConfig(uint32_t NVIC_PriorityGroup);
void NVIC_Init(NVIC_InitTypeDef* NVIC_InitStruct);
void NVIC_SetVectorTable(uint32_t NVIC_VectTab, uint32_t Offset);
void SysTick_CLKSourceConfig(uint32_t SysTick_CLKSource);

#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
