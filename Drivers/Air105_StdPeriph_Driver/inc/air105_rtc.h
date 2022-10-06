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

#ifndef __AIR105_RTC_H
#define __AIR105_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"

#define RTC_32K_EXT_INC_SELECT                      (0x00000400U)
	
typedef enum 
{
    SELECT_EXT32K,
    SELECT_INC32K
} RTCCLK_SOURCE_TypeDef;
#define IS_RTCCLK_SOURCE(FREQ)                      (((FREQ) == SELECT_EXT32K) || \
                                                    ((FREQ) == SELECT_INC32K))	

FlagStatus RTC_IsReady(void);

void RTC_ResetCounter(void);
uint32_t RTC_GetCounter(void);

void RTC_SetRefRegister(uint32_t RefValue);
uint32_t RTC_GetRefRegister(void);

void RTC_SetAlarm(uint32_t AlarmValue);

uint32_t RTC_GetAttrackTime(void);

void RTC_ITConfig(FunctionalState NewState);
void RTC_ClearITPendingBit(void);
ITStatus RTC_GetITStatus(void);
void RTC_CLKSourceSelect(RTCCLK_SOURCE_TypeDef source);
	
#ifdef __cplusplus
}
#endif

#endif
