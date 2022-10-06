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

#ifndef __AIR105_WDT_H
#define __AIR105_WDT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"

typedef enum
{
	WDT_Mode_CPUReset = 0,
	WDT_Mode_Interrupt = 1
}WDT_ModeTypeDef;
	 
void WDT_SetReload(uint32_t Reload);
void WDT_ReloadCounter(void);
void WDT_Enable(void);
void WDT_ModeConfig(WDT_ModeTypeDef WDT_Mode);
	 
ITStatus WDT_GetITStatus(void);
void WDT_ClearITPendingBit(void);

#ifdef __cplusplus
}
#endif

#endif
