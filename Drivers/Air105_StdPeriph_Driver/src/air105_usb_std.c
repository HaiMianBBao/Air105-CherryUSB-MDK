/*
 * Copyright (c) 2022 OpenLuat & AirM2M
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "air105_std_usb.h"
#include <stdio.h>

#define HANDLE			void *
#define USB_OTG ( (USB_TypeDef *) USB_BASE )
typedef struct
{
	const USB_TypeDef *RegBase;
	const int IrqLine;
	// USB_EndpointCtrlStruct EpCtrl[USB_EP_MAX];
	uint32_t RxData[USB_FIFO_DW_DAX];
	// PV_Union pRxData;
	uint8_t IsConnect;
	uint8_t IsHost;
	uint8_t NewDeviceAddress;
}USB_HWCtrlStruct;

// static USB_HWCtrlStruct prvUSB =
// {
// 		.RegBase = USB_BASE,
// 		.IrqLine = USB_IRQn,
// };

static uint8_t USB_OTG_FifosizeReg(uint16_t fifosiz)
{
    uint8_t register_value = 0;
    switch (fifosiz)
    {
	case 8:
		register_value = 0;
		break;
	case 16:
		register_value = 1;
		break;
	case 32:
		register_value = 2;
		break;
	case 64:
		register_value = 3;
		break;
	case 128:
		register_value = 4;
		break;
    }
    return register_value;
}

#if 0
static void prvUSB_IrqHandle(int32_t IrqLine, void *pData)
{
	uint32_t USB_ID = (uint32_t)pData;
	uint32_t i;
	uint32_t Count32b;
	uint16_t RxLen;
	uint16_t TxBit = USB_OTG->INTRTX;
	uint16_t RxBit = USB_OTG->INTRRX;
	uint8_t StateBit = USB_OTG->INTRUSB;
	uint8_t EpIndex = 1;
	USB_OTG->INTRUSB = 0;
	USB_OTG->INTRRX = 0;
	USB_OTG->INTRTX = 0;
	if (prvUSB.IsHost || USB_OTG->DEVCTL_b.host_mode)
	{
		;
	}
	else
	{

		if (StateBit & 0x1)
		{
			USB_StackDeviceBusChange(USB_ID, USBD_BUS_TYPE_SUSPEND);
		}
		if (StateBit & 0x2)
		{
			USB_StackDeviceBusChange(USB_ID, USBD_BUS_TYPE_RESUME);
			USB_OTG->POWER_b.resume = 1;
		}
		if (StateBit & 0x4)
		{
			USB_OTG->FADDR = 0;
			USB_OTG->INDEX = 0;
			USB_FlushFifo(&prvUSB, 0, 0);
			for(i = 1; i < 8; i++)
			{
				USB_FlushFifo(&prvUSB,i, 0);
				USB_FlushFifo(&prvUSB,i, 1);
			}
			USB_SetDeviceEPStatus(&prvUSB, 0, 0, USB_EP_STATE_ACK);
			USB_StackDeviceBusChange(USB_ID, USBD_BUS_TYPE_RESET);
		}
		if (StateBit & 0x8)
		{
			USB_StackDeviceBusChange(USB_ID, USBD_BUS_TYPE_NEW_SOF);
		}
		if (StateBit & 0x20)
		{
			for(i = 0; i < 8; i++)
			{
				memset(&prvUSB.EpCtrl[i].TxBuf, 0, sizeof(Buffer_Struct));
			}
			USB_StackDeviceBusChange(USB_ID, USBD_BUS_TYPE_DISCONNECT);
		}
		if (TxBit & 0x01)
		{
			if (USB_OTG->CSR0L_DEV_b.sent_stall)
			{
				USB_SetDeviceEPStatus(&prvUSB, 0, 0, USB_EP_STATE_ACK);
			}
			if (USB_OTG->CSR0L_DEV_b.setup_end)
			{
				USB_OTG->CSR0L_DEV_b.serviced_setupend = 1;
			}
            if (prvUSB.NewDeviceAddress & 0x80)
            {
            	USB_OTG->FADDR = prvUSB.NewDeviceAddress & 0x7f;
            	prvUSB.NewDeviceAddress = 0;
            }
			if (USB_OTG->CSR0L_DEV_b.rx_pkt_rdy)
			{
				RxLen = USB_OTG->COUNT0_b.ep0_rx_count;
				if (RxLen)
				{
					Count32b = RxLen >> 2;
				    for (i = 0; i < Count32b; i++)
				    {
				    	prvUSB.RxData[i] = USB_OTG->FIFOX[0].dword;
				    }
				    for (i = Count32b * 4; i < RxLen; i++)
				    {
				    	prvUSB.pRxData.pu8[i] = USB_OTG->FIFOX[0].byte;
				    }
				    USB_StackPutRxData(USB_ID, 0, prvUSB.pRxData.pu8, RxLen);
				}
				USB_OTG->CSR0L_DEV_b.serviced_rxpktrdy = 1;
			    USB_StackAnalyzeDeviceEpRx(USB_ID, 0);
			}
            else if (!USB_OTG->CSR0L_DEV_b.tx_pkt_rdy)
            {
            	USB_DeviceXfer(&prvUSB, 0);
            }

		}
		else if (TxBit)
		{
			TxBit >>= 1;
			EpIndex = 1;
			while(TxBit)
			{
				if (TxBit & 0x01)
				{
					if (!USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.tx_pkt_rdy)
					{
						USB_DeviceXfer(&prvUSB, EpIndex);
					}
					USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.sent_stall = prvUSB.EpCtrl[EpIndex].INSTATUS_b.Halt?1:0;
					USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.send_stall = 0;
					USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.under_run = 0;
				}
				TxBit >>= 1;
				EpIndex++;
			}
		}
		if (RxBit)
		{
			EpIndex = 1;
			RxBit >>= 1;
			while(RxBit)
			{
				if (RxBit & 0x01)
				{
					if (USB_OTG->CSR[EpIndex].RXCSRL_DEV_b.rx_pkt_rdy)
					{
						RxLen = USB_OTG->CSR[EpIndex].RXCOUNT_b.ep_rx_count;
						if (RxLen)
						{
							Count32b = RxLen >> 2;
						    for (i = 0; i < Count32b; i++)
						    {
						    	prvUSB.RxData[i] = USB_OTG->FIFOX[EpIndex].dword;
						    }
						    for (i = Count32b * 4; i < RxLen; i++)
						    {
						    	prvUSB.pRxData.pu8[i] = USB_OTG->FIFOX[EpIndex].byte;
						    }
						    USB_StackPutRxData(USB_ID, EpIndex, prvUSB.pRxData.pu8, RxLen);
						}
					}
					USB_OTG->CSR[EpIndex].RXCSRL = 0;
					USB_OTG->CSR[EpIndex].RXCSRL_DEV_b.sent_stall = prvUSB.EpCtrl[EpIndex].OUTSTATUS_b.Halt?1:0;
					USB_StackAnalyzeDeviceEpRx(USB_ID, EpIndex);
				}
				RxBit >>= 1;
				EpIndex++;
			}

		}
	}
}
#endif
static int prvUSB_SetupEPFifo(HANDLE hUSB)
{
	// USB_HWCtrlStruct *hwUSB = (USB_HWCtrlStruct *)hUSB;
	uint8_t i;
	uint8_t Len;
	uint16_t FifoStart = (64 >> 3);
	uint16_t UseFifo = 64;
	//ep0使用默认的64byte

	USB_OTG->INDEX = 0;
	USB_OTG->TXFIFOSZ = 3;
	USB_OTG->RXFIFOSZ = 3;
	USB_OTG->TXFIFOADD = 0;
	USB_OTG->RXFIFOADD = 0;
	USB_OTG->CSR0H_DEV_b.flush_fifo = 1;	//刷新EP0的FIFO
	// hwUSB->EpCtrl[0].MaxPacketLen = 64;
	for(i = 1; i < 4; i++)
	{
		USB_OTG->INDEX = i;
		// if (hwUSB->EpCtrl[i].ToDeviceEnable)
		{
			// Len = USB_OTG_FifosizeReg(hwUSB->EpCtrl[i].MaxPacketLen);
            Len = USB_OTG_FifosizeReg(64);
			// USB_OTG->CSR[i].RXMAXP = hwUSB->EpCtrl[i].MaxPacketLen;
            USB_OTG->CSR[i].RXMAXP = 64;
			USB_OTG->RXFIFOADD = FifoStart;
			USB_OTG->RXFIFOSZ = Len;
			USB_OTG->CSR[i].RXCSRL = (1 << 4);
			//DBG("%d,%d,%x,%d,%x", i, USB_OTG->CSR[i].RXMAXP, USB_OTG->RXFIFOADD, USB_OTG->RXFIFOSZ, USB_OTG->CSR[i].RXCSRL);
			// FifoStart += (hwUSB->EpCtrl[i].MaxPacketLen >> 3);
            FifoStart += (64 >> 3);
			// UseFifo += hwUSB->EpCtrl[i].MaxPacketLen;
            UseFifo += 64;
		}
		if (UseFifo > 512)
		{
			printf("to much fifo! \r\n");
			return -1;
		}
		// if (hwUSB->EpCtrl[i].ToHostEnable)
		// {
		// 	USB_OTG->CSR[i].TXMAXP = hwUSB->EpCtrl[i].MaxPacketLen;
		// 	USB_OTG->TXFIFOADD = FifoStart;
		// 	USB_OTG->TXFIFOSZ = Len;
		// 	USB_OTG->CSR[i].TXCSRL = (1 << 3);
		// 	//DBG("%d,%d,%x,%d,%x", i, USB_OTG->CSR[i].TXMAXP, USB_OTG->TXFIFOADD, USB_OTG->TXFIFOSZ, USB_OTG->CSR[i].TXCSRL);
		// 	FifoStart += (hwUSB->EpCtrl[i].MaxPacketLen >> 3);
		// 	UseFifo += hwUSB->EpCtrl[i].MaxPacketLen;
		// }
		// if (UseFifo > 512)
		// {
		// 	DBG("to much fifo!");
		// 	return -1;
		// }
	}
	USB_OTG->INDEX = 0;
	return 0;
}

void USB_SetWorkMode(HANDLE hUSB, uint8_t Mode);

void USB_PowerOnOff(HANDLE hUSB, uint8_t OnOff);

void air105_usb_flush_fifo(uint8_t ep_index, uint8_t dir);
void USB_ResetStart(HANDLE hUSB);
void USB_ResetEnd(HANDLE hUSB);

void USB_GlobalInit(void)
{
    USB_PowerOnOff(0,0);
    NVIC_SetPriority(USB_IRQn, 7);
	USB_OTG->INTRRXE = 0;
	USB_OTG->INTRTXE = 0;
	USB_OTG->INTRUSBE = 0;
	// prvUSB.pRxData.pu32 = prvUSB.RxData;
    prvUSB_SetupEPFifo(0);

    air105_usb_flush_fifo(0, 0);
    for (uint8_t i = 1; i < 8; i++)
    {
        air105_usb_flush_fifo(i, 0);
        air105_usb_flush_fifo(i, 1);
    }

    SYSCTRL->CG_CTRL2 &= ~SYSCTRL_AHBPeriph_USB;
    USB_PowerOnOff(0,1);
    NVIC_EnableIRQ(USB_IRQn);

    USB_ResetEnd(0);
    USB_SetWorkMode(0,1);
    uint32_t cout = 100000;
    while (cout--)
    {
        /* code */
    }
    USB_Start(0);
}

