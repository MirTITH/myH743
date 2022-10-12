#include "user_main.hpp"
#include "main.h"
#include "cmsis_os.h"

void StartDefaultTask(void const *argument)
{
    (void)argument;

    for (;;) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelay(200);
    }
}
