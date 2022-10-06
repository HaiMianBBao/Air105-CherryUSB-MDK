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

#include "air105_wdt.h"

#define COUNTER_RELOAD_KEY				((uint32_t)0x76)

void WDT_SetReload(uint32_t Reload)
{
	WDT->WDT_RLD = Reload;
}

void WDT_ReloadCounter(void)
{
	WDT->WDT_CRR = COUNTER_RELOAD_KEY;
}

ITStatus WDT_GetITStatus(void)
{
	ITStatus bitstatus = RESET;

	if(!(WDT->WDT_STAT & WDT_STAT_INT))
	{
		bitstatus = RESET;
	}
	else
	{
		bitstatus = SET;
	}

	return bitstatus;

}

void WDT_ClearITPendingBit(void)
{
	//read EOI register to clear interrupt panding
	uint32_t clr = WDT->WDT_EOI;
}

void WDT_Enable(void)
{
	WDT->WDT_CR |= WDT_CR_WDT_EN;
}

void WDT_ModeConfig(WDT_ModeTypeDef WDT_Mode)
{
	if(WDT_Mode == WDT_Mode_Interrupt)
		WDT->WDT_CR |= WDT_CR_RMOD;
	else if(WDT_Mode == WDT_Mode_CPUReset)
		WDT->WDT_CR &= ~WDT_CR_RMOD;
}
