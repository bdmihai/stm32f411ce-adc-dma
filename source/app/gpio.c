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
#include "gpio.h"

void gpio_init()
{
    /* disable JTAG */
    MODIFY_REG(GPIOA->MODER,    GPIO_MODER_MODER15_Msk,     0);                         /* JTDI disabled */
    MODIFY_REG(GPIOB->MODER,    GPIO_MODER_MODER4_Msk,      0);                         /* NJTRST disabled */
    MODIFY_REG(GPIOB->MODER,    GPIO_MODER_MODER3_Msk,      0);                         /* JTDO disabled */
    
    /* configure LED pin */
    MODIFY_REG(GPIOC->MODER,    GPIO_MODER_MODER13_Msk,     GPIO_MODER_MODER13_0);      /* set the pin as output */
    MODIFY_REG(GPIOC->OTYPER,   GPIO_OTYPER_OT13_Msk,       0);                         /* push pull */
    MODIFY_REG(GPIOC->OSPEEDR,  GPIO_OSPEEDR_OSPEED13_Msk,  0);                         /* low speed */
    MODIFY_REG(GPIOC->PUPDR,    GPIO_PUPDR_PUPD13_Msk,      0);                         /* no pull up, no pull down */
}

void gpio_set_blue_led()
{
    GPIOC->BSRR = GPIO_BSRR_BR13;
}

void gpio_reset_blue_led()
{
    GPIOC->BSRR = GPIO_BSRR_BS13;
}

void gpio_e_high() 
{  
    GPIOB->BSRR = GPIO_BSRR_BS9;
}

void gpio_e_low() 
{  
    GPIOB->BSRR = GPIO_BSRR_BR9;
}

void gpio_rs_high() 
{  
    GPIOB->BSRR = GPIO_BSRR_BS8;
}

void gpio_rs_low() 
{  
    GPIOB->BSRR = GPIO_BSRR_BR8;
}

void gpio_data_wr(const uint8_t data)
{
    GPIOA->ODR = data;
}

uint8_t gpio_data_rd()
{
    return GPIOA->IDR;
}

void gpio_config_data_out()
{
    /* set the pin as output */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER0_Msk, GPIO_MODER_MODER0_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER1_Msk, GPIO_MODER_MODER1_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER2_Msk, GPIO_MODER_MODER2_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER3_Msk, GPIO_MODER_MODER3_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER4_Msk, GPIO_MODER_MODER4_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER5_Msk, GPIO_MODER_MODER5_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER6_Msk, GPIO_MODER_MODER6_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER7_Msk, GPIO_MODER_MODER7_0);

    /* push pull */
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT0_Msk, 0);
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT1_Msk, 0);
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT2_Msk, 0);
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT3_Msk, 0);
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT4_Msk, 0);
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT5_Msk, 0);
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT6_Msk, 0);
    MODIFY_REG(GPIOA->OTYPER, GPIO_OTYPER_OT7_Msk, 0);

    /* low speed */
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED0_Msk, 0);
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED1_Msk, 0);
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED2_Msk, 0);
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED3_Msk, 0);
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED4_Msk, 0);
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED5_Msk, 0);
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED6_Msk, 0);
    MODIFY_REG(GPIOA->OSPEEDR, GPIO_OSPEEDR_OSPEED7_Msk, 0);

    /* no pull up, no pull down */
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD0_Msk, 0);
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD1_Msk, 0);
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD2_Msk, 0);
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD3_Msk, 0);
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD4_Msk, 0);
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD5_Msk, 0);
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD6_Msk, 0);
    MODIFY_REG(GPIOA->PUPDR, GPIO_PUPDR_PUPD7_Msk, 0);
}

void gpio_config_data_in()
{
    /* set the pin as input */
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER0_Msk, 0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER1_Msk, 0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER2_Msk, 0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER3_Msk, 0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER4_Msk, 0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER5_Msk, 0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER6_Msk, 0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER7_Msk, 0);
}

void gpio_config_control_out()
{
    /* set the pin as output */
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER8_Msk, GPIO_MODER_MODER8_0);
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER9_Msk, GPIO_MODER_MODER9_0);

    /* push pull */
    MODIFY_REG(GPIOB->OTYPER, GPIO_OTYPER_OT8_Msk, 0);
    MODIFY_REG(GPIOB->OTYPER, GPIO_OTYPER_OT9_Msk, 0);

    /* low speed */
    MODIFY_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED8_Msk, 0);
    MODIFY_REG(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED9_Msk, 0);

    /* no pull up, no pull down */
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD8_Msk, 0);
    MODIFY_REG(GPIOB->PUPDR, GPIO_PUPDR_PUPD9_Msk, 0);
}
