#include "ssd1312.h"
#include "font.h"

uint8_t OLED_GRAM[8*128] = {};

uint8_t ssd1312_rotation;

const uint8_t rotation_sequence[][4] = {
	// 页寻址模式，无法使用dma发送数据
	// {0x20, 0x09, 0xa1, 0xc8},
	// {0x20, 0x02, 0xa1, 0xc0},
	// {0x20, 0x09, 0xa0, 0xc0},
	// {0x20, 0x02, 0xa0, 0xc8}

	// 列寻址模式，可以使用dma发送数据
	{0x20, 0x09, 0xa1, 0xc8},
	{0x20, 0x01, 0xa1, 0xc0},
	{0x20, 0x09, 0xa0, 0xc0},
	{0x20, 0x01, 0xa0, 0xc8}
};

const uint8_t initial_sequence[] = {
	0xAE, // 关闭屏幕

	0x00, // 设置低列起始地址
	0x10, // 设置高列起始地址

	0xB0, // 设置页地址

	0x81, // 设置对比度
	0x5f, // 为 128

	// 设置旋转

	0xA4, // 关闭全屏显示

	0xA6, // 设置正常显示

	0xA8, // 设置多路复用比
	0x3f, // 为 1/64

	0xD3, // 设置显示偏移
	0x00, // 为 0

	0xD5, // 设置振荡分频
	0x80, // 为 0x80

	0xD9, // 设置电荷泵
	0x10, // 为 0x10

	0xDA, // 设置硬件配置
	0x10, // 为 0x02

	0xDB, // 设置VCOMH
	0x30, // 为 0x30

	0x8D, // 设置电荷泵
	0x72, // 0x12:7.5V; 0x52:8V;  0x72:9V;  0x92:10V

	0xAF, // 打开屏幕
};


void ssd1312_init(uint8_t rotation) {
	ssd1312_clr_rst();
	ssd1312_delay();
	ssd1312_set_rst();
	ssd1312_setRotation(rotation);
	for (uint8_t i = 0; i < sizeof(initial_sequence); i++) {
		ssd1312_write_byte(0x00, initial_sequence[i]);
	}
}
void ssd1312_normalColor(void) {
	ssd1312_write_byte(0x00, 0xA6); // 设置正常显示
}
void ssd1312_invColor(void) {
	ssd1312_write_byte(0x00, 0xA7); // 设置反色显示
}

void ssd1312_display_on(void) {
	ssd1312_write_byte(0x00, 0xAF); // 打开屏幕
}

void ssd1312_display_off(void) {
	ssd1312_write_byte(0x00, 0xAE); // 关闭屏幕
}

void ssd1312_setRotation(uint8_t rotation) {
	ssd1312_rotation = rotation;
	for (uint8_t i = 0; i < 4; i++) {
		ssd1312_write_byte(0x00, rotation_sequence[ssd1312_rotation][i]);
	}
}

void ssd1312_sendBuffer(void) {
	//* 页寻址模式，无法使用dma发送数据
	/*
	for (uint8_t i = 0; i < 8; i++) {
		ssd1312_write_byte(0x00, 0xB0 + i); // 设置页地址
		ssd1312_write_byte(0x00, 0x00); // 设置低列起始地址
		ssd1312_write_byte(0x00, 0x10); // 设置高列起始地址
		for (uint8_t j = 0; j < 128; j++) {
			ssd1312_write_byte(0x40, OLED_GRAM[j + i * 128]);
		}
	}
	*/
	ssd1312_write_bytes(0x40, OLED_GRAM, 128*8);
}

void ssd1312_clear(void) {
	for (uint16_t i = 0; i < 128*8; i++) {
		OLED_GRAM[i] = 0x00;
	}
}

void ssd1312_drawPixel(uint8_t x, uint8_t y, uint8_t t) {
	uint8_t n = y / 8;
	if (t) {
		ssd1312_drawSeg(x, n, ssd1312_getSeg(x, n) | 1 << (y % 8));
	} else {
		ssd1312_drawSeg(x, n, ssd1312_getSeg(x, n) & ~(1 << (y % 8)));
	}
}

