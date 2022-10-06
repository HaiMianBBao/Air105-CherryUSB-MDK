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

#ifndef __AIR105_CRC_H
#define __AIR105_CRC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"

/* Exported types ------------------------------------------------------------*/
typedef enum
{
	CRC_16 = 0x01,
	CRC_16_Modbus = 0x02,
	CRC_CCITT_0xffff = 0x03,
	CRC_CCITT_XModem = 0x04,
	CRC_32 = 0x05
}CRC_Param_TypeDef;
	 
typedef enum
{
	CRC_Poly_16_1021 = 0x01,
	CRC_Poly_16_8005 = 0x02,
	CRC_Poly_32_04C11DB7 = 0x03
}CRC_Poly_TypeDef;

typedef enum
{
	CRC_PolyMode_Normal = 0x01,
	CRC_PolyMode_Reversed = 0x02,
}CRC_PolyMode_TypeDef;

typedef struct
{
	CRC_Poly_TypeDef CRC_Poly;
	CRC_PolyMode_TypeDef CRC_PolyMode;
	uint32_t CRC_Init_Value;
	uint32_t CRC_Xor_Value;
}CRC_ConfigTypeDef;

 
uint32_t CRC_CalcBlockCRC(uint32_t CRC_type, uint8_t *pData, uint32_t len);
uint32_t CRC_Calc(CRC_ConfigTypeDef *CRC_Config, uint8_t *pData, uint32_t len);
	 
	 
#ifdef __cplusplus
}
#endif

#endif