void USB_PowerOnOff(HANDLE hUSB, uint8_t OnOff)
{
	if (OnOff)
	{
		SYSCTRL->CG_CTRL2 |= SYSCTRL_AHBPeriph_USB;
	}
	else
	{
		SYSCTRL->CG_CTRL2 &= ~SYSCTRL_AHBPeriph_USB;
	}
}

void USB_Stop(HANDLE hUSB)
{
	uint8_t dummy;
	USB_HWCtrlStruct *hwUSB = (USB_HWCtrlStruct *)hUSB;
	// ISR_OnOff(hwUSB->IrqLine, 0);
	USB_OTG->POWER = 0;
	USB_OTG->INTRRXE = 0;
	USB_OTG->INTRTXE = 0;
	USB_OTG->INTRUSBE = 0;
	USB_OTG->INTRTX = 0;
	USB_OTG->INTRRX = 0;
	USB_OTG->INTRUSB = 0;
	dummy = USB_OTG->INTRUSB;
}

void USB_ResetStart(HANDLE hUSB)
{
	SYSCTRL->LOCK_R &= ~SYSCTRL_USB_RESET;
    SYSCTRL->SOFT_RST2 |= SYSCTRL_USB_RESET;
    SYSCTRL->LOCK_R |= SYSCTRL_USB_RESET;
}

