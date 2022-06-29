#include "gpio.h"

/*
 * Private functions
 */

void gpio_configure_usart1(void);
void gpio_configure_stm32f746disco_led1(void);
void gpio_configure_quadspi(void);

void gpio_setup(void) {
	gpio_configure_usart1();
	gpio_configure_stm32f746disco_led1();
	gpio_configure_quadspi();
}

void gpio_configure_quadspi(void) {
	/*
	 QuadSPI:
	 NCS -  PB6
	 CLK -  PB2
	 D0  - PD11
	 D1  - PD12
	 D2  -  PE2
	 D3  - PD13
	 */

	/*
	 * PB2 QSPI CLK
	 */
	GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODER2_0)) | (GPIO_MODER_MODER2_1); //PB2 set to alternate function
	GPIOB->AFR[0] = (GPIOB->AFR[0] & ~(0xFU << GPIO_AFRL_AFRL2_Pos)) | (0x09 << GPIO_AFRL_AFRL2_Pos); //PB2 to AF9
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_2); //PB2 output push-pull
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR2); //PB2 output speed very high
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR2); //PB2 no pull-up no pull-down

	/*
	 * PB6 QSPI NCS
	 */
	GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODER6_0)) | (GPIO_MODER_MODER6_1); //PB6 set to alternate function
	GPIOB->AFR[0] = (GPIOB->AFR[0] & ~(0xFU << GPIO_AFRL_AFRL6_Pos)) | (0x0A << GPIO_AFRL_AFRL6_Pos); //PB6 to AF10
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_6); //PB6 output push-pull
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR6); //PB6 output speed very high
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6) | (0x00 << GPIO_PUPDR_PUPDR6_Pos); //PB6 no pull-up no pull-down (has external)

	/*
	 * PD11 QSPI D0
	 */
	GPIOD->MODER = (GPIOD->MODER & ~(GPIO_MODER_MODER11_0)) | (GPIO_MODER_MODER11_1); //PD11 set to alternate function
	GPIOD->AFR[1] = (GPIOD->AFR[1] & ~(0xFU << GPIO_AFRH_AFRH3_Pos)) | (0x09 << GPIO_AFRH_AFRH3_Pos); //PD11 to AF9
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT_11); //PD11 output push-pull
	GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR11); //PD11 output speed very high
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR11); //PD11 no pull-up no pull-down

	/*
	 * PD12 QSPI D1
	 */
	GPIOD->MODER = (GPIOD->MODER & ~(GPIO_MODER_MODER12_0)) | (GPIO_MODER_MODER12_1); //PD12 set to alternate function
	GPIOD->AFR[1] = (GPIOD->AFR[1] & ~(0xFU << GPIO_AFRH_AFRH4_Pos)) | (0x09 << GPIO_AFRH_AFRH4_Pos); //PD12 to AF9
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT_12); //PD12 output push-pull
	GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR12); //PD12 output speed very high
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR12); //PD12 no pull-up no pull-down

	/*
	 * PD13 QSPI D3
	 */
	GPIOD->MODER = (GPIOD->MODER & ~(GPIO_MODER_MODER13_0)) | (GPIO_MODER_MODER13_1); //PD13 set to alternate function
	GPIOD->AFR[1] = (GPIOD->AFR[1] & ~(0xFU << GPIO_AFRH_AFRH5_Pos)) | (0x09 << GPIO_AFRH_AFRH5_Pos); //PD13 to AF9
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT_13); //PD13 output push-pull
	GPIOD->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR13); //PD13 output speed very high
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPDR13); //PD13 no pull-up no pull-down

	/*
	 * PE2 QSPI D2
	 */
	GPIOE->MODER = (GPIOE->MODER & ~(GPIO_MODER_MODER2_0)) | (GPIO_MODER_MODER2_1); //PE2 set to alternate function
	GPIOE->AFR[0] = (GPIOE->AFR[0] & ~(0xFU << GPIO_AFRL_AFRL2_Pos)) | (0x09 << GPIO_AFRL_AFRL2_Pos); //PE2 to AF9
	GPIOE->OTYPER &= ~(GPIO_OTYPER_OT_2); //PE2 output push-pull
	GPIOE->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR2); //PE2 output speed very high
	GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPDR2); //PE2 no pull-up no pull-down

}

void gpio_configure_stm32f746disco_led1(void) {
	/*
	 *  PI1 GREEN LED LD1 ACTIVE HIGH
	 * */
	GPIOI->MODER = (GPIOI->MODER & ~(1U << 3U)) | (1U << 2U); //PI1 set to general purpose output mode
	GPIOI->OTYPER &= ~(GPIO_OTYPER_OT_1); //PI1 output push-pull
	GPIOI->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR1); //PI1 output speed very low
	GPIOI->PUPDR &= ~(GPIO_PUPDR_PUPDR1); //PI1 no pull-up no pull-down
}

void gpio_configure_usart1(void) {
	/*
	 USART1:
	 TX  -  PA9
	 RX  -  PB7
	 */

	/*
	 * PA9 USART1 TX
	 */
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODER9_0)) | GPIO_MODER_MODER9_1; //PA9 set to alternate function
	GPIOA->AFR[1] = (GPIOA->AFR[1] & ~(1U << 7)) | (0x07 << 4); //PA9 to AF7
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9; //PA9 output push-pull
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR9; //PA9 output speed very high
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR9; //PA9 no pull-up no pull-down

	/*
	 * PB7 USART1 RX
	 */
	GPIOB->MODER = (GPIOB->MODER & ~(GPIO_MODER_MODER7_0)) | (GPIO_MODER_MODER7_1); //PB7 set to alternate function
	GPIOB->AFR[0] = (GPIOB->AFR[0] & ~(1U << 31)) | (0x07 << 28); //PB7 to AF7
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_7); //PB7 output push-pull
	GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR7); //PB7 output speed very high
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR7); //PB7 no pull-up no pull-down
}


void toggle_stm32f746disco_ld1(void) {
	GPIOI->ODR ^= GPIO_ODR_OD1;
}
