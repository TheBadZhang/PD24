#ifndef __WS2812_H__
#define __WS2812_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define WS_BIT_1            0b01111100
#define WS_BIT_0            0b01110000
#define WS_RESET            0b00000000

extern uint32_t colors [4];
extern uint8_t buffer [sizeof(colors)*8];
void ws2812_convert (uint32_t* color, int len);

// 用户自定义函数

void ws2812_display(void);




#ifdef __cplusplus
}
#endif

#endif // __WS2812_H__