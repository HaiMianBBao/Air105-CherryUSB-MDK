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

#ifndef __AIR105_TRNG_H
#define __AIR105_TRNG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"

/** @defgroup RNG_Exported_Types
  * @{
  */
typedef enum{
    TRNG0
} TRNG_ChannelTypeDef;
#define IS_TRNG_CHANNEL(CHANNEL)  	(((CHANNEL) == TRNG0))


#define TRNG_IT_RNG0_S128           ((uint32_t)0x00000001)
#define TRNG_IT_RNG0_ATTACK         ((uint32_t)0x00000004)    
#define IS_TRNG_GET_IT(IT)  		(((IT) == TRNG_IT_RNG0_S128) || \
									((IT) == TRNG_IT_RNG0_ATTACK))

/** @defgroup RNG_Exported_Functions
  * @{
  */
void TRNG_Start(TRNG_ChannelTypeDef TRNGx);
void TRNG_Stop(TRNG_ChannelTypeDef TRNGx);
uint32_t TRNG_Get(uint32_t rand[4], TRNG_ChannelTypeDef TRNGx);
void TRNG_SetPseudoRandom(uint32_t PseudoRandom);
void TRNG_DirectOutANA(TRNG_ChannelTypeDef TRNGx, FunctionalState NewState);

void TRNG_ITConfig(FunctionalState NewState);
ITStatus TRNG_GetITStatus(uint32_t TRNG_IT);
void TRNG_ClearITPendingBit(uint32_t TRNG_IT);


#ifdef __cplusplus
}
#endif

#endif 
