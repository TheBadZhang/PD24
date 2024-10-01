#include "lis2dw12.h"


void lis2dw12_init(void) {
	lis2dw12_writeReg(LIS2DW12_CTRL1, 0x77);  // 使能X、Y、Z轴，ODR=100Hz，连续模式
}

int16_t lis2dw12_readTemprature(void) {
	uint8_t low = lis2dw12_readReg(LIS2DW12_OUT_T_L);
	uint8_t high = lis2dw12_readReg(LIS2DW12_OUT_T_H);
	return ((high << 8) | low) >> 4;
}
int8_t lis2dw12_readTemprature8bit(void) {
	return lis2dw12_readReg(LIS2DW12_OUT_T);
}

int16_t lis2dw12_readX(void) {
	uint8_t low = lis2dw12_readReg(LIS2DW12_OUT_X_L);
	uint8_t high = lis2dw12_readReg(LIS2DW12_OUT_X_H);
	return (high << 8) | low;
}
int16_t lis2dw12_readY(void) {
	uint8_t low = lis2dw12_readReg(LIS2DW12_OUT_Y_L);
	uint8_t high = lis2dw12_readReg(LIS2DW12_OUT_Y_H);
	return (high << 8) | low;
}
int16_t lis2dw12_readZ(void) {
	uint8_t low = lis2dw12_readReg(LIS2DW12_OUT_Z_L);
	uint8_t high = lis2dw12_readReg(LIS2DW12_OUT_Z_H);
	return (high << 8) | low;
}

void lis2dw12_single_tap_init(void) {
	lis2dw12_writeReg(LIS2DW12_CTRL6, 0x04);
	lis2dw12_writeReg(LIS2DW12_TAP_THS_X, 0x09);
	lis2dw12_writeReg(LIS2DW12_TAP_THS_Y, 0x09);
	lis2dw12_writeReg(LIS2DW12_TAP_THS_Z, 0xe9);
	lis2dw12_writeReg(LIS2DW12_INT_DUR, 0x05);
	lis2dw12_writeReg(LIS2DW12_WAKE_UP_THS, 0x00);
	lis2dw12_writeReg(LIS2DW12_CTRL4, 0x40);
	lis2dw12_writeReg(LIS2DW12_CTRL7, 0x20);
}
void lis2dw12_double_tap_init(void) {
	lis2dw12_writeReg(LIS2DW12_CTRL6, 0x04);
	lis2dw12_writeReg(LIS2DW12_TAP_THS_X, 0x09);
	lis2dw12_writeReg(LIS2DW12_TAP_THS_Y, 0xe9);
	lis2dw12_writeReg(LIS2DW12_TAP_THS_Z, 0xe9);
	lis2dw12_writeReg(LIS2DW12_INT_DUR, 0x7f);
	lis2dw12_writeReg(LIS2DW12_WAKE_UP_THS, 0x80);
	lis2dw12_writeReg(LIS2DW12_CTRL4, 0x08);
	lis2dw12_writeReg(LIS2DW12_CTRL7, 0x20);
}

uint8_t lis2dw12_single_tap_event_status(void) {
	return lis2dw12_readReg(LIS2DW12_TAP_SRC) & 0x20; // 仅返回单击事件状态
}

uint8_t lis2dw12_6d_detection(void) {
	// lis2dw12_writeReg(LIS2DW12_CTRL4, 0xc0);  // 使能INT1上的6D和单击中断输出
	uint8_t value = lis2dw12_readReg(LIS2DW12_SIXD_SRC);  // 读取6D事件源寄存器
	// struct SIXD_SRC_t *sixd = (struct SIXD_SRC_t*)&value;
	if (value & 0x08) {
		return 1;
	} else if (value & 0x04) {
		return 2;
	} else if (value & 0x02) {
		return 3;
	} else if (value & 0x01) {
		return 0;
	}
	return 0;
}