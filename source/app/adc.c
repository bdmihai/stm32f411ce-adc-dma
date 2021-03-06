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
#include "queue.h"
#include "adc.h"

extern QueueHandle_t adc_queue;
uint32_t adc_buffer = 0;
uint16_t adc_buffer_index = 0;

void adc_init()
{
    /* set the pis B0 as input ADC */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER0_Msk, 11);      /* set the pin as analog */
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD0_Msk,  0);       /* no pull up, no pull down */

    /* ADC prescalar 8 */
    MODIFY_REG(ADC1_COMMON->CCR, ADC_CCR_ADCPRE_Msk, ADC_CCR_ADCPRE_0 | ADC_CCR_ADCPRE_1);

    /* 12 bit ADC with interupt enable */
    MODIFY_REG(ADC1->CR1, ADC_CR1_RES_Msk, 0);
    MODIFY_REG(ADC1->CR1, ADC_CR1_EOCIE_Msk, ADC_CR1_EOCIE);
    MODIFY_REG(ADC1->CR2, ADC_CR2_CONT_Msk, ADC_CR2_CONT);

    /* select B0 channel for conversion */
    MODIFY_REG(ADC1->SQR1, ADC_SQR1_L_Msk, 0);
    MODIFY_REG(ADC1->SQR3, ADC_SQR3_SQ1_Msk, 8);

    /* select 480 ADC cycles */
    MODIFY_REG(ADC1->SMPR2, ADC_SMPR2_SMP8_Msk, ADC_SMPR2_SMP8_0 | ADC_SMPR2_SMP8_1 | ADC_SMPR2_SMP8_2);
}

void adc_start_conversion()
{
    adc_buffer = 0;
    adc_buffer_index = 0;

    CLEAR_BIT(ADC1->SR, ADC_SR_EOC | ADC_SR_STRT);

    /* ADC 1 ON */
    MODIFY_REG(ADC1->CR2, ADC_CR2_ADON_Msk, ADC_CR2_ADON);
    MODIFY_REG(ADC1->CR2, ADC_CR2_SWSTART_Msk, ADC_CR2_SWSTART);
}

void adc_stop_conversion()
{
    /* ADC 1 OFF */
    MODIFY_REG(ADC1->CR2, ADC_CR2_ADON_Msk, 0);
}
 
void adc_handle_conversion(void)
{
    adc_buffer += ADC1->DR;
    adc_buffer_index++;

    /*
    ADC_CLOCK = ((96000000/8)/480)/250 = 100Hz
    Duration calculated = 10ms
    Duration measured = 10.25ms
    */

    if (adc_buffer_index >= 250) {
        adc_event_t event;

        // stop the conversion
        adc_stop_conversion();

        // produce one adc event
        event.digital_value = adc_buffer;
        xQueueSendFromISR(adc_queue, &event, (TickType_t) 0);
    }

}
