#include "air105_usb.h"

#define USB_BASE 0x40000C00UL
#define USB_OTG ((USB_TypeDef *)USB_BASE)

/*!< ep dir in */
#define EP_DIR_IN 1
/*!< ep dir out */
#define EP_DIR_OUT 0
/*!< get ep id by epadd */
#define GET_EP_ID(ep_add) (uint8_t)(ep_add & 0x7f)
/*!< get ep dir by epadd */
#define GET_EP_DIR(ep_add) (uint8_t)(ep_add & 0x80)
/*!< ep nums */
#define EP_NUMS 16
/*!< ep mps */
#define EP_MPS 64

#define USB_FIFO_DW_DAX 128

/**
 * @brief   Endpoint information structure
 */
typedef struct _usbd_ep_info
{
    uint8_t mps;          /*!< Maximum packet length of endpoint */
    uint8_t eptype;       /*!< Endpoint Type */
    uint8_t *ep_ram_addr; /*!< Endpoint buffer address */
} usbd_ep_info;

static struct _air105_core_prvi
{
    uint8_t address;              /*!< address */
    usbd_ep_info ep_in[EP_NUMS];  /*!< ep in */
    usbd_ep_info ep_out[EP_NUMS]; /*!< ep out */
    //   struct usb_setup_packet setup; /*!< Setup package that may be used in
    //   interrupt processing (outside the protocol stack) */
    uint32_t rx_data[USB_FIFO_DW_DAX];
    volatile uint8_t is_setup;
    uint16_t fifo_start;
    uint16_t use_fifo;
} usb_dc_cfg;

/**
 * @brief   Set ep ack
 */
static inline void air105_set_ep_ack(uint8_t ep_add)
{
    uint8_t epid = GET_EP_ID(ep_add);
    uint8_t ep_dir = GET_EP_DIR(ep_add);
    if (epid == 0)
    {
        /*!< ep 0 */
        USB_OTG->CSR0L_DEV_b.send_stall = 0;
        USB_OTG->CSR0L_DEV_b.sent_stall = 0;
    }
    else
    {
        if (ep_dir == EP_DIR_IN)
        {
            USB_OTG->CSR[epid].TXCSRL_DEV_b.send_stall = 0;
            USB_OTG->CSR[epid].TXCSRL_DEV_b.sent_stall = 0;
            USB_OTG->CSR[epid].TXCSRL_DEV_b.under_run = 0;
        }
        else
        {
            USB_OTG->CSR[epid].RXCSRL_DEV_b.send_stall = 0;
            USB_OTG->CSR[epid].RXCSRL_DEV_b.sent_stall = 0;
            USB_OTG->CSR[epid].RXCSRL_DEV_b.over_run = 0;
            USB_OTG->CSR[epid].RXCSRL_DEV_b.data_error = 0;
        }
    }
}

/**
 * @brief   Flush fifo
 */
static void usb_flush_fifo(uint8_t epid, uint8_t ep_dir)
{
    if (epid)
    {
        if (ep_dir == EP_DIR_IN)
        {
            /*!< in end point */
            USB_OTG->CSR[epid].TXCSRL = (1 << 3);
        }
        else
        {
            /*!< out end point */
            USB_OTG->CSR[epid].RXCSRL = (1 << 4);
        }
    }
    else
    {
        /*!< ep 0 */
        if (USB_OTG->CSR0L_DEV_b.rx_pkt_rdy || USB_OTG->CSR0L_DEV_b.tx_pkt_rdy)
        {
            USB_OTG->CSR0H_DEV_b.flush_fifo = 1;
            USB_OTG->CSR0L_DEV_b.serviced_rxpktrdy = 1;
            USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 0;
        }
    }
}

/**
 * @brief
 */
static uint8_t usb_otg_fifosize_reg(uint16_t fifosiz)
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

/**
 * @brief            Set address
 * @pre              None
 * @param[in]        address 8-bit valid address
 * @retval           >=0 success otherwise failure
 */
