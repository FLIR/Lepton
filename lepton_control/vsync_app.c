#include <stdio.h>
#include <stdlib.h>

#include "FLIR_I2C.h"

#include "LEPTON_OEM.h"
#include "LEPTON_SDK.h"
#include "LEPTON_SYS.h"
#include "LEPTON_Types.h"

LEP_CAMERA_PORT_DESC_T lepton_port;

void init_vsync()
{
    LEP_RESULT result;
    LEP_OEM_GPIO_MODE_E gpio_mode;

    gpio_mode = LEP_OEM_END_GPIO_MODE;
    result = LEP_GetOemGpioMode(&lepton_port, &gpio_mode);
    printf("LEP_GetOemGpioMode gpio_mode = %d result = %d.\n", gpio_mode, result);

    result = LEP_SetOemGpioMode(&lepton_port, LEP_OEM_GPIO_MODE_VSYNC);
    printf("LEP_SetOemGpioMode result = %d.\n", result);

    gpio_mode = LEP_OEM_END_GPIO_MODE;
    result = LEP_GetOemGpioMode(&lepton_port, &gpio_mode);
    printf("LEP_GetOemGpioMode gpio_mode = %d result = %d.\n", gpio_mode, result);
}

int main(int argc, char **argv)
{
    LEP_OpenPort(1, LEP_CCI_TWI, 400, &lepton_port);
    init_vsync();
    return 0;
}

