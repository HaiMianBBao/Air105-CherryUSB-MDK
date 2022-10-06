#pragma once

#define CDC_DEMO 0
#define HIDKBD_DEMO 1
#define HIDMOUSE_DEMO 2
#define HIDCUSTOM_DEMO 3
#define MSC_DEMO 4
#define HID_MSC_CDC_DEMO 5
#define CDC_MULTITUDE_DEMO 6



#ifndef DEMO_TARGET
#define DEMO_TARGET CDC_DEMO
#endif