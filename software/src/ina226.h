#ifndef F7_INA226_H
#define F7_INA226_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef INA226_ADDRESS
	#define INA226_ADDRESS 0x80
#endif

#define INA226_CALIB_VAL	  1024
#define INA226_CURRENTLSB	  0.5F							 // mA/bit
#define INA226_CURRENTLSB_INV 1 / INA226_CURRENTLSB			 // bit/mA
#define INA226_POWERLSB_INV	  1 / (INA226_CURRENTLSB * 25)	 // bit/mW
#define INA226_I2CTIMEOUT	  10

// current_lsb = maxExpectedCurrent / 32768
// calib = 0.00512 / (current_lsb * r_shunt)
extern float current_lsb;             // 电流最小分辨率（动态值）与应用（校准值）相关
// power_lsb = current_lsb * 25
extern float power_lsb;               // 功率最小分辨率（动态值）与应用（校准值）相关
#define INA226_BUSV_LSB   0.00125f    // 总线电压最小分辨率（固定值） 1.25mV/bit，满量程 40.96V
#define INA226_SHUNTV_LSB 0.0000025f  // 分流电压最小分辨率（固定值） 2.5uV/bit，满量程 81.92mV

#define INA226_CONFIG	0x00   // Configuration Register (R/W)
#define INA226_SHUNTV	0x01   // Shunt Voltage ®
#define INA226_BUSV		0x02   // Bus Voltage ®
#define INA226_POWER	0x03   // Power ®
#define INA226_CURRENT	0x04   // Current ®
#define INA226_CALIB	0x05   // Calibration (R/W)
#define INA226_MASK		0x06   // Mask/Enable (R/W)
#define INA226_ALERTL	0x07   // Alert Limit (R/W)
#define INA226_MANUF_ID 0xFE   // Manufacturer ID ®
#define INA226_DIE_ID	0xFF   // Die ID ®

#define INA226_MODE_POWER_DOWN		   (0 << 0)	  // Power-Down
#define INA226_MODE_TRIG_SHUNT_VOLTAGE (1 << 0)	  // Shunt Voltage, Triggered
#define INA226_MODE_TRIG_BUS_VOLTAGE   (2 << 0)	  // Bus Voltage, Triggered
#define INA226_MODE_TRIG_SHUNT_AND_BUS (3 << 0)	  // Shunt and Bus, Triggered
#define INA226_MODE_POWER_DOWN2		   (4 << 0)	  // Power-Down
#define INA226_MODE_CONT_SHUNT_VOLTAGE (5 << 0)	  // Shunt Voltage, Continuous
#define INA226_MODE_CONT_BUS_VOLTAGE   (6 << 0)	  // Bus Voltage, Continuous
#define INA226_MODE_CONT_SHUNT_AND_BUS (7 << 0)	  // Shunt and Bus, Continuous

// Shunt Voltage Conversion Time
#define INA226_VSH_140uS  (0 << 3)
#define INA226_VSH_204uS  (1 << 3)
#define INA226_VSH_332uS  (2 << 3)
#define INA226_VSH_588uS  (3 << 3)
#define INA226_VSH_1100uS (4 << 3)
#define INA226_VSH_2116uS (5 << 3)
#define INA226_VSH_4156uS (6 << 3)
#define INA226_VSH_8244uS (7 << 3)

// Bus Voltage Conversion Time (VBUS CT Bit Settings[6-8])
#define INA226_VBUS_140uS  (0 << 6)
#define INA226_VBUS_204uS  (1 << 6)
#define INA226_VBUS_332uS  (2 << 6)
#define INA226_VBUS_588uS  (3 << 6)
#define INA226_VBUS_1100uS (4 << 6)
#define INA226_VBUS_2116uS (5 << 6)
#define INA226_VBUS_4156uS (6 << 6)
#define INA226_VBUS_8244uS (7 << 6)

// Averaging Mode (AVG Bit Settings[9-11])
#define INA226_AVG_1	(0 << 9)
#define INA226_AVG_4	(1 << 9)
#define INA226_AVG_16	(2 << 9)
#define INA226_AVG_64	(3 << 9)
#define INA226_AVG_128	(4 << 9)
#define INA226_AVG_256	(5 << 9)
#define INA226_AVG_512	(6 << 9)
#define INA226_AVG_1024 (7 << 9)

// Reset Bit (RST bit [15])
#define INA226_RESET_ACTIVE	  (1 << 15)
#define INA226_RESET_INACTIVE (0 << 15)

// Mask/Enable Register
#define INA226_MER_SOL	(1 << 15)	// Shunt Voltage Over-Voltage
#define INA226_MER_SUL	(1 << 14)	// Shunt Voltage Under-Voltage
#define INA226_MER_BOL	(1 << 13)	// Bus Voltagee Over-Voltage
#define INA226_MER_BUL	(1 << 12)	// Bus Voltage Under-Voltage
#define INA226_MER_POL	(1 << 11)	// Power Over-Limit
#define INA226_MER_CNVR (1 << 10)	// Conversion Ready
#define INA226_MER_AFF	(1 << 4)	// Alert Function Flag
#define INA226_MER_CVRF (1 << 3)	// Conversion Ready Flag
#define INA226_MER_OVF	(1 << 2)	// Math Overflow Flag
#define INA226_MER_APOL (1 << 1)	// Alert Polarity Bit
#define INA226_MER_LEN	(1 << 0)	// Alert Latch Enable

#define INA226_MANUF_ID_DEFAULT 0x5449
#define INA226_DIE_ID_DEFAULT	0x2260

// #define

/**
 * @brief INA226 初始化
 * @param config INA226配置寄存器
 * @param r_shunt 采样电阻值(mΩ)
 */
void ina226_init(uint16_t config, float r_shunt, float maxExpectedCurrent_);

/**
 * @brief INA226 读取电压数值寄存器
 */
float ina226_getBusV (void);
float ina226_getCurrent (void);
float ina226_getPower (void);
uint16_t ina226_getPowerReg(void);
uint8_t ina226_setConfig(uint16_t config);
uint16_t ina226_getConfig(void);
uint16_t ina226_getShuntV (void);
uint8_t ina226_setCalibrationReg (uint16_t config);
uint16_t ina226_getCalibrationReg (void);
uint16_t ina226_getManufID (void);
uint16_t ina226_getDieID (void);
uint8_t ina226_setMaskEnable (uint16_t config);
uint16_t ina226_getMaskEnable (void);
uint8_t ina226_setAlertLimit (uint16_t config);
uint16_t ina226_getAlertLimit (void);


// 用户自定义函数
uint8_t ina226_writeReg (uint16_t regAddress, uint16_t regValue);
// 无符号读取寄存器
uint16_t ina226_readReg (uint16_t regAddress);
// 带符号读取寄存器
int16_t ina226_readRegS (uint16_t regAddress);

#ifdef __cplusplus
}
#endif
#endif