uint8_t mabs(uint8_t x) {
	if (x < 0) {
		return -x;
	}
	return x;
}

void ssd1312_drawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t t) {

	int16_t dx = mabs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int16_t dy = mabs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int16_t erro = (dx > dy ? dx : -dy) / 2;

	while(ssd1312_drawPixel(x0, y0, 1), x0 != x1 || y0 != y1){
		int16_t e2 = erro;
		if(e2 > -dx) { erro -= dy; x0 += sx;}
		if(e2 <  dy) { erro += dx; y0 += sy;}
	}
}

void ssd1312_drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t t) {
	for (uint8_t i = x; i < x + w; i++) {
		for (uint8_t j = y; j < y + h; j++) {
			ssd1312_drawPixel(i, j, t);
		}
	}
}

void ssd1312_drawSeg(uint8_t x, uint8_t y, uint8_t t) {
	if (x >= 0 && y >= 0) {
		if (ssd1312_rotation == 1 || ssd1312_rotation == 3) {
			if (y < 8 && x < 128) OLED_GRAM[y + x * 8] = t;
		} else {
			if (y < 16 && x < 64) OLED_GRAM[y*64 + x] = t;
		}
	}
}
uint8_t ssd1312_getSeg(uint8_t x, uint8_t y) {
	if (ssd1312_rotation == 1 || ssd1312_rotation == 3) {
		if (y < 8 && x < 128) return OLED_GRAM[y + x * 8];
	} else {
		if (y < 16 && x < 64) return OLED_GRAM[y*64 + x];
	}
	return 0;
}


void ssd1312_drawSegs(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t t) {
	if (x1 < x) {
		uint8_t tmp = x;
		x = x1;
		x1 = tmp;
	}
	if (y1 < y) {
		uint8_t tmp = y;
		y = y1;
		y1 = tmp;
	}
	for (uint8_t i = x; i <= x1; i++) {
		for (uint8_t j = y; j <= y1; j++) {
			ssd1312_drawSeg(i, j, t);
		}
	}
}

uint8_t isnum(char c) {
	return (c >= '0' && c <= '9');
}

void ssd1312_showchar(uint8_t x, uint8_t y, uint8_t num, const uint8_t* font, uint8_t w, uint8_t h) {
	h= (h+7)/8;
	for (uint8_t i = 0; i < w; i++) {
		for (uint8_t j = 0; j < h; j++) {
			// OLED_GRAM[(i+x)*8+j+y] = num_10x24[num][j*10+i];
			ssd1312_drawSeg(i+x, j+y, font[num*w*h+j*w+i]);
			// ssd1312_drawSeg(i+x, j+y, num_10x24[num][j*w+i]);
		}
	}
}
/**
 * @brief 字符串绘制函数
 * @param x x坐标
 * @param y y坐标
 * @param str 字符串
 * @param str_len 字符串长度
 * @param font 字体数据
 * @param w 字体宽度
 * @param h 字体高度
 * @param spacing_x 字符间距
 * @param newline 最大显示行数
 */
void ssd1312_showstr(short x, short y, const char* str, const uint8_t str_len, const uint8_t* font, uint8_t w, uint8_t h, uint8_t spacing_x, int8_t newline) {
	uint8_t ww = ((ssd1312_rotation == 0 || ssd1312_rotation == 2)? 64-w : 128-w);
	for (uint8_t i = 0; i < str_len; i++) {
		// if (isnum(str[i]))
		if (x >= -w && x <= ww) {
			ssd1312_showchar(x, y, str[i]-' ', font, w, h);
		}
		if (x > ww) {
			if (--newline > 0) {
					x = x-ww;
					y += h/8+1;
			} else {
				return;
			}
		} else {
			x += w+spacing_x;
		}
	}
}

void ssd1312_drawXBMP(uint8_t x, uint8_t y, uint8_t* bmp, uint8_t w, uint8_t h) {
	for (uint8_t i = 0; i < w; i++) {
		for (uint8_t j = 0; j < h; j++) {
			ssd1312_drawSeg(x+i, y+j, bmp[j*w+i]);
		}
	}
}