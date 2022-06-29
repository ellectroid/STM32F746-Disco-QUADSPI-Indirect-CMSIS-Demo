#include "nvic.h"

void nvic_setup(void) {

	NVIC_SetPriority(DMA2_Stream2_IRQn, 16U);
	NVIC_ClearPendingIRQ(DMA2_Stream2_IRQn);
	NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	NVIC_SetPriority(DMA2_Stream7_IRQn, 16U);
	NVIC_ClearPendingIRQ(DMA2_Stream7_IRQn);
	NVIC_EnableIRQ(DMA2_Stream7_IRQn);

}

void DMA2_Stream2_IRQHandler(void) {
	if (DMA2->LISR & DMA_LISR_TCIF2) { //If stream 2 transfer complete (USART RX)
		DMA2->LIFCR |= DMA_LIFCR_CTCIF2; //clear flag
	}
}

void DMA2_Stream7_IRQHandler(void) {
	if (DMA2->HISR & DMA_HISR_TCIF7) { //If stream 7 transfer complete (USART TX)
		DMA2->HIFCR |= DMA_HIFCR_CTCIF7; //clear flag
	}
}
