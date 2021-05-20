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
 |  Author: Mihai Baneu                           Last modified: 20.Mai.2021  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stm32f4xx.h"
#include "adc.h"

void adc_init()
{
    /* set the pis B0 as input ADC */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER0_Msk, 11);      /* set the pin as analog */
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD0_Msk,  0);       /* no pull up, no pull down */

    /* ADC prescalar 8 */
    MODIFY_REG(ADC1_COMMON->CCR, ADC_CCR_ADCPRE_Msk, ADC_CCR_ADCPRE_0 | ADC_CCR_ADCPRE_1);

    /* 12 bit ADC with dma enable */
    MODIFY_REG(ADC1->CR1, ADC_CR1_RES_Msk, 0);
    MODIFY_REG(ADC1->CR2, ADC_CR2_CONT_Msk, ADC_CR2_CONT);
    MODIFY_REG(ADC1->CR2, ADC_CR2_DMA_Msk, ADC_CR2_DMA);
    MODIFY_REG(ADC1->CR2, ADC_CR2_DDS_Msk, ADC_CR2_DDS);

    /* select B0 channel for conversion */
    MODIFY_REG(ADC1->SQR1, ADC_SQR1_L_Msk, 0);
    MODIFY_REG(ADC1->SQR3, ADC_SQR3_SQ1_Msk, (8 << ADC_SQR3_SQ1_Pos));

    /* select 480 ADC cycles */
    MODIFY_REG(ADC1->SMPR2, ADC_SMPR2_SMP8_Msk, ADC_SMPR2_SMP8_0 | ADC_SMPR2_SMP8_1 | ADC_SMPR2_SMP8_2);


    /*
        ADCCLK = 48MHz/8 = 6MHZ -> 0.0000001666...
        Conversion time = 480+15 cycles * 1000 = 0.0000825 s * 1000 = 0.0825
    */
}

void adc_enable()
{  
     /* ADC ON */
    MODIFY_REG(ADC1->CR2, ADC_CR2_ADON_Msk, ADC_CR2_ADON);
    MODIFY_REG(ADC1->CR2, ADC_CR2_SWSTART_Msk, ADC_CR2_SWSTART);
}

void adc_disable()
{
    /* ADC 1 OFF */
    MODIFY_REG(ADC1->CR2, ADC_CR2_ADON_Msk, 0);
}
