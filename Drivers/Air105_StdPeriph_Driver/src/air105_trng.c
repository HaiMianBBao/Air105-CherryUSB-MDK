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

#include "air105_trng.h"


const uint32_t TRNG_RNG_AMA_PD_Mask[1] =			{TRNG_RNG_AMA_PD_ALL_Mask};
const uint32_t TRNG_RNG_AMA_ANA_OUT_Mask[1] =		{TRNG_RNG_AMA_ANA_OUT_TRNG0_Mask};
const uint32_t TRNG_RNG_CSR_S128_Mask[1] =			{TRNG_RNG_CSR_S128_TRNG0_Mask};
const uint32_t TRNG_RNG_CSR_ATTACK_Mask[1] =		{TRNG_RNG_CSR_ATTACK_TRNG0_Mask};

/**
  * @method	TRNG_Get
  * @brief	
  * @param	rand
  * @param	TRNGx
  * @retval 
  */
uint32_t TRNG_Get(uint32_t rand[4], TRNG_ChannelTypeDef TRNGx)
{
	uint32_t ret = 1;
	assert_param(IS_TRNG_CHANNEL(TRNGx));
	
	if (TRNG->RNG_CSR & TRNG_RNG_CSR_S128_Mask[TRNGx])
	{
		rand[0] = TRNG->RNG_DATA[TRNGx];
		rand[1] = TRNG->RNG_DATA[TRNGx];
		rand[2] = TRNG->RNG_DATA[TRNGx];
		rand[3] = TRNG->RNG_DATA[TRNGx];
		ret = 0;
	}
	return ret;
}

/**
  * @method	TRNG_SetPseudoRandom
  * @brief	
  * @param	PseudoRandom
  * @retval 
  */
void TRNG_SetPseudoRandom(uint32_t PseudoRandom)
{
	TRNG->RNG_PN = PseudoRandom;
}

/**
  * @method	TRNG_DirectOutANA
  * @brief	
  * @param	TRNGx
  * @param	NewState
  * @retval 
  */
void TRNG_DirectOutANA(TRNG_ChannelTypeDef TRNGx, FunctionalState NewState)
{
	assert_param(IS_TRNG_CHANNEL(TRNGx));
	assert_param(IS_FUNCTIONAL_STATE(NewState));
	
	if (NewState != DISABLE)
	{
		TRNG->RNG_ANA |= TRNG_RNG_AMA_ANA_OUT_Mask[TRNGx];
	}
	else
	{
		TRNG->RNG_ANA &= ~TRNG_RNG_AMA_ANA_OUT_Mask[TRNGx];
	}
}


/**
  * @method	TRNG_ITConfig
  * @brief	
  * @param	NewState
  * @retval 
  */
void TRNG_ITConfig(FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
		TRNG->RNG_CSR |= TRNG_RNG_CSR_INTP_EN_Mask;
	}
	else
	{
		TRNG->RNG_CSR &= ~TRNG_RNG_CSR_INTP_EN_Mask;
	}
}

/**
  * @brief  获取中断状态
  * @param  TRNG_IT:
  *         TRNG_IT_RNG0_S128
  *         TRNG_IT_RNG1_S128
  *         TRNG_IT_RNG0_ATTACK
  *         TRNG_IT_RNG1_ATTACK
  * @retval None
  */
ITStatus TRNG_GetITStatus(uint32_t TRNG_IT)
{
    assert_param(IS_TRNG_GET_IT(TRNG_IT));
	
    if (((TRNG->RNG_CSR) & TRNG_IT) != (uint32_t)RESET)
	{
		return SET;
	}
	else
	{
		return RESET;
	}
}

/**
  * @brief  清除中断标志位
  * @param  TRNG_IT:
  *         TRNG_IT_RNG0_S128
  *         TRNG_IT_RNG1_S128
  *         TRNG_IT_RNG0_ATTACK
  *         TRNG_IT_RNG1_ATTACK
  * @retval None
  */
void TRNG_ClearITPendingBit(uint32_t TRNG_IT)
{
    assert_param(IS_TRNG_GET_IT(TRNG_IT));
	
    TRNG->RNG_CSR &= ~TRNG_IT;
}

/**
  * @brief  启动生成随机数
  * @param  
  * @retval None
  */
void TRNG_Start(TRNG_ChannelTypeDef TRNGx)
{
	assert_param(IS_TRNG_CHANNEL(TRNGx));
	
	TRNG->RNG_ANA &= ~TRNG_RNG_AMA_PD_Mask[TRNGx];
	TRNG->RNG_CSR &= ~TRNG_RNG_CSR_S128_Mask[TRNGx];
}

/**
  * @brief  停止产生随机数
  * @param  
  * @retval None
  */
void TRNG_Stop(TRNG_ChannelTypeDef TRNGx)
{
	assert_param(IS_TRNG_CHANNEL(TRNGx));
	
	TRNG->RNG_ANA |= TRNG_RNG_AMA_PD_Mask[TRNGx];
}
