#include "el_utils.h"

uint8_t upperNibbleToAscii(uint8_t input) {
	input = input >> 4;
	if (input < 0x0A)
		return (input + 0x30);
	else
		return (input + 0x37);
}

uint8_t lowerNibbleToAscii(uint8_t input) {
	input = input & 0x0F;
	if (input < 0x0A)
		return (input + 0x30);
	else
		return (input + 0x37);
}
