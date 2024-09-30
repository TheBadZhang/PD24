#include "ws2812.h"


uint32_t colors [4] = {0xff0000, 0x00ff00, 0x0000ff};
uint8_t buffer [sizeof(colors)*8] = {};

void ws2812_convert (uint32_t* color, int len) {
	int count = 0;
	for (int i = 0; i < len; i++) {
		uint32_t c = color[i];
		for (int j = 0; j < 24; j++) {
			if (c & 0x800000) {
				buffer[count++] = WS_BIT_1;
			} else {
				buffer[count++] = WS_BIT_0;
			}
			c <<= 1;
		}
	}
}

