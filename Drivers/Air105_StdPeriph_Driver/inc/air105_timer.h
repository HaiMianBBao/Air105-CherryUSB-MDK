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

#ifndef __AIR105_TIMER_H
#define __AIR105_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"    

/* Exported types ------------------------------------------------------------*/	
typedef enum
{
	TIM_0 = 0,
	TIM_1,
	TIM_2,
	TIM_3,
	TIM_4,
	TIM_5,
    TIM_6,
    TIM_7,
}TIM_NumTypeDef;

typedef struct 
{
	TIM_NumTypeDef TIMx;
	uint32_t TIM_Period;			/*!< Specifies the period value to be loaded into the active
									   Auto-Reload Register at the next update event.
									   This parameter must be a number between 0x0000 and 0xFFFFFFFF.  */ 
}TIM_InitTypeDef;

typedef struct 
{
	TIM_NumTypeDef TIMx;
	uint32_t TIM_LowLevelPeriod;
	uint32_t TIM_HighLevelPeriod;
}TIM_PWMInitTypeDef;

typedef enum
{
	TIM_Mode_General	= 0,
	TIM_Mode_PWM		= 1
}TIM_ModeTypeDef;
#define IS_TIM_MODE(MODE)			(MODE == TIM_Mode_General || MODE == TIM_Mode_PWM)


void TIM_DeInit(TIM_Module_TypeDef *TIMMx);
void TIM_Init(TIM_Module_TypeDef* TIMMx, TIM_InitTypeDef* TIM_InitStruct);

void TIM_PWMInit(TIM_Module_TypeDef* TIMMx, TIM_PWMInitTypeDef* TIM_PWMInitStruct);
void TIM_Cmd(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx, FunctionalState NewState);
void TIM_ModeConfig(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx, TIM_ModeTypeDef TIM_Mode);
void TIM_SetPeriod(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx, uint32_t Period);
void TIM_SetPWMPeriod(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx, uint32_t PWM_LowLevelPeriod, uint32_t PWM_HighLevelPeriod);
void TIM_ITConfig(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx, FunctionalState NewState);
void TIM_ClearITPendingBit(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx);
void TIM_PWMSinglePulseConfig(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx, FunctionalState NewState);
void TIM_PWMReloadSinglePulse(TIM_Module_TypeDef *TIMMx, TIM_NumTypeDef TIMx);

ITStatus TIM_GetITStatus(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx);
uint32_t TIM_GetAllITStatus(TIM_Module_TypeDef* TIMMx);
ITStatus TIM_GetRawITStatus(TIM_Module_TypeDef* TIMMx, TIM_NumTypeDef TIMx);
uint32_t TIM_GetAllRawITStatus(TIM_Module_TypeDef* TIMMx);

uint32_t TIM_GetCounter(TIM_Module_TypeDef *TIMMx, TIM_NumTypeDef TIMx);

#ifdef __cplusplus
}
#endif

#endif
