/*_____________________________________________________________________________
 │                                                                            |
 │ COPYRIGHT (C) 2021 Mihai Baneu                                             |
 │                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 21.Jan.2021  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "stm32rtos.h"
#include "task.h"
#include "queue.h"
#include "system.h"
#include "gpio.h"
#include "adc.h"
#include "isr.h"
#include "st7066u.h"
#include "printf.h"

/* Queue used to send and receive complete message structures. */
QueueHandle_t adc_queue = NULL;

static void vTaskLED(void *pvParameters)
{
    (void)pvParameters;

    /* led OFF */
    gpio_set_blue_led();

    for (;;) {
        gpio_reset_blue_led();
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        gpio_set_blue_led();
        vTaskDelay(100 / portTICK_PERIOD_MS);

        gpio_reset_blue_led();
        vTaskDelay(100 / portTICK_PERIOD_MS);

        gpio_set_blue_led();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

static void vTaskDisplay(void *pvParameters)
{
    adc_event_t event;
    (void)pvParameters;

    st7066u_hw_control_t hw = {
        .config_control_out  = gpio_config_control_out,
        .config_data_out     = gpio_config_data_out,
        .config_data_in      = gpio_config_data_in,
        .e_high              = gpio_e_high,
        .e_low               = gpio_e_low,
        .rs_high             = gpio_rs_high,
        .rs_low              = gpio_rs_low,
        .data_wr             = gpio_data_wr,
        .data_rd             = gpio_data_rd,
        .delay_us            = delay_us
    };

    st7066u_init(hw);

    st7066u_cmd_function_set(ST7066U_8_BITS_DATA, ST7066U_2_LINE_DISPLAY, ST7066U_5x8_SIZE);
    st7066u_cmd_function_set(ST7066U_8_BITS_DATA, ST7066U_2_LINE_DISPLAY, ST7066U_5x8_SIZE);

    st7066u_cmd_on_off(ST7066U_DISPLAY_ON, ST7066U_CURSOR_OFF, ST7066U_CURSOR_POSITION_OFF);
    st7066u_cmd_clear_display();
    st7066u_cmd_entry_mode(ST7066U_INCREMENT_ADDRESS, ST7066U_SHIFT_DISABLED);

    st7066u_write_str("    Welcome!    ");
    st7066u_cmd_set_ddram(0x40);
    st7066u_write_str("ADC meas on PB0");
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    for (;;) {
        if (xQueueReceive(adc_queue, &event, portMAX_DELAY) == pdPASS) {
            char txt[17] = {0};
            float voltage = event.digital_value * 3.312f / (4096.0f * 250);
            
            st7066u_cmd_clear_display();
            sprintf(txt, "    %4.4f V    ", voltage);
            st7066u_write_str(txt);
            st7066u_cmd_set_ddram(0x40);
            sprintf(txt, "   %5d dig   ", event.digital_value);
            st7066u_write_str(txt);
        }
    }
}

static void vTaskAdc(void *pvParameters)
{
    (void)pvParameters;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, 250 / portTICK_PERIOD_MS);
        adc_start_conversion();
    }
}

int main(void)
{
    /* initialize the system */
    system_init();

    /* initialize the gpio */
    gpio_init();

    /* initialize the adc */
    adc_init();

    /* initialize the interupt service routines */
    isr_init();

    /* create the queues */
    adc_queue = xQueueCreate(1, sizeof(adc_event_t));

    /* create the tasks specific to this application. */
    xTaskCreate(vTaskLED, "vTaskLED", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(vTaskDisplay, "vTaskDisplay", configMINIMAL_STACK_SIZE*2, NULL, 2, NULL);
    xTaskCreate(vTaskAdc, "vTaskAdc", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    /* start the scheduler. */
    vTaskStartScheduler();

    /* should never get here ... */
    blink(10);
    return 0;
}
