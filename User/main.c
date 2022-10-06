#include <stdio.h>
#include <stdint.h>
#include "air105.h"
#include "sysc.h"
#include "delay.h"
#include "usart.h"
#include "SEGGER_RTT.h"
#include "cherryusb_demo.h"

int fputc(int ch, FILE *f) 
{
  SEGGER_RTT_PutChar(0, ch);
  return ch;
}

void usb_dc_low_level_init(void) 
{
  SYSCTRL->CG_CTRL2 &= ~SYSCTRL_AHBPeriph_USB;
  NVIC_SetPriority(USB_IRQn, 7);
  SYSCTRL->CG_CTRL2 |= SYSCTRL_AHBPeriph_USB;
  NVIC_EnableIRQ(USB_IRQn);
  SYSCTRL->LOCK_R &= ~SYSCTRL_USB_RESET;
  SYSCTRL->SOFT_RST2 &= ~SYSCTRL_USB_RESET;
  SYSCTRL->LOCK_R |= SYSCTRL_USB_RESET;

  volatile uint32_t temp = SYSCTRL->USBPHY_CR3;
  temp &= 0xfffffffe;
  SYSCTRL->USBPHY_CR3 = temp;
}

int main(void)
{
	SystemClock_Config_HSE();
    Delay_Init();
	printf("Air105 RTT Viewer MDK CherryUSB Demo \r\n");
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = GPIO_Pin_14;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Remap = GPIO_Remap_1;
	GPIO_Init(GPIOD, &gpio);
	gpio.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOD, &gpio);

#if (DEMO_TARGET == CDC_DEMO)
	extern void cdc_acm_init(void);
	cdc_acm_init();
#elif (DEMO_TARGET == HIDKBD_DEMO)
	extern void hid_keyboard_init(void);
	hid_keyboard_init();
#elif (DEMO_TARGET == HIDMOUSE_DEMO)
	extern void hid_mouse_init(void);
	hid_mouse_init();
#elif (DEMO_TARGET == HIDCUSTOM_DEMO)
  extern void hid_custom_keyboard_init(void);
	hid_custom_keyboard_init();
#elif (DEMO_TARGET == MSC_DEMO)
	extern void msc_ram_init(void);
	msc_ram_init();
#elif (DEMO_TARGET == HID_MSC_CDC_DEMO)
	extern void cdc_acm_hid_msc_descriptor_init(void);
	cdc_acm_hid_msc_descriptor_init();
#elif (DEMO_TARGET == CDC_MULTITUDE_DEMO)

#endif

	while (1)
	{
		GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);
		printf("ON\r\n");
		Delay_ms(1000);
		GPIO_SetBits(GPIOD, GPIO_Pin_14);
		GPIO_SetBits(GPIOD, GPIO_Pin_15);
		printf("OFF\r\n");
		Delay_ms(1000);
#if (DEMO_TARGET == CDC_DEMO)
extern void cdc_acm_data_send_with_dtr_test(void);
cdc_acm_data_send_with_dtr_test();
#elif (DEMO_TARGET == HIDKBD_DEMO)
extern void hid_keyboard_test(void);
hid_keyboard_test();
#elif (DEMO_TARGET == HIDMOUSE_DEMO)
extern void hid_mouse_test(void);
hid_mouse_test();
#elif (DEMO_TARGET == HIDCUSTOM_DEMO)
extern void hid_custom_test(void);
hid_custom_test();
#elif (DEMO_TARGET == MSC_DEMO)
#elif (DEMO_TARGET == HID_MSC_CDC_DEMO)
extern void cdc_acm_data_send_with_dtr_test(void);
cdc_acm_data_send_with_dtr_test();
#elif (DEMO_TARGET == CDC_MULTITUDE_DEMO)
// extern void cdc_acm_data_send_with_dtr_test(void);
// cdc_acm_data_send_with_dtr_test();
#endif
	}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	 /* Infinite loop */
	while (1)
	{
	}
}
#endif
