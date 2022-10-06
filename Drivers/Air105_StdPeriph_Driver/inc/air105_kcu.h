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

#ifndef __AIR105_KCU_H
#define __AIR105_KCU_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"
	 
/** 
  * @brief  KCU Init structure definition  
  */
typedef struct
{
	uint32_t KCU_DebounceTimeLevel;
	uint32_t KCU_PortInput;
	uint32_t KCU_PortOutput;
	uint32_t KCU_Rand;
}KCU_InitTypeDef;


#define KCU_IT_PUSH							(KCU_CTRL1_PUSH_IT)
#define KCU_IT_RELEASE						(KCU_CTRL1_RELEASE_IT)
#define KCU_IT_OVERRUN						(KCU_CTRL1_OVERRUN_IT)

void KCU_DeInit(void);

void KCU_Init(KCU_InitTypeDef *KCU_InitStruct);
void KCU_StructInit(KCU_InitTypeDef *KCU_InitStruct);

void KCU_Cmd(FunctionalState NewState);

uint32_t KCU_SetRand(uint32_t rand);

void KCU_ITConfig(uint32_t KCU_IT, FunctionalState NewState);
FlagStatus KCU_GetITStatus(uint32_t KCU_IT);
void KCU_ClearITPending(void);

uint32_t KCU_GetEvenReg(void);


#ifdef __cplusplus
}
#endif

#endif 