void USB_ResetEnd(HANDLE hUSB)
{
	SYSCTRL->LOCK_R &= ~SYSCTRL_USB_RESET;
    SYSCTRL->SOFT_RST2 &= ~SYSCTRL_USB_RESET;
    SYSCTRL->LOCK_R |= SYSCTRL_USB_RESET;
}

void USB_SetWorkMode(HANDLE hUSB, uint8_t Mode)
{
	USB_HWCtrlStruct *hwUSB = (USB_HWCtrlStruct *)hUSB;
	USBPHY_CR1_TypeDef CR1;
	USBPHY_CR3_TypeDef CR3;
	CR1.d32 = SYSCTRL->USBPHY_CR1;
	CR1.b.commononn = 0;
	CR1.b.stop_ck_for_suspend = 0;
	SYSCTRL->USBPHY_CR1 = CR1.d32;

	switch (Mode)
	{
	case 0:
		USB_OTG->POWER = 1;
		USB_OTG->DEVCTL |= (1 << 0)|(1 << 2);
		CR3.d32 = SYSCTRL->USBPHY_CR3;
		CR3.b.idpullup = 1;
		SYSCTRL->USBPHY_CR3 = CR3.d32;
		// hwUSB->IsHost = 1;
		break;
    case 1:
		USB_OTG->DEVCTL &= ~((1 << 0)|(1 << 2));
		USB_OTG->POWER = 1|(1 << 6);
		CR3.d32 = SYSCTRL->USBPHY_CR3;
		CR3.b.idpullup = 0;
		SYSCTRL->USBPHY_CR3 = CR3.d32;
		// hwUSB->IsHost = 0;
		break;
	}
}

