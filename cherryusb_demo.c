#include "cherryusb_demo.h"

#if (DEMO_TARGET == CDC_DEMO)
#include "cdc_acm_template.c"
#elif (DEMO_TARGET == HIDKBD_DEMO)
#include "hid_keyboard_template.c"
#elif (DEMO_TARGET == HIDMOUSE_DEMO)
#include "hid_mouse_template.c"
#elif (DEMO_TARGET == HIDCUSTOM_DEMO)
#include "hid_custom_inout_template.c"
#elif (DEMO_TARGET == MSC_DEMO)
#include "usbd_msc.c"
#include "msc_ram_template.c"
#elif (DEMO_TARGET == HID_MSC_CDC_DEMO)
#include "usbd_msc.c"
#include "cdc_acm_hid_msc_template.c"
#elif (DEMO_TARGET == CDC_MULTITUDE_DEMO)
#include "cdc_acm_multi_template.c"
#endif