int usbd_set_address(const uint8_t address)
{
    if (address == 0)
    {
        /*!< init 0 address */
    }
    else
    {
        /*!< For non-0 addresses, write the address to the register in the state
         * phase of setting the address */
        
    }
    return 0;
}

/**
 * @brief            Open endpoint
 * @pre              None
 * @param[in]        ep_cfg : Endpoint configuration structure pointer
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_open(const struct usbd_endpoint_cfg *ep_cfg)
{
    if (usb_dc_cfg.use_fifo > 512)
    {
        return -1;
    }

    /*!< ep id */
    uint8_t epid = GET_EP_ID(ep_cfg->ep_addr);

    /*!< 选中需要操作的端点 */
    USB_OTG->INDEX = epid;

    /*!< ep dir */
    bool dir = GET_EP_DIR(ep_cfg->ep_addr);
    /*!< ep max packet length */
    uint8_t mps = ep_cfg->ep_mps;

    if (mps < 8)
    {
        /*!< 最小包长为设为8字节 */
        mps = 8;
    }

    if (dir == EP_DIR_IN)
    {
        /*!< In */
        /*!< 配置fifo */
        usb_dc_cfg.ep_in[epid].mps = mps;
        USB_OTG->CSR[epid].TXMAXP = mps;
        USB_OTG->TXFIFOADD = usb_dc_cfg.fifo_start;
        USB_OTG->TXFIFOSZ = usb_otg_fifosize_reg(mps);
        /*!< flush fifo */
        USB_OTG->CSR[epid].TXCSRL = (1 << 3);

        /*!< 8字节为单位的fifo起始地址
             fifo start 往后偏移 mps / 8 个单位
         */
        usb_dc_cfg.fifo_start += (mps >> 3);

        /*!< 更行已经使用的fifo大小 */
        usb_dc_cfg.use_fifo += mps;

        /*!< 使能端点中断 */
        USB_OTG->INTRTXE |= (1 << epid);
    }
    else if (dir == EP_DIR_OUT)
    {
        /*!< Out */
        /*!< 配置fifo */
        usb_dc_cfg.ep_out[epid].mps = mps;
        USB_OTG->CSR[epid].RXMAXP = mps;
        USB_OTG->RXFIFOADD = usb_dc_cfg.fifo_start;
        USB_OTG->RXFIFOSZ = usb_otg_fifosize_reg(mps);
        /*!< flush fifo */
        USB_OTG->CSR[epid].RXCSRL = (1 << 4);
        /*!< 8字节为单位的fifo起始地址
             fifo start 往后偏移 mps / 8 个单位
        */
        usb_dc_cfg.fifo_start += (mps >> 3);

        /*!< 更行已经使用的fifo大小 */
        usb_dc_cfg.use_fifo += mps;

        /*!< 使能端点中断 */
        USB_OTG->INTRRXE |= (1 << epid);
    }

    USB_OTG->INDEX = 0;
    return 0;
}

/**
 * @brief            Close endpoint
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_close(const uint8_t ep)
{
    /*!< ep id */
    uint8_t epid = GET_EP_ID(ep);

    USB_OTG->INTRTXE &= ~(1 << epid);
    USB_OTG->INTRRXE &= ~(1 << epid);

    usb_dc_cfg.use_fifo -= usb_dc_cfg.ep_in[epid].mps;
    usb_dc_cfg.use_fifo -= usb_dc_cfg.ep_out[epid].mps;

    usb_dc_cfg.fifo_start -= usb_dc_cfg.ep_out[epid].mps >> 3;
    usb_dc_cfg.fifo_start -= usb_dc_cfg.ep_in[epid].mps >> 3;

    return 0;
}

