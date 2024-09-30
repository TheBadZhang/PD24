#ifndef __TBZ_SSD1312_H__
#define __TBZ_SSD1312_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SSD1312_IIC_ADDRESS 0x78
extern uint8_t OLED_GRAM[8*128];
extern uint8_t ssd1312_rotation;    // 屏幕旋转方向

void ssd1312_init(uint8_t rotation);
void ssd1312_invColor(void);
void ssd1312_display_on(void);
void ssd1312_display_off(void);
void ssd1312_clear(void);
void ssd1312_sendBuffer(void);
void ssd1312_setRotation(uint8_t rotation);

void ssd1312_drawPixel(uint8_t x, uint8_t y, uint8_t t);
void ssd1312_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t t);
void ssd1312_drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t t);
void ssd1312_drawSeg(uint8_t x, uint8_t y, uint8_t t);
uint8_t ssd1312_getSeg(uint8_t x, uint8_t y);
void ssd1312_drawSegs(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t t);

void ssd1312_drawXBMP(uint8_t x, uint8_t y, uint8_t* bmp, uint8_t w, uint8_t h); // 画位图


uint8_t isnum(char c);

void ssd1312_showchar(uint8_t x, uint8_t y, uint8_t num, const uint8_t* font, uint8_t w, uint8_t h);
void ssd1312_showstr(short x, short y, const char* str, const uint8_t str_len, const uint8_t* font, uint8_t w, uint8_t h, uint8_t spacing_x, int8_t newline);


// 用户自定义函数

void ssd1312_write_byte(uint8_t addr, uint8_t data);            // 有阻塞写入寄存器
void ssd1312_write_bytes(uint8_t addr, uint8_t* data, int len); // 无阻塞写入寄存器
void ssd1312_set_rst(void);                                     // 复位
void ssd1312_clr_rst(void);                                     // 取消复位
void ssd1312_delay(void);                                       // 延迟函数


#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __TBZ_SSD1312_H__