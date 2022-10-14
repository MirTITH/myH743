#include "user_main.hpp"
#include "main.h"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include <iostream>
#include "adc.h"
#include "sys_monitor.hpp"

using namespace std;

void StartDefaultTask(void const *argument)
{
    (void)argument;

    MX_USB_DEVICE_Init();

    HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

    SysMonitor_Start();

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        cout << SysMonitorInfo.core_temp << endl;
        osDelay(200);
    }
}