void USB_SetDeviceAddress(HANDLE hUSB, uint8_t Address)
{
	// prvUSB.NewDeviceAddress = 0x80 | Address;
}

int USB_ReInitEpCfg(HANDLE hUSB)
{
	return -1;
}

int USB_InitEpCfg(HANDLE hUSB)
{
	return prvUSB_SetupEPFifo(hUSB);
}

void USB_Start(HANDLE hUSB)
{
	USB_HWCtrlStruct *hwUSB = (USB_HWCtrlStruct *)hUSB;
	USB_OTG->INTRUSBE = 0xff;
	USB_OTG->INTRUSBE_b.en_sof = 0;
	USB_OTG->INTRTXE = 0xff;
	USB_OTG->INTRRXE = 0xfe;
	// ISR_OnOff(hwUSB->IrqLine, 1);
}

void USB_SendZeroPacket(HANDLE hUSB, uint8_t EpIndex)
{
	if (EpIndex)
	{
		USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.tx_pkt_rdy = 1;
	}
	else
	{
		USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 1;
	}
}

void USB_DeviceXferStop(HANDLE hUSB, uint8_t EpIndex)
{
	USB_HWCtrlStruct *hwUSB = (USB_HWCtrlStruct *)hUSB;
	USB_OTG->INTRTXE &= ~(1 << EpIndex);
	// USB_FlushFifo(hUSB, EpIndex, 0);
	USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.send_stall = 0;
	USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.sent_stall = 0;
	USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.under_run = 0;
	USB_OTG->INTRTXE |= (1 << EpIndex);
}

#if 0
void USB_DeviceXfer(HANDLE hUSB, uint8_t EpIndex)
{
	USB_HWCtrlStruct *hwUSB = (USB_HWCtrlStruct *)hUSB;
	USB_EndpointCtrlStruct *pEpCtrl = &hwUSB->EpCtrl[EpIndex];
	USB_EndpointDataStruct EpData;
	uint16_t TxLen, i;


	if (pEpCtrl->TxBuf.Data)
	{
		if (pEpCtrl->TxBuf.Pos >= pEpCtrl->TxBuf.MaxLen)
		{
			if (pEpCtrl->NeedZeroPacket)
			{
//				DBG("Ep%d %d %d %d need send 0 packet", EpIndex,
//						pEpCtrl->ForceZeroPacket, pEpCtrl->TxBuf.Pos, pEpCtrl->XferMaxLen);
				if (!pEpCtrl->TxZeroPacket)
				{
					pEpCtrl->TxZeroPacket = 1;
					if (EpIndex)
					{
						USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.tx_pkt_rdy = 1;
					}
					else
					{
						USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 1;
					}
				}
				else
				{
					DBG("!");
				}

			}
			goto XFER_DONE;
		}
		else
		{
			pEpCtrl->TxZeroPacket = 0;
			if ((pEpCtrl->TxBuf.MaxLen - pEpCtrl->TxBuf.Pos) > pEpCtrl->MaxPacketLen)
			{
				TxLen = pEpCtrl->MaxPacketLen;
			}
			else
			{
				if (pEpCtrl->ForceZeroPacket)
				{
					pEpCtrl->NeedZeroPacket = 1;
				}
				if ((pEpCtrl->TxBuf.MaxLen - pEpCtrl->TxBuf.Pos) == pEpCtrl->MaxPacketLen)
				{
					if ( pEpCtrl->TxBuf.MaxLen < pEpCtrl->XferMaxLen )
					{
						pEpCtrl->NeedZeroPacket = 1;
					}
				}
				TxLen = pEpCtrl->TxBuf.MaxLen - pEpCtrl->TxBuf.Pos;
			}


			for(i = pEpCtrl->TxBuf.Pos; i < pEpCtrl->TxBuf.Pos + TxLen; i++)
			{
				USB_OTG->FIFOX[EpIndex].byte = pEpCtrl->TxBuf.Data[i];
			}
			pEpCtrl->TxBuf.Pos += TxLen;
			if (EpIndex)
			{
				USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.tx_pkt_rdy = 1;
			}
			else
			{
				USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 1;
			}
		}
	}
	return;
XFER_DONE:
	memset(&pEpCtrl->TxBuf, 0, sizeof(Buffer_Struct));
	if (EpIndex)
	{
		memset(&EpData, 0, sizeof(EpData));
		EpData.USB_ID = USB_ID0;
		EpData.EpIndex = EpIndex;
		pEpCtrl->CB(&EpData, pEpCtrl->pData);
	}
	else
	{
		USB_StackDeviceEp0TxDone(USB_ID0);
	}
}
#endif



