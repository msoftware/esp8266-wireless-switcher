/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * 
 * Razvan Dubau <dubau_razvan@yahoo.com> modified some of the files. So you can give 
 * me a beer and 2 beers to Jeroen.
 * ----------------------------------------------------------------------------
 */

#include "ets_sys.h"
#include "osapi.h"
#include "espmissingincludes.h"
#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "gpio.h"


#define LEDGPIO 2
#define BTNGPIO 0

static ETSTimer resetBtntimer;
static char GpioState = 0;

void ioLed(int state) {
    if (state == 0) {
        gpio_output_set(0, (1<<LEDGPIO), (1<<LEDGPIO), (1<<BTNGPIO));
        GpioState = 0;
    } else {
        gpio_output_set((1<<LEDGPIO), 0, (1<<LEDGPIO), (1<<BTNGPIO));
        GpioState = 1;
    }
}

static void ICACHE_FLASH_ATTR resetBtnTimerCb(void *arg) {
    static int resetCnt=0;
    if (!GPIO_INPUT_GET(BTNGPIO)) {
            resetCnt++;
    } else {
        
        if (resetCnt>=1) {
            if (GpioState == 0) {
                gpio_output_set((1<<LEDGPIO), 0, (1<<LEDGPIO), (1<<BTNGPIO));
                GpioState = 1;
            } else {
                gpio_output_set(0, (1<<LEDGPIO), (1<<LEDGPIO), (1<<BTNGPIO));
                GpioState = 0;
            }
        }
        
        if (resetCnt>=6) { //3 sec pressed
            wifi_station_disconnect();
            wifi_set_opmode(0x3); //reset to AP+STA mode
            os_printf("Reset to AP mode. Restarting system...\n");
            system_restart();
        }
        resetCnt=0;
    }
}

void ioInit() {
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
	gpio_output_set(0, (1<<LEDGPIO), (1<<LEDGPIO), (1<<BTNGPIO));
        os_timer_disarm(&resetBtntimer);
	os_timer_setfn(&resetBtntimer, resetBtnTimerCb, NULL);
	os_timer_arm(&resetBtntimer, 500, 1);
}

char ioGetGpio02() {
    return GpioState;
}