/**
 * @brief            Write send buffer
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @param[in]        data ： First address of data buffer to be written
 * @param[in]        data_len ： Write total length
 * @param[in]        ret_bytes ： Length actually written
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_write(const uint8_t ep, const uint8_t *data, uint32_t data_len,
                  uint32_t *ret_bytes)
{
    /*!< ep id */
    uint8_t epid = GET_EP_ID(ep);
    /*!< real write byte nums */
    uint32_t real_wt_nums = 0;
    /*!< ep mps */
    uint8_t ep_mps = usb_dc_cfg.ep_in[epid].mps;
    /*!< Analyze bytes actually written */
    if (data == NULL && data_len > 0)
    {
        return -1;
    }

    if (data_len == 0)
    {
        /*!< write 0 len data */
        if (epid)
        {
            USB_OTG->CSR[epid].TXCSRL_DEV_b.tx_pkt_rdy = 1;
        }
        else
        {
            USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 1;
        }
        return 0;
    }

    if (data_len > ep_mps)
    {
        /*!< The data length is greater than the maximum packet length of the
         * endpoint */
        real_wt_nums = ep_mps;
    }
    else
    {
        real_wt_nums = data_len;
    }

    /*!< write buff start */
    for (uint16_t i = 0; i < real_wt_nums; i++)
    {
        USB_OTG->FIFOX[epid].byte = data[i];
    }
    /*!< write buff over */

    /**
     * Note that starting DMA transmission is to transmit data to USB peripherals,
     * and then wait for the host to get it
     */
    /*!< Start transfer */
    if (epid)
    {
        USB_OTG->CSR[epid].TXCSRL_DEV_b.tx_pkt_rdy = 1;
    }
    else
    {
        USB_OTG->CSR0L_DEV_b.tx_pkt_rdy = 1;
    }

    if (ret_bytes != NULL)
    {
        *ret_bytes = real_wt_nums;
    }

    return 0;
}

/**
 * @brief            Read receive buffer
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @param[in]        data ： Read the first address of the buffer where the data
 * is stored
 * @param[in]        max_data_len ： Maximum readout length
 * @param[in]        read_bytes ： Actual read length
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_read(const uint8_t ep, uint8_t *data, uint32_t max_data_len,
                 uint32_t *read_bytes)
{
    /*!< ep id */
    uint8_t epid = GET_EP_ID(ep);
    /*!< real read byte nums */
    uint32_t real_rd_nums = 0;
    /*!< ep mps */
    uint8_t ep_mps = usb_dc_cfg.ep_out[epid].mps;

    if (data == NULL && max_data_len > 0)
    {
        return -1;
    }

    if (max_data_len == 0)
    {
        return 0;
    }

    if (max_data_len > ep_mps)
        max_data_len = ep_mps;

    real_rd_nums = USB_OTG->COUNT0_b.ep0_rx_count;
    real_rd_nums = MIN(real_rd_nums, max_data_len);

    /*!< read buff start */
    for (uint8_t i = 0; i < real_rd_nums; i++)
    {
        usb_dc_cfg.ep_out[epid].ep_ram_addr[i] = USB_OTG->FIFOX[epid].byte;
    }

    if (read_bytes != NULL)
    {
        *read_bytes = real_rd_nums;
    }

    return 0;
}

/**
 * @brief            Endpoint setting pause
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_set_stall(const uint8_t ep)
{
    uint8_t epid = GET_EP_ID(ep);
    uint8_t ep_dir = GET_EP_DIR(ep);
    if (epid == 0)
    {
        USB_OTG->CSR0L_DEV_b.send_stall = 1;
    }
    else
    {
        if (ep_dir == EP_DIR_IN)
        {
            USB_OTG->CSR[epid].TXCSRL_DEV_b.send_stall = 1;
        }
        else
        {
            USB_OTG->CSR[epid].RXCSRL_DEV_b.send_stall = 1;
        }
    }
    return 0;
}

/**
 * @brief            Endpoint clear pause
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_clear_stall(const uint8_t ep) { return 0; }

/**
 * @brief            Check endpoint status
 * @pre              None
 * @param[in]        ep ： Endpoint address
 * @param[out]       stalled ： Outgoing endpoint status
 * @retval           >=0 success otherwise failure
 */