/*!< ------------------------------------------------ */

void air105_usb_dc_init(void)
{
}

void air105_usb_ep_config(uint8_t ep_addr, uint8_t ep_type,
                          uint8_t fifo_start_add, uint8_t ep_mps)
{
}

/**
* @brief  Set the EP Status
* @param  pdev : Selected device
*         Status : new Status
*         ep : EP structure
* @retval : None
*/
void air105_usb_set_ep_status(uint8_t index, uint8_t dir, uint8_t status)
{
    if (index)
    {
        if (!dir)
        {
            /*!< Out ep */
            switch (status)
            {
            case USB_EP_STATE_DISABLE:
            case USB_EP_STATE_NAK:
            case USB_EP_STATE_NYET:
                break;
            case USB_EP_STATE_STALL:
                printf("%d", index);
                USB_OTG->CSR[index].RXCSRL_DEV_b.send_stall = 1;
                break;
            case USB_EP_STATE_ENABLE:
            case USB_EP_STATE_ACK:
                USB_OTG->CSR[index].RXCSRL_DEV_b.send_stall = 0;
                USB_OTG->CSR[index].RXCSRL_DEV_b.sent_stall = 0;
                USB_OTG->CSR[index].RXCSRL_DEV_b.over_run = 0;
                USB_OTG->CSR[index].RXCSRL_DEV_b.data_error = 0;
                break;
            }
        }
        else
        {
            /*!< In ep */
            switch (status)
            {
            case USB_EP_STATE_DISABLE:
            case USB_EP_STATE_NAK:
            case USB_EP_STATE_NYET:
                break;
            case USB_EP_STATE_STALL:
                printf("%d", index);
                USB_OTG->CSR[index].TXCSRL_DEV_b.send_stall = 1;
                break;
            case USB_EP_STATE_ENABLE:
            case USB_EP_STATE_ACK:
                USB_OTG->CSR[index].TXCSRL_DEV_b.send_stall = 0;
                USB_OTG->CSR[index].TXCSRL_DEV_b.sent_stall = 0;
                USB_OTG->CSR[index].TXCSRL_DEV_b.under_run = 0;
                break;
            }
        }
    }
    else
    {
        switch (status)
        {
        case USB_EP_STATE_DISABLE:
        case USB_EP_STATE_NYET:
        case USB_EP_STATE_NAK:
            break;
        case USB_EP_STATE_STALL:
            USB_OTG->CSR0L_DEV_b.send_stall = 1;
            break;
        case USB_EP_STATE_ACK:
        case USB_EP_STATE_ENABLE:
            USB_OTG->CSR0L_DEV_b.send_stall = 0;
            USB_OTG->CSR0L_DEV_b.sent_stall = 0;
            break;
        }
    }
}

/**
* @brief  returns the EP Status
* @param  pdev : Selected device
*         ep : endpoint structure
* @retval : EP status
*/
uint8_t air105_usb_get_ep_status(uint8_t index, uint8_t dir)
{
	if (index)
    {
	    if (!dir)
	    {
            /*!< Out ep */
    		if (USB_OTG->CSR[index].RXCSRL_DEV_b.sent_stall)
    		{
    			return USB_EP_STATE_STALL;
    		}
    		if (USB_OTG->CSR[index].RXCSRL_DEV_b.data_error || USB_OTG->CSR[index].RXCSRL_DEV_b.over_run)
    		{
    			return USB_EP_STATE_NAK;
    		}
    		return USB_EP_STATE_ACK;
    	}
	    else
	    {
            /*!< In ep */
    		if (USB_OTG->CSR[index].TXCSRL_DEV_b.sent_stall)
    		{
    			return USB_EP_STATE_STALL;
    		}
    		if (USB_OTG->CSR[index].TXCSRL_DEV_b.under_run || USB_OTG->CSR[index].TXCSRL_DEV_b.tx_pkt_rdy)
    		{
    			return USB_EP_STATE_NAK;
    		}
    		return USB_EP_STATE_ACK;
	    }
    }
    else
    {
		if (USB_OTG->CSR0L_DEV_b.sent_stall)
		{
			return USB_EP_STATE_STALL;
		}
		return USB_EP_STATE_ACK;
    }
}

