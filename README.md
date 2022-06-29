# STM32F746-DISCO-QUADSPI-INDIRECT-CMSIS-DEMO
STM32F746-DISCO QuadSPI Minimal Demo

USART1 ST-Link COM Port

Clock: 216MHz.
USART1 is the USART connected to ST-Link that makes virtual COM port on PC.
QuadSPI is connected to QSPI Nor Flash MT25QL128ABA1EW9
This project is a proof of concept (that I can).

Might be useful for someone who wants to figure out how the stuff works.
I commented almost every line to make it simple to understand.


Hardware:
STM32F746 Disco board (QSPI Flash MT25QL128ABA1EW9), Windows PC


Software:
STM32CubeIDE, Putty for UART communication;
No libraries used except for CMSIS register definitions.
