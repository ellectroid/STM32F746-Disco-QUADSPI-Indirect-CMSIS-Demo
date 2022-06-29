/*
 * qspi.h
 *
 *  Created on: Apr 13, 2022
 *      Author: User
 */

#ifndef QSPI_H_
#define QSPI_H_
#include "stm32f746xx.h"
#include "MT25QL128ABA1EW9.h"

/* ---------- QUADSPI Definitions  ---------- */

#define QSPI_ADSIZE_8 ((uint8_t)0x00)
#define QSPI_ADSIZE_16 ((uint8_t)0x01)
#define QSPI_ADSIZE_24 ((uint8_t)0x02)
#define QSPI_ADSIZE_32 ((uint8_t)0x03)

#define QIO_SKIP ((uint8_t)0x00)
#define QIO_SINGLE ((uint8_t)0x01)
#define QIO_DOUBLE ((uint8_t)0x02)
#define QIO_QUAD ((uint8_t)0x03)

#define QSPI_FMODE_INDIRECT_WRITE ((uint8_t)0x00)
#define QSPI_FMODE_INDIRECT_READ ((uint8_t)0x01)
#define QSPI_FMODE_AUTOMATIC_POLLING ((uint8_t)0x02)
#define QSPI_FMODE_MEMORY_MAPPED ((uint8_t)0x03)


/* ---------- QUADSPI Functions ---------- */

void QSPI_setupIndirect(void);

void QSPI_indirectWriteInstruction(uint8_t instruction, uint8_t dataLinesMode);

void QSPI_indirectWriteRegister(uint8_t instruction, uint8_t dataLinesMode, uint8_t registerContent);
void QSPI_readRegister(uint8_t instruction, uint8_t dataLinesMode, uint8_t *destination);

void QSPI_programMemoryBytesQuad(uint32_t address, uint32_t length, uint8_t data[]);
void QSPI_readMemoryBytesQuad(uint32_t address, uint32_t length, uint8_t destination[]);
void QSPI_eraseQuad(uint8_t instruction, uint32_t address);





#endif /* QSPI_H_ */