void air105_usb_resume_start(void)
{
	USB_OTG->POWER_b.resume = 1;
}

void air105_usb_resume_end(void)
{
	USB_OTG->POWER_b.resume = 0;
}

void air105_usb_flush_fifo(uint8_t ep_index, uint8_t dir)
{
    if (ep_index)
    {
        if (!dir)
        {
            USB_OTG->CSR[ep_index].RXCSRL = (1 << 4);
        }
        else
        {

            USB_OTG->CSR[ep_index].TXCSRL = (1 << 3);
        }
    }
    else
    {
        if (USB_OTG->CSR0L_DEV_b.rx_pkt_rdy || USB_OTG->CSR0L_DEV_b.tx_pkt_rdy)
        {
            USB_OTG->CSR0H_DEV_b.flush_fifo = 1;
            USB_OTG->CSR0L_DEV_b.serviced_rxpktrdy = 1;
            USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 0;
        }
    }
}

int32_t air105_usb_set_isoch_delay(uint16_t delay)
{
   (void*)delay;
   return -1;
}

int32_t air105_usb_exit_latency(uint8_t config[6])
{
    (void*)config;
	return -1;
}

void air105_usb_set_ep0_nodata_setup(void) 
{
  USB_OTG->CSR0L_DEV_b.data_end = 1;
}

void air105_usb_ep_int_on_off(uint8_t ep_index, uint8_t dir, uint8_t on_off)
{
	if (!dir)
	{
		if (on_off)
		{
			USB_OTG->INTRRXE |= (1 << ep_index);
		}
		else
		{
			USB_OTG->INTRRXE &= ~(1 << ep_index);
		}
	}
	else
	{
		if (on_off)
		{
			USB_OTG->INTRTXE |= (1 << ep_index);
		}
		else
		{
			USB_OTG->INTRTXE &= ~(1 << ep_index);
		}
	}
}

void air105_usb_send_zero_packet(uint8_t ep_index)
{
    if (ep_index)
    {
        USB_OTG->CSR[ep_index].TXCSRL_DEV_b.tx_pkt_rdy = 1;
    }
    else
    {
        USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 1;
    }
}

void air105_usb_start(void)
{
	USB_OTG->INTRUSBE = 0xff;
	USB_OTG->INTRUSBE_b.en_sof = 0;
	USB_OTG->INTRTXE = 0xff;
	USB_OTG->INTRRXE = 0xfe;
	// ISR_OnOff(hwUSB->IrqLine, 1);
}

#define SETUP_STAGE 0 
#define DATA_TO_HOST 1
#define DATA_TO_DEVICE 2
volatile int8_t ep0_stage = 0; /* 0setup  1tohost  2to dev */

static const uint8_t hid_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(0x0200, 0x00, 0x00, 0x00, 0x0903, 0x0220, 0x0002, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x01,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                          /* bCountryCode: Hardware target country */
    0x01,                          /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                          /* bDescriptorType */
    HID_KEYBOARD_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    HID_INT_EP,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    HID_INT_EP_SIZE,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    HID_INT_EP_INTERVAL, /* bInterval: Polling Interval */
    /* 34 */
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x14,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x26,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'C', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'e', 0x00,                  /* wcChar2 */
    'r', 0x00,                  /* wcChar3 */
    'r', 0x00,                  /* wcChar4 */
    'y', 0x00,                  /* wcChar5 */
    'U', 0x00,                  /* wcChar6 */
    'S', 0x00,                  /* wcChar7 */
    'B', 0x00,                  /* wcChar8 */
    ' ', 0x00,                  /* wcChar9 */
    'H', 0x00,                  /* wcChar10 */
    'I', 0x00,                  /* wcChar11 */
    'D', 0x00,                  /* wcChar12 */
    ' ', 0x00,                  /* wcChar13 */
    'D', 0x00,                  /* wcChar14 */
    'E', 0x00,                  /* wcChar15 */
    'M', 0x00,                  /* wcChar16 */
    'O', 0x00,                  /* wcChar17 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
    0x00
};

uint32_t rx_buff_32[128];
uint8_t rx_buff_8[512];

uint8_t device_desc[18] = {0x12, 0x01, 0x00, 0x02, 0x00, 0x00,
                           0x00, 0x40, 0x09, 0x03, 0x02, 0x02,
                           0x00, 0x02, 0x01, 0x02, 0x03, 0x01};

uint8_t config_desc[]={
	0x09,
	0x02,
	34,
	0x00,
	0x01,
	0x01,
	0x00,
	0x80,
	50,
	/* */
	0x09,
	0x04,
	0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x01,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x01,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x01,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */

	/******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    0x21, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                          /* bCountryCode: Hardware target country */
    0x01,                          /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                          /* bDescriptorType */
    63,                            /* wItemLength: Total length of Report descriptor */
    0x00,

	/******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07,                         /* bLength: Endpoint Descriptor size */
    0x05U, /* bDescriptorType: */
    0x81,                   /* bEndpointAddress: Endpoint Address (IN) */
    0x03,                         /* bmAttributes: Interrupt endpoint */
    4,              /* wMaxPacketSize: 4 Byte max */
    0x00,
    1 /* bInterval: Polling Interval */
    /* 34 */
}

