/*
 * qspi.c
 *
 *  Created on: Apr 13, 2022
 *      Author: ellectroid
 */
#include "qspi.h"

/*
 * This function sets up QUADSPI in SDR mode with prescaler of 2
 */
void QSPI_setupIndirect(void) {

	/* ------------ QSPI Initialization --------------- */

	QUADSPI->CR &= ~(QUADSPI_CR_EN); //Make sure QUADSPI is disabled
	QUADSPI->DCR = 0x00; //reset
	QUADSPI->CCR = 0x00; //reset
	QUADSPI->DCR |= ((24U - 1U) << QUADSPI_DCR_FSIZE_Pos) | ((6U - 1U) << QUADSPI_DCR_CSHT_Pos); //Flash size 128Mb=16MB=2^24 bytes; Chip Select High Time 6 cycles
	QUADSPI->CR = 0x00; //reset
	QUADSPI->CR |= (QUADSPI_CR_SSHIFT | ((2U - 1U) << QUADSPI_CR_PRESCALER_Pos)); //Sample shift 1/2 clock cycle; Prescaler = 2 (216MHz/2 = 108MHz)
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Clear all flags
	QUADSPI->CR |= (QUADSPI_CR_EN); //Enable peripheral

	/* ---------- QSPI Flash Setup ---------- */

	QSPI_indirectWriteInstruction(MT25QL128ABA1EW9_COMMAND_RESET_ENABLE, QIO_SINGLE); //Enable reset
	QSPI_indirectWriteInstruction(MT25QL128ABA1EW9_COMMAND_RESET_MEMORY, QIO_SINGLE); //Reset QSPI Flash. If MCU was reset, but the QSPI Flash wasn't power cycled, it will retain old config

	QSPI_indirectWriteInstruction(MT25QL128ABA1EW9_COMMAND_WRITE_ENABLE, QIO_SINGLE); //Enable write to register
	QSPI_indirectWriteRegister(MT25QL128ABA1EW9_COMMAND_WRITE_VOLATILE_CONFIGURATION_REGISTER, QIO_SINGLE, ((0x06 << 4U) | 0xB)); //Set 6 dummy cycles. Quad output fast read only! Quad io fast read needs 9 dummies

	QSPI_indirectWriteInstruction(MT25QL128ABA1EW9_COMMAND_WRITE_ENABLE, QIO_SINGLE); //Enable write to register
	QSPI_indirectWriteRegister(MT25QL128ABA1EW9_COMMAND_WRITE_ENHANCED_VOLATILE_CONFIGURATION_REGISTER, QIO_SINGLE, 0x6F); //Enable QUAD IO, disable reset/hold

	/* ---------- End of QSPI Flash Setup ---------- */
}

/*
 * This function read a single register of QSPI Flash via extended SPI, Dual SPI or Quad SPI
 *
 * @param		instruction		8-bit instruction
 * @param		dataLinesMode	0x01 = 1 data line
 * 								0x02 = 2 data lines
 * 								0x03 = 4 data lines
 * @param		destination		address of byte to store register content
 * */
void QSPI_readRegister(uint8_t instruction, uint8_t dataLinesMode, uint8_t *destination) {

	while (QUADSPI->SR & QUADSPI_SR_BUSY); //Make sure no operation is going on
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Clear all flags
	QUADSPI->DLR = 1U - 1U; //Set data length to 1
	QUADSPI->CR = (QUADSPI->CR & ~(QUADSPI_CR_FTHRES)) | (0x00 << QUADSPI_CR_FTHRES_Pos); //Set FIFO threshold to 1

	/*
	 * Set communication configuration register
	 * Functional mode: 			indirect read
	 * Data mode: 					as per param dataLinesMode
	 * Instruction mode: 			as per param dataLinesMode
	 * Instruction: 				as per param instruction
	 *
	 */
	QUADSPI->CCR =
			(QSPI_FMODE_INDIRECT_READ << QUADSPI_CCR_FMODE_Pos) 	|
			(dataLinesMode << QUADSPI_CCR_DMODE_Pos) 				|
			(dataLinesMode << QUADSPI_CCR_IMODE_Pos) 				|
			(instruction << QUADSPI_CCR_INSTRUCTION_Pos);

	/* ---------- Communication Starts Automatically ----------*/

	while (!(QUADSPI->SR & QUADSPI_SR_FLEVEL)); //Wait for the byte of data to arrive
	*destination = *((uint8_t*) (&(QUADSPI->DR))); //Read byte from data register and place it into provided memory; Byte access
	while (QUADSPI->SR & QUADSPI_SR_BUSY); //Make sure no operation is going on
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Clear all flags

}


/*
 * This function writes a single instruction via extended SPI, Dual SPI or Quad SPI
 *
 * @param		instruction		8-bit instruction
 * @param		dataLinesMode	1 = 1 data line
 * 								2 = 2 data lines
 * 								3 = 4 data lines
 * */