int usbd_ep_get_stall(const uint8_t ep, uint8_t *stalled) { return 0; }

/**
 * @brief            USB initialization
 * @pre              None
 * @param[in]        None
 * @retval           >=0 success otherwise failure
 */
int usb_dc_init(void) { return 0; }

void USBOTG_IRQ_Handler(void)
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
    }

    if (usb_it_state & USB_Resume_Mask)
    {
        /*!< Resume */

        USB_OTG->POWER_b.resume = 1;
    }

    if (usb_it_state & USB_Reset_Mask)
    {
        /*!< Reset */
        /*!< Process */
        USB_OTG->FADDR = 0;
        USB_OTG->INDEX = 0;

        /**
         * EP0 refresh flush doesn't matter direction
         * Fill in any out direction here
         */
        usb_flush_fifo(0x00, EP_DIR_OUT);
        for (i = 1; i < 8; i++)
        {
            usb_flush_fifo(i, EP_DIR_OUT);
            usb_flush_fifo(i, EP_DIR_IN);
        }
        // PM_SetDriverRunFlag(PM_DRV_USB, 1);
        air105_set_ep_ack(0x00);
        /*!<  */
        // usbd_event_notify_handler(USBD_EVENT_RESET, NULL);
    }

    if (usb_it_state & USB_SOF_Mask)
    {
        /*!< Sof */
    }

    if (usb_it_state & USB_Discon_Mask)
    {
        /*!< Diconnect */
    }

    if (ep_in_it_state & EP0_IT_Mask)
    {
        /*!< ep 0 interrupt */

        /*!< ep 0 发送 stall 产生的中断 */
        if (USB_OTG->CSR0L_DEV_b.sent_stall)
        {
            air105_set_ep_ack(0x00);
        }
        /*!< 主机发送 setup令牌产生的中断 */
        if (USB_OTG->CSR0L_DEV_b.setup_end)
        {
            /*!< 控制事务完成 */
            usb_dc_cfg.is_setup = 1;

            /*!< 清除 setup_end 标志位 */
            USB_OTG->CSR0L_DEV_b.serviced_setupend = 1;
        }

        /*!< 主机下发数据产生的中断 */
        if (USB_OTG->CSR0L_DEV_b.rx_pkt_rdy)
        {
            /*!< 获取接收字节的长度 */
            RxLen = USB_OTG->COUNT0_b.ep0_rx_count;
            if (RxLen)
            {
                Count32b = RxLen >> 2;
                for (i = 0; i < Count32b; i++)
                {
                    usb_dc_cfg.rx_data[i] = USB_OTG->FIFOX[0].dword;
                }
                // for (i = Count32b * 4; i < RxLen; i++)
                // {

                // }

                if (usb_dc_cfg.is_setup == 1)
                {
                    /*!< setup handler */
                    usbd_event_notify_handler(USBD_EVENT_SETUP, NULL);
                    usb_dc_cfg.is_setup = 0;
                }
                else
                {
                    /*!< ep 0 out handler */
                    // usbd_event_notify_handler(USBD_EVENT_EP0_HANDLER, NULL);
                }
                // USB_StackPutRxData(USB_ID, 0, prvUSB.pRxData.pu8, RxLen);
            }
            /*!< Write 1 to serviced_rxpktrdy to clear rx_pkt_rdy */
            USB_OTG->CSR0L_DEV_b.serviced_rxpktrdy = 1;
            // USB_StackAnalyzeDeviceEpRx(USB_ID, 0);
        }
        else if (!USB_OTG->CSR0L_DEV_b.tx_pkt_rdy)
        {
            /*!< 发送成功中断 */
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
                            usb_dc_cfg.rx_data[i] = USB_OTG->FIFOX[EpIndex].dword;
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