#include "ina226.h"
#include <stdint.h>


float current_lsb = 0.0;
float power_lsb;

/**
 * @brief INA226 初始化
 * @param config INA226配置寄存器
 * @param r_shunt 采样电阻值(mΩ)
 * @param maxExpectedCurrent 最大期望电流(mA)
 */
void ina226_init(uint16_t config, float r_shunt, float maxExpectedCurrent) {
	ina226_setConfig(config);
	// INA226_setConfig(&hi2c2, INA226_CONFIG, 0x45ff);

	// 校准寄存器值计算公式如下（注意单位）：
	// current_lsb(mA) = maxExpectedCurrent(mA) / 32768
	// calib = 0.00512 / (current_lsb(mA) * r_shunt(mΩ))
	current_lsb = maxExpectedCurrent / 32768.0;
	power_lsb = current_lsb * 25;
	uint16_t calib = 5120 / (current_lsb * r_shunt);
	ina226_setCalibrationReg(calib);
}

/**
 * @brief INA226 读取电压数值寄存器
 */
float ina226_getBusV (void) {
	return ina226_readReg(INA226_BUSV) * INA226_BUSV_LSB;
}

/**
 * @brief INA226 读取电流数值寄存器(mA)
 */
float ina226_getCurrent (void) {
	// 因为ina226的SHUNT_V存在+-2.5uV的误差，所以这里直接使用带符号进行处理避免意外大值
	return (int16_t)(ina226_readReg(INA226_CURRENT)) * current_lsb;
}


/**
 * @brief INA226 读取功率数值寄存器(mW)
 */
float ina226_getPower (void) {
	return ina226_getPowerReg() * power_lsb;
}

uint16_t ina226_getPowerReg(void) {
	return ina226_readReg(INA226_POWER);
}

uint8_t ina226_setConfig(uint16_t config) {
	return ina226_writeReg(INA226_CONFIG, config);
}

uint16_t ina226_getConfig(void) {
	return ina226_readReg(INA226_CONFIG);
}

uint16_t ina226_getShuntV (void) {
	return ina226_readReg(INA226_SHUNTV);
}

uint8_t ina226_setCalibrationReg (uint16_t config) {
	return ina226_writeReg(INA226_CALIB, config);
}

uint16_t ina226_getCalibrationReg (void) {
	return ina226_readReg(INA226_CALIB);
}

uint16_t ina226_getManufID (void) {
	return ina226_readReg(INA226_MANUF_ID);
}

uint16_t ina226_getDieID (void) {
	return ina226_readReg(INA226_DIE_ID);
}

uint8_t ina226_setMaskEnable (uint16_t config) {
	return ina226_writeReg(INA226_MASK, config);
}

uint16_t ina226_getMaskEnable (void) {
	return ina226_readReg(INA226_MASK);
}

uint8_t ina226_setAlertLimit (uint16_t config) {
	return ina226_writeReg(INA226_ALERTL, config);
}

uint16_t ina226_getAlertLimit (void) {
	return ina226_readReg(INA226_ALERTL);
}