void QSPI_indirectWriteInstruction(uint8_t instruction, uint8_t dataLinesMode) {

	while (QUADSPI->SR & QUADSPI_SR_BUSY); //Make sure no operation is going on
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; // clear all flags

	/*
	 * Set communication configuration register
	 * Functional mode: 			indirect write
	 * Instruction mode: 			as per param dataLines
	 * Instruction: 				as per param instruction
	 *
	 */
	QUADSPI->CCR =
			(QSPI_FMODE_INDIRECT_WRITE << QUADSPI_CCR_FMODE_Pos) 	|
			(dataLinesMode << QUADSPI_CCR_IMODE_Pos)				|
			(instruction << QUADSPI_CCR_INSTRUCTION_Pos);

	/* ---------- Communication Starts Automatically ----------*/

	while (!(QUADSPI->SR & QUADSPI_SR_TCF)); //Wait for operation to complete
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Clear flags

}


/*
 * This function writes a byte to a register of QSPI Flash via extended SPI, Dual SPI or Quad SPI
 *
 * @param		instruction		8-bit instruction (write to register command)
 * @param		registerContent	New register value
 * @param		dataLinesMode	1 = 1 data line
 * 								2 = 2 data lines
 * 								3 = 4 data lines
 * */
void QSPI_indirectWriteRegister(uint8_t instruction, uint8_t dataLinesMode, uint8_t registerContent) {

	while (QUADSPI->SR & QUADSPI_SR_BUSY); //Make sure no operation is going on
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; // Clear all flags
	QUADSPI->DLR = 1U - 1U; //Set number of bytes to write: 1
	QUADSPI->CR = (QUADSPI->CR & ~(QUADSPI_CR_FTHRES)) | ((1U - 1U) << QUADSPI_CR_FTHRES_Pos); //FIFO threshold: 1

	/*
	 * Set communication configuration register
	 * Functional mode: 			indirect write
	 * Data mode: 					as per param dataLinesMode
	 * Instruction mode: 			as per param dataLinesMode
	 * Instruction: 				as per param instruction
	 *
	 * Load data
	 *
	 */
	QUADSPI->CCR =
			(QSPI_FMODE_INDIRECT_WRITE << QUADSPI_CCR_FMODE_Pos) 	|
			(dataLinesMode << QUADSPI_CCR_DMODE_Pos) 				|
			(dataLinesMode << QUADSPI_CCR_IMODE_Pos) 				|
			(instruction << QUADSPI_CCR_INSTRUCTION_Pos);

	QUADSPI->DR = registerContent; //Load new register value into data register

	/* ---------- Communication Starts Automatically ----------*/

	while (!(QUADSPI->SR & QUADSPI_SR_TCF)); //Wait for the end of operation
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Clear all flags

}


/*
 * This function reads QSPI memory via Quad SPI
 *
 * @param		address			address of the first byte to read
 * @param		length			how many bytes to read
 * @param		destination		destination array
 *
 * */
void QSPI_readMemoryBytesQuad(uint32_t address, uint32_t length, uint8_t destination[]) {

	while (QUADSPI->SR & QUADSPI_SR_BUSY); //Make sure no operation is going on
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; // clear all flags
	QUADSPI->DLR = length - 1U; //Set number of bytes to read
	QUADSPI->CR = (QUADSPI->CR & ~(QUADSPI_CR_FTHRES)) | (0x00 << QUADSPI_CR_FTHRES_Pos); //Set FIFO threshold to 1

	/*
	 * Set communication configuration register
	 * Functional mode: 			Indirect read
	 * Data mode: 					4 Lines
	 * Instruction mode: 			4 Lines
	 * Address mode:				4 Lines
	 * Address size:				24 Bits
	 * Dummy cycles:				6 Cycles
	 * Instruction: 				Quad Output Fast Read
	 *
	 * Set 24-bit Address
	 *
	 */

	QUADSPI->CCR =
			(QSPI_FMODE_INDIRECT_READ << QUADSPI_CCR_FMODE_Pos) 			|
			(QIO_QUAD << QUADSPI_CCR_DMODE_Pos) 							|
			(QIO_QUAD << QUADSPI_CCR_IMODE_Pos)								|
			(QIO_QUAD << QUADSPI_CCR_ADMODE_Pos) 							|
			(QSPI_ADSIZE_24 << QUADSPI_CCR_ADSIZE_Pos) 						|
			(0x06 << QUADSPI_CCR_DCYC_Pos)									|
			(MT25QL128ABA1EW9_COMMAND_QUAD_OUTPUT_FAST_READ << QUADSPI_CCR_INSTRUCTION_Pos);

	QUADSPI->AR = (0xFFFFFF) & address;

	/* ---------- Communication Starts Automatically ----------*/

	while (QUADSPI->SR & QUADSPI_SR_BUSY) {
		if (QUADSPI->SR & QUADSPI_SR_FTF) {
			*destination = *((uint8_t*) &(QUADSPI->DR)); //Read a byte from data register, byte access
			destination++;
		}
	}
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Clear flags

}


