#include "user_main.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "string.h"
#include <iostream>
#include "adc.h"
#include "sys_monitor.hpp"
#include "bsp_driver_sd.h"

using namespace std;

uint8_t BSP_SD_IsDetected(void)
{
    return SD_NOT_PRESENT;
}

void StartDefaultTask(void const *argument)
{
    (void)argument;

    MX_USB_DEVICE_Init();

    HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

    SysMonitor sys_monitor;

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        cout << sys_monitor.temperature << endl;
        osDelay(200);
    }
}