uint8_t string1[] = {
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    0x04,                           /* bLength */     
    0x03U, /* bDescriptorType */ 
	1033 & 0xff,
	1033 >> 8,
}

void USB_IRQHandler(void)
{
    uint32_t i;
    uint32_t Count32b;
    uint16_t RxLen;

    /*!< 这个寄存器也是只读的 说明读一次会自动清除中断标志位 */
    volatile uint8_t usb_it_state = USB_OTG->INTRUSB;

    /*!< INTRRX INTRTX 读一次就会被清除 */
    volatile uint16_t ep_out_it_state = USB_OTG->INTRRX;
    volatile uint16_t ep_in_it_state = USB_OTG->INTRTX;

    uint8_t EpIndex = 1;
    USB_OTG->INTRUSB = 0;
    USB_OTG->INTRRX = 0;
    USB_OTG->INTRTX = 0;

    if (usb_it_state & USB_Suspend_Mask)
    {
        /*!< Suspend */
        printf("Suspend \r\n");
    }

    if (usb_it_state & USB_Resume_Mask)
    {
        /*!< Resume */
        printf("Resume \r\n");
        USB_OTG->POWER_b.resume = 1;
    }

    if (usb_it_state & USB_Reset_Mask)
    {
        /*!< Reset */
        /*!< Process */
        USB_OTG->FADDR = 0;
        USB_OTG->INDEX = 0;
        printf("Reset \r\n");
        /**
         * EP0 refresh flush doesn't matter direction
         * Fill in any out direction here
         */
        // usb_flush_fifo(0x00, EP_DIR_OUT);
        for (i = 1; i < 8; i++)
        {
            // usb_flush_fifo(i, EP_DIR_OUT);
            // usb_flush_fifo(i, EP_DIR_IN);
        }
        // PM_SetDriverRunFlag(PM_DRV_USB, 1);
        // air105_set_ep_ack(0x00);
        /*!<  */
        // usbd_event_notify_handler(USBD_EVENT_RESET, NULL);
    }

    if (usb_it_state & USB_SOF_Mask)
    {
        /*!< Sof */
        // printf("Sof \r\n");
    }

    if (usb_it_state & USB_Discon_Mask)
    {
        /*!< Diconnect */
        printf("Diconnect \r\n");
    }

    if (ep_in_it_state & EP0_IT_Mask)
    {
        /*!< ep 0 interrupt */

        /*!< ep 0 发送 stall 产生的中断 */
        if (USB_OTG->CSR0L_DEV_b.sent_stall)
        {
            // air105_set_ep_ack(0x00);
        }
        /*!< 主机发送 setup令牌产生的中断 */
        if (USB_OTG->CSR0L_DEV_b.setup_end)
        {
            printf("Setup end \r\n");
            /*!< 控制事务完成 */
            // usb_dc_cfg.is_setup = 1;
			ep0_stage = SETUP_STAGE;
            /*!< 清除 setup_end 标志位 */
            USB_OTG->CSR0L_DEV_b.serviced_setupend = 1;
        }

        /*!< 主机下发数据产生的中断 */
        if (USB_OTG->CSR0L_DEV_b.rx_pkt_rdy)
        {
            /*!< 获取接收字节的长度 */
            RxLen = USB_OTG->COUNT0_b.ep0_rx_count;
            printf("rxlen :%d\r\n",RxLen);
            if (RxLen)
            {
                Count32b = RxLen >> 2; // 除4
				for (i = 0; i < RxLen; i++)
				{
					rx_buff_8[i] = USB_OTG->FIFOX[0].byte;
					printf("%02x ",rx_buff_8[i]);
				}
				printf("\r\n");
            }
			else
			{
				printf("0 length packet \r\n");
			}
            /*!< Write 1 to serviced_rxpktrdy to clear rx_pkt_rdy */
            USB_OTG->CSR0L_DEV_b.serviced_rxpktrdy = 1;
			if (ep0_stage == SETUP_STAGE)
			{
			    /*!< setup handler */
			    // usbd_event_notify_handler(USBD_EVENT_SETUP, NULL);
			    // usb_dc_cfg.is_setup = 0;
			    for (uint16_t count = 0; count < 18; count++)
				{
			       USB_OTG->FIFOX[0].byte = device_desc[count];
			    }
				ep0_stage = DATA_TO_HOST;
				USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 1;
			}
			else
			{
			    /*!< ep 0 out handler */
			    // usbd_event_notify_handler(USBD_EVENT_EP0_HANDLER, NULL);
			}
		}
        else if (!USB_OTG->CSR0L_DEV_b.tx_pkt_rdy)
        {
            /*!< 发送成功中断 */
			printf("tx success\r\n");
            // usbd_event_notify_handler(USBD_EVENT_EP0_IN_HANDLER, NULL);
        }
        else if (USB_OTG->CSR0L_DEV_b.tx_pkt_rdy)
        {
            /*!< do noting */
            /*!< 自动清除 */
        }
    }
    else if (ep_in_it_state)
    {
        /*!< 1--7 in */
        ep_in_it_state >>= 1;
        EpIndex = 1;
        while (ep_in_it_state)
        {
            if (ep_in_it_state & 0x01)
            {
                if (!USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.tx_pkt_rdy)
                {
                    /*!< 发送成功中断 */
                    // usbd_event_notify_handler(USBD_EVENT_EP_IN_HANDLER, NULL);
                }
                // USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.sent_stall =
                // prvUSB.EpCtrl[EpIndex].INSTATUS_b.Halt ? 1 : 0;
                if (USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.sent_stall == 1)
                {
                    USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.sent_stall = 0;
                }

                USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.send_stall = 0;

                if (USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.under_run == 1)
                {
                    /*!< tx_rdy 没有设置 但是主机发了in令牌 */
                    USB_OTG->CSR[EpIndex].TXCSRL_DEV_b.under_run = 0;
                }
                /*!<   */
            }
            ep_in_it_state >>= 1;
            EpIndex++;
        }
    }

    /*!< 1--7 out interrupt */
    if (ep_out_it_state)
    {
        EpIndex = 1;
        ep_out_it_state >>= 1;
        while (ep_out_it_state)
        {
            if (ep_out_it_state & 0x01)
            {
                if (USB_OTG->CSR[EpIndex].RXCSRL_DEV_b.rx_pkt_rdy)
                {
                    RxLen = USB_OTG->CSR[EpIndex].RXCOUNT_b.ep_rx_count;
                    if (RxLen)
                    {
                        Count32b = RxLen >> 2;
                        for (i = 0; i < Count32b; i++)
                        {
                            // prvUSB.RxData[i] = USB_OTG->FIFOX[EpIndex].dword;
                            // usb_dc_cfg.rx_data[i] = USB_OTG->FIFOX[EpIndex].dword;
                        }
                        for (i = Count32b * 4; i < RxLen; i++)
                        {
                            // prvUSB.pRxData.pu8[i] = USB_OTG->FIFOX[EpIndex].byte;
                        }
                        // USB_StackPutRxData(USB_ID, EpIndex, prvUSB.pRxData.pu8, RxLen);
                        // usbd_event_notify_handler(USBD_EVENT_EP_OUT_HANDLER, (uint32_t
                        // *)(EpIndex|0x00));
                    }
                }
                USB_OTG->CSR[EpIndex].RXCSRL = 0;
                // USB_OTG->CSR[EpIndex].RXCSRL_DEV_b.sent_stall =
                // prvUSB.EpCtrl[EpIndex].OUTSTATUS_b.Halt ? 1 : 0;
                if (USB_OTG->CSR[EpIndex].RXCSRL_DEV_b.sent_stall == 1)
                {
                    USB_OTG->CSR[EpIndex].RXCSRL_DEV_b.sent_stall = 0;
                }
                // USB_StackAnalyzeDeviceEpRx(USB_ID, EpIndex);
            }
            ep_out_it_state >>= 1;
            EpIndex++;
        }
    }
}