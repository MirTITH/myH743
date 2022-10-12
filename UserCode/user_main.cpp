#include "user_main.hpp"
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include <iostream>

using namespace std;

void StartDefaultTask(void const *argument)
{
    (void)argument;
    
    MX_USB_DEVICE_Init();

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        cout << "Hello USB cout" << endl;
        osDelay(200);
    }
}
