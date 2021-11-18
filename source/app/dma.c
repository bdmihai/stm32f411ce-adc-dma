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
#include "stm32rtos.h"
#include "queue.h"
#include "lcd.h"
#include "dma.h"


/* adc samples count to take for averaging */
#define ADC_SAMPLES_COUNT  1000

/* Queue used to communicate dma messages. */
QueueHandle_t dma_queue = NULL;

/* message counter */
static volatile uint32_t mss_counter = 0;

/* the memmory buffers for the DMA */
static uint16_t dma_buffer0[ADC_SAMPLES_COUNT];
static uint16_t dma_buffer1[ADC_SAMPLES_COUNT];

void dma_init()
{
    /* make sure the DMA stream is disabled */
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_EN_Msk, 0);
    do {
    } while ((DMA2_Stream0->CR & DMA_SxCR_EN_Msk) != 0);

    /* clear the interupt register */
    SET_BIT(DMA2->LIFCR, DMA_LIFCR_CFEIF0_Msk | DMA_LIFCR_CDMEIF0_Msk | DMA_LIFCR_CTEIF0_Msk | DMA_LIFCR_CHTIF0_Msk | DMA_LIFCR_CTCIF0_Msk);

    /* configure the pointers/data amount for DMA */
    DMA2_Stream0->PAR  = (uint32_t)&(ADC1->DR);
    DMA2_Stream0->M0AR = (uint32_t)dma_buffer0;
    DMA2_Stream0->M1AR = (uint32_t)dma_buffer1;
    DMA2_Stream0->NDTR = ADC_SAMPLES_COUNT;

    /* select the channel 0 for the stram 0 - ADC1*/
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_CHSEL_Msk, 0);

    /* double buffer mode: start with memmory pointer 0 (periferal to memory)*/
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_DBM_Msk, DMA_SxCR_DBM);
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_CT_Msk,  0);
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_DIR,     0);

    /* configure periferal */
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_PSIZE_Msk, DMA_SxCR_PSIZE_0);   // 16 bit
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_PINC_Msk,  0);                  // no increment
    
    /* configure memory */
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_MSIZE_Msk, DMA_SxCR_MSIZE_0);   // 16 bit
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_MINC_Msk,  DMA_SxCR_MINC);      // increment

    /* enable interupt */
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_TCIE_Msk, DMA_SxCR_TCIE);
}

void dma_enable()
{
    memset(dma_buffer0, 0, ADC_SAMPLES_COUNT);
    memset(dma_buffer1, 0, ADC_SAMPLES_COUNT);
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_EN_Msk, DMA_SxCR_EN);
}

void dma_disable()
{
    MODIFY_REG(DMA2_Stream0->CR, DMA_SxCR_EN_Msk, 0);
}

void dma_isr_handler()
{
    if (DMA2->LISR & DMA_LISR_TCIF0_Msk) {
        dma_event_t dma_event;

        dma_event.length = ADC_SAMPLES_COUNT;
        if (DMA2_Stream0->CR & DMA_SxCR_CT_Msk) {
            dma_event.buffer = dma_buffer0;
        } else {
            dma_event.buffer = dma_buffer1;
        }

        /* clear the interupt register */
        SET_BIT(DMA2->LIFCR, DMA_LIFCR_CFEIF0_Msk | DMA_LIFCR_CDMEIF0_Msk | DMA_LIFCR_CTEIF0_Msk | DMA_LIFCR_CHTIF0_Msk | DMA_LIFCR_CTCIF0_Msk);
        xQueueSendFromISR(dma_queue, &dma_event, (TickType_t) 0);
    }
}

void vTaskDma(void *pvParameters)
{
    (void)pvParameters;

    dma_enable();
    adc_enable();

    for (;;) {
        dma_event_t dma_event;
        if (xQueueReceive(dma_queue, &dma_event, portMAX_DELAY) == pdPASS) {
            lcd_event_t lcd_event;

            // cumulate all values measured by the ADC in order to get the average
            lcd_event.digital_value = 0;
            for (uint16_t i = 0; i < dma_event.length; i++) {
                lcd_event.digital_value += dma_event.buffer[i];
            }

            // calculate the voltage
            lcd_event.voltage = (lcd_event.digital_value * 3.312f) / (4096.0f * dma_event.length);
            lcd_event.mss_counter = mss_counter;
            mss_counter++;

            // send the measurement to the display task
            xQueueSendToBack(lcd_queue, &lcd_event, (TickType_t) 0);
        }
    }
}
