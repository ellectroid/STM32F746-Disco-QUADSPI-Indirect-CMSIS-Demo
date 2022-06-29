#include "main.h"

void system_hw_setup(void);
void printQspiFlashConfigs(void);

int main(void) {

	/*
	 * This program is a QUADSPI Indirect Mode Demo, CMSIS register definitions only
	 * Hardware: STM32F746G-Discovery
	 * MCU: STM32F746
	 * QUADSPI NOR Flash IC: 128Mbit MT25QL128ABA1EW9
	 * Software extras: DMA UART to ST-Link
	 *
	 *
	 * Actions performed: MCU QUADSPI peripheral setup
	 * QUADSPI Flash setup
	 * Erase the first 4kb subsector
	 * Print the first 8 bytes via UART
	 * Write the first 8 bytes to Flash
	 * Print the first 8 bytes via UART
	 * Victoriously blink an LED
	 * */

	system_hw_setup(); //initialize hardware
	unsigned char programStart[] = "\r\nProgram start\r\n";
	usart_dma_sendArray(USART1, (uint8_t *) programStart, sizeof(programStart));
	system_msdelay(100);


	/* Print QSPI Flash configuration registers via UART */
	printQspiFlashConfigs();

	/* ---------- QUADSPI ERASE TEST ---------- */

	/* Erase the first 4kb subsector */
	QSPI_eraseQuad(MT25QL128ABA1EW9_COMMAND_4KB_SUBSECTOR_ERASE, 0x000000);

	/* Read first 8 bytes of memory */
	uint8_t memoryContent[8];
	QSPI_readMemoryBytesQuad(0x000000, 8U, memoryContent);

	/* Print these 8 bytes over UART */
	uint8_t memoryContentChars[sizeof(memoryContent) * 2U]; //every byte is two hex chars
	for (uint8_t i = 0; i < sizeof(memoryContent); i++) {
		memoryContentChars[2 * i] = upperNibbleToAscii(memoryContent[i]);
		memoryContentChars[2 * i + 1] = lowerNibbleToAscii(memoryContent[i]);
	}
	unsigned char memoryAddressString[] = "Memory at 0x000000: 0x";
	usart_dma_sendArray(USART1, (uint8_t *)memoryAddressString, sizeof(memoryAddressString) - 1U);
	system_msdelay(40U);

	usart_dma_sendArray(USART1, memoryContentChars, sizeof(memoryContentChars));
	system_msdelay(100U);

	/* ---------- QUADSPI PROGRAM TEST ---------- */

	unsigned char newLine[] = "\r\nWriting to Flash via QUADSPI\r\n";
	usart_dma_sendArray(USART1, (uint8_t *) newLine, sizeof(newLine) - 1U);
	system_msdelay(100U);

	/* Write to the first 8 bytes*/
	uint8_t dataToWrite[] = { 0xFA, 0xCE, 0x8D, 0x00, 0xFA, 0xCA, 0xDE, 0x07 };
	QSPI_programMemoryBytesQuad(0x000000, sizeof(dataToWrite), dataToWrite);

	QSPI_readMemoryBytesQuad(0x000000, 8U, memoryContent);

	for (uint8_t i = 0; i < sizeof(memoryContent); i++) {
		memoryContentChars[2 * i] = upperNibbleToAscii(memoryContent[i]);
		memoryContentChars[2 * i + 1] = lowerNibbleToAscii(memoryContent[i]);
	}
	usart_dma_sendArray(USART1, (uint8_t *)memoryAddressString, sizeof(memoryAddressString) - 1U);
	system_msdelay(40U);

	usart_dma_sendArray(USART1, memoryContentChars, sizeof(memoryContentChars));
	system_msdelay(100U);

	/* ---------- END OF QUADSPI TEST ---------- */

	while (1) {
		toggle_stm32f746disco_ld1();
		system_msdelay(200U);
	}
}

void system_hw_setup(void) {
	rcc_setup(); //clock for peripheral, clock will not be altered; therefore default HSI 16MHz
	systick_setup(SYSTEM_FREQUENCY); //activate systick
	gpio_setup(); //set pin modes and functions
	dma_reset_flags(DMA2); //clear DMA2 flags for USART1
	dma_reset_flags(DMA1); //clear DMA1 flags for I2C3
	usart_dma_setup(USART1); //set control registers and settings for USART1 and its DMA connected to st-link
	usart_enable(USART1); //enable uart1
	usart_enable_tx(USART1); //enable tx line (wrapper)
	usart_enable_rx(USART1); //enable rx line (wrapper)
	QSPI_setupIndirect(); //setup qspi in indirect mode
	nvic_setup(); //set interrupts and their priorities

}

