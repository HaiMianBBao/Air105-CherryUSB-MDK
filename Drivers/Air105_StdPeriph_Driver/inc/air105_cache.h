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

#ifndef __AIR105_CACHE_H
#define __AIR105_CACHE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "air105.h"

	
#define CACHE_REFRESH		                  ((uint32_t)0x80000000)
#define CACHE_REFRESH_ALLTAG	              ((uint32_t)0x40000000)

#define CACHE_KEY_GEN_START		              ((uint32_t)0x80000000)	

#define CACHE_IS_BUSY			              ((uint32_t)0x20000000)
	
#define CACHE_SIZE                            ((uint32_t)0x8000)
#define CACHE_PARTICLE_SIZE                   (0x20)
#define CACHE_ADDRESS_START                   ((uint32_t)0x01000000)	
#define CACHE_ADDRESS_MAX                     ((uint32_t)0x00FFFFFF)
#define IS_CACHE_ADDR_VALID(addr)             (((addr) & CACHE_ADDRESS_START) == CACHE_ADDRESS_START)
		
#define CACHE_AES_BYPASS		              (0xA5)
#define CACHE_KEY_GEN			              (0xA5)
#define CACHE_WRAP_ENABLE				 	  (0xA5)

#define CACHE_ZONE_ENCRYPT			          ((uint32_t)0xA5000000)
	
#define CACHE_CODE_BUS_OFFSET_POS               (0)
#define CACHE_CODE_BUS_OFFSET_WIDTH             (5)
#define CACHE_CODE_BUS_OFFSET_MASK              ((uint32_t)0x001F)

#define CACHE_CODE_BUS_SET_POS                  (5)
#define CACHE_CODE_BUS_SET_WIDTH                (8)
#define CACHE_CODE_BUS_SET_MASK                 ((uint32_t)0x00FF)

#define CACHE_CODE_BUS_TAG_POS                  (13)
#define CACHE_CODE_BUS_TAG_WIDTH                (11)
#define CACHE_CODE_BUS_TAG_MASK                 ((uint32_t)0x07FF)

#define CHCHE_ALGORITHM_SET_POS					(28)

#define CACHE_BUILD_INDEX_OFFSET(x)				(((x) & CACHE_CODE_BUS_OFFSET_MASK) << CACHE_CODE_BUS_OFFSET_POS)
#define CACHE_BUILD_INDEX_SET(x)				(((x) & CACHE_CODE_BUS_SET_MASK) << CACHE_CODE_BUS_SET_POS)
#define CACHE_BUILD_INDEX_TAG(x)				(((x) & CACHE_CODE_BUS_TAG_MASK) << CACHE_CODE_BUS_TAG_POS)
#define CACHE_ADDRESS_BUILD(base,index_set,tag_way,offset)   ((base) | CACHE_BUILD_INDEX_SET(index_set) | CACHE_BUILD_INDEX_TAG(tag_way) | CACHE_BUILD_INDEX_OFFSET(offset))

#define CACHE_TAG_NUM(x)						((x >> CACHE_CODE_BUS_TAG_POS) & CACHE_CODE_BUS_TAG_MASK)
#define CACHE_SET_NUM(x)						((x >> CACHE_CODE_BUS_SET_POS) & CACHE_CODE_BUS_SET_MASK)
#define CACHE_OFF_NUM(x)						((x >> CACHE_CODE_BUS_OFFSET_POS) & CACHE_CODE_BUS_OFFSET_MASK)

typedef enum
{
	CACHE_Encrypt_Mode_All = 0x0,
	CACHE_Encrypt_Mode_Zone,
}CACHE_EncryptModeTypeDef;
#define IS_CACHE_ENCRYPT_MODE(MODE)		(((MODE) == CACHE_Encrypt_Mode_All) || \
										((MODE) == CACHE_Encrypt_Mode_Zone))

typedef struct                        
{      
	uint32_t I[4];
	uint32_t K[4];   
	uint32_t AES_CS;
	uint32_t CONFIG;

	uint32_t aes_enable;
	uint32_t Address;                 
	uint32_t size;
	uint32_t algorithm;
	
	uint32_t encrypt_mode; 
	uint32_t encrypt_saddr; 
	uint32_t encrypt_eaddr; 	
}CACHE_InitTypeDef;

void CACHE_Init(CACHE_TypeDef *Cache, CACHE_InitTypeDef *CACHE_InitStruct);
void CACHE_Clean(CACHE_TypeDef *Cache, CACHE_InitTypeDef *CACHE_InitStruct);
void CACHE_CleanAll(CACHE_TypeDef *Cache);


#ifdef __cplusplus
}
#endif

#endif 