/*
 * This function erases QSPI memory via Quad SPI
 *
 * @param		address			address of the first byte of subsector to erase
 * @param		instruction		8-bit instruction (various erase commands)
 *
 * */
void QSPI_eraseQuad(uint8_t instruction, uint32_t address) {

	QSPI_indirectWriteInstruction(MT25QL128ABA1EW9_COMMAND_WRITE_ENABLE, QIO_QUAD); //erase requires write enable command first
	while (QUADSPI->SR & QUADSPI_SR_BUSY); //Make sure no operation going on
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; // clear all flags

	/*
	 * Set communication configuration register
	 * Functional mode: 			indirect write
	 * Address mode:				4 lines
	 * Address size:				24 bits
	 * Address:						as per param address (any address in subsector is valid)
	 * Instruction mode: 			4 lines
	 * Instruction: 				as per param instruction
	 *
	 * Set 24-bit Address
	 *
	 */
	QUADSPI->CCR = (QSPI_FMODE_INDIRECT_WRITE << QUADSPI_CCR_FMODE_Pos) | (QIO_QUAD << QUADSPI_CCR_ADMODE_Pos) | (0x02 << QUADSPI_CCR_ADSIZE_Pos)
			| (QIO_QUAD << QUADSPI_CCR_IMODE_Pos) | (instruction << QUADSPI_CCR_INSTRUCTION_Pos);
	QUADSPI->AR = 0xFFFFFF & address;

	/* ---------- Communication Starts Automatically ----------*/

	while (QUADSPI->SR & QUADSPI_SR_BUSY); //wait until operation ends

	/* ---------- Waiting for erase process to end ---------- */

	uint8_t eraseEnded = 0x00;
	do {
		QSPI_readRegister(MT25QL128ABA1EW9_COMMAND_READ_FLAG_STATUS_REGISTER, QIO_QUAD, &eraseEnded);
		eraseEnded &= 0x80;
	} while (eraseEnded == 0x00);

	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Erase ended; clear flags

}

void QSPI_programMemoryBytesQuad(uint32_t address, uint32_t length, uint8_t data[]) {

	QSPI_indirectWriteInstruction(MT25QL128ABA1EW9_COMMAND_WRITE_ENABLE, QIO_QUAD); //program requires write enable
	while (QUADSPI->SR & QUADSPI_SR_BUSY); //Wait for the ongoing operations to finish
	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; // clear all flags
	QUADSPI->DLR = length - 1U; //Set number of bytes to write
	QUADSPI->CR = (QUADSPI->CR & ~(QUADSPI_CR_FTHRES)) | ((1U - 1U) << QUADSPI_CR_FTHRES_Pos); //Set FIFO threshold: 1

	/*
	 * Set communication configuration register
	 * Functional mode: 			Indirect write
	 * Data mode: 					4 lines
	 * Instruction mode: 			4 lines
	 * Address mode:				4 lines
	 * Address size:				24 bits
	 * Instruction: 				Quad Input Fast Program
	 *
	 * Write 24-bit Address
	 *
	 */
	QUADSPI->CCR =
			(QSPI_FMODE_INDIRECT_WRITE << QUADSPI_CCR_FMODE_Pos) 	|
			(QIO_QUAD << QUADSPI_CCR_DMODE_Pos) 					|
			(QIO_QUAD << QUADSPI_CCR_IMODE_Pos)						|
			(QIO_QUAD << QUADSPI_CCR_ADMODE_Pos) 					|
			(QSPI_ADSIZE_24 << QUADSPI_CCR_ADSIZE_Pos)				|
			(MT25QL128ABA1EW9_COMMAND_QUAD_INPUT_FAST_PROGRAM << QUADSPI_CCR_INSTRUCTION_Pos);
	QUADSPI->AR = 0xFFFFFF & address;
	uint32_t data_pointer = 0;
	do {
		*((uint8_t*) (&(QUADSPI->DR))) = data[data_pointer]; //place data - byte access
		data_pointer++;
		while ((QUADSPI->SR & QUADSPI_SR_FLEVEL) != 0x00); //wait for the data to be shifted out
	} while ((QUADSPI->SR & QUADSPI_SR_BUSY));

	/* ---------- Communication Starts Automatically, ends at this point ----------*/

	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //clear flags

	/* ---------- Checking status register to make sure writing operation has finished ---------- */
	uint8_t readyStatusFlag = 0x00;
	do {
		QSPI_readRegister(MT25QL128ABA1EW9_COMMAND_READ_FLAG_STATUS_REGISTER, QIO_QUAD, &readyStatusFlag);
		readyStatusFlag &= 0x80;
	} while (readyStatusFlag == 0x00);

	QUADSPI->FCR = QUADSPI_FCR_CTOF | QUADSPI_FCR_CSMF | QUADSPI_FCR_CTCF | QUADSPI_FCR_CTEF; //Clear flags

}