void printQspiFlashConfigs(void) {
	/*
	 * Read volatile configuration register
	 * Send it over UART
	 *
	 * */
	uint8_t registerContent;
	QSPI_readRegister(MT25QL128ABA1EW9_COMMAND_READ_VOLATILE_CONFIGURATION_REGISTER, QIO_QUAD, &registerContent);
	char configurationRegisterString[] = "Configuration register: ";
	uint8_t registerValue[6];
	registerValue[0] = (uint8_t) '0';
	registerValue[1] = (uint8_t) 'x';
	registerValue[2] = (uint8_t) upperNibbleToAscii(registerContent);
	registerValue[3] = (uint8_t) lowerNibbleToAscii(registerContent);
	registerValue[4] = '\r';
	registerValue[5] = '\n';
	usart_dma_sendArray(USART1, (uint8_t*) configurationRegisterString, sizeof(configurationRegisterString));
	system_msdelay(100);
	usart_dma_sendArray(USART1, (uint8_t*) registerValue, sizeof(registerValue));
	system_msdelay(100);

	/*
	 * Read enhanced volatile configuration register
	 * Send it over UART
	 *
	 * */

	QSPI_readRegister(MT25QL128ABA1EW9_COMMAND_READ_ENHANCED_VOLATILE_CONFIGURATION_REGISTER, QIO_QUAD, &registerContent);
	char enhancedConfigurationRegisterString[] = "Enhanced configuration register: ";
	registerValue[0] = (uint8_t) '0';
	registerValue[1] = (uint8_t) 'x';
	registerValue[2] = (uint8_t) upperNibbleToAscii(registerContent);
	registerValue[3] = (uint8_t) lowerNibbleToAscii(registerContent);
	registerValue[4] = '\r';
	registerValue[5] = '\n';
	usart_dma_sendArray(USART1, (uint8_t*) enhancedConfigurationRegisterString, sizeof(enhancedConfigurationRegisterString));
	system_msdelay(100);
	usart_dma_sendArray(USART1, (uint8_t*) registerValue, sizeof(registerValue));
	system_msdelay(100);

	/*
	 * Read status register
	 * Send it over UART
	 *
	 * */

	QSPI_readRegister(MT25QL128ABA1EW9_COMMAND_READ_STATUS_REGISTER, QIO_QUAD, &registerContent);
	char statusRegisterString[] = "Status register: ";
	registerValue[0] = (uint8_t) '0';
	registerValue[1] = (uint8_t) 'x';
	registerValue[2] = (uint8_t) upperNibbleToAscii(registerContent);
	registerValue[3] = (uint8_t) lowerNibbleToAscii(registerContent);
	registerValue[4] = '\r';
	registerValue[5] = '\n';
	usart_dma_sendArray(USART1, (uint8_t*) statusRegisterString, sizeof(statusRegisterString));
	system_msdelay(100);
	usart_dma_sendArray(USART1, (uint8_t*) registerValue, sizeof(registerValue));
	system_msdelay(100);

	/*
	 * Read flag status register
	 * Send it over UART
	 *
	 * */

	QSPI_readRegister(MT25QL128ABA1EW9_COMMAND_READ_FLAG_STATUS_REGISTER, QIO_QUAD, &registerContent);
	char flagStatusRegisterString[] = "Flag status register: ";
	registerValue[0] = (uint8_t) '0';
	registerValue[1] = (uint8_t) 'x';
	registerValue[2] = (uint8_t) upperNibbleToAscii(registerContent);
	registerValue[3] = (uint8_t) lowerNibbleToAscii(registerContent);
	registerValue[4] = '\r';
	registerValue[5] = '\n';
	usart_dma_sendArray(USART1, (uint8_t*) flagStatusRegisterString, sizeof(flagStatusRegisterString));
	system_msdelay(100);
	usart_dma_sendArray(USART1, (uint8_t*) registerValue, sizeof(registerValue));
	system_msdelay(100);
}
