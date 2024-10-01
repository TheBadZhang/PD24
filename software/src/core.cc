#include "common.h"
#include "core.h"

#include "trick.h"
#include "main.h"

#include <stdio.h>
char strbuf[128] = {0};
uint8_t str_len = 0;

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "font.h"
#include "ssd1312.h"

#include "ina226.h"

#include "ws2812.h"

#include "husb238.h"

#include "lis2dw12.h"


char uart_rx_buf[32];


//***********按键处理*************//
enum KEY_EVENT {
	KEY_NONE = 0,
	KEY_DOWN = 1,
	KEY_UP = 2
};

uint16_t key[4] = {0x00};
bool key_flag[4] = {true, true, true, true};

// 一般来说按键扫描函数需要按照5kHz的频率对按键进行扫描
void key_scan(void) {

	key[1] = (key[1] << 1) | ((!read(KEY1))?0x01:0x00);
	key[2] = (key[2] << 1) | ((!read(KEY2))?0x01:0x00);
	key[3] = (key[3] << 1) | ((!read(KEY3))?0x01:0x00);
}
/**
 * @brief 按键处理函数
 * @param i 按键编号
 * @return 0 无按键动作 1 按键按下 2 按键释放
 *
 * @note 该函数会自动处理按键的抖动
 * @note 改函数还需要和按键扫描函数配合使用
 */
uint8_t key_push(uint8_t i) {
	if (key[i] == 0xffff && key_flag[i]) {
		key_flag[i] = false;
		return 1;
	} else if (key[i] == 0x0000 && !key_flag[i]) {
		key_flag[i] = true;
		return 2;
	}

	return 0;
}
bool key_pressed(uint8_t i) {
	return key[i] == 0xffff;
}
bool key_released(uint8_t i) {
	return key[i] == 0x0000;
}


//***********INA226数据处理*************//
float bus_voltage;
int16_t current_raw;
float current;
uint16_t power_raw;
float power;
float power_sum = 0;


void ina226_sample(void) {
	bus_voltage = ina226_getBusV();
	current = ina226_getCurrent();
	power_raw = ina226_getPowerReg();
	power = ina226_getPower();

	// 将电压、电流、功率数据传给上位机
	// int bus_voltage_i = bus_voltage * 10000;
	// int current_i = current * 10000;
	// int power_i = power*10000;
	// str_len = sprintf(strbuf, "ina226: %d.%04d, %d.%04d, %d.%04d\r\n",
	// 	bus_voltage_i/10000, bus_voltage_i%10000,
	// 	current_i/10000, current_i%10000,
	// 	power_i/10000, power_i%10000);
	// HAL_UART_Transmit_DMA(&huart2, (const uint8_t*)strbuf, str_len);
}


template<class T>
T returnmin(T* list, uint8_t len) {
	T min = list[0];
	for (uint8_t i = 0; i < len; i++) {
		if (list[i] < min) {
			min = list[i];
		}
	}
	return min;
}
template<class T>
T returnmax(T* list, uint8_t len) {
	T max = list[0];
	for (uint8_t i = 0; i < len; i++) {
		if (list[i] > max) {
			max = list[i];
		}
	}
	return max;
}



//***********几个独立的数据信息处理*************//
class STATUS_BAR {
	uint16_t list[128] = {0};
	int32_t sum = 0;
	int32_t average = 0;
	int32_t d;
	uint8_t index = 0;
	bool if_index_max = false;

	// uint8_t x, y, w;
	uint8_t hw;   // 宽度的一半
public:

	void set_w(uint8_t w) {
		hw = w/2;
	}

	//* 数据条相关数值计算
	void calc(const uint16_t power_raw) {
		sum -= list[index];           // 减去旧值
		list[index] = power_raw;      // 更新新值
		sum += list[index];           // 加上新值

		// 计算平均值
		// 根据计数长短，平均值计算有所不同
		if (if_index_max) {
			average = sum / 128;
			d = (int32_t)power_raw*hw - sum*hw / 128;
		} else {
			average = sum / (index+1);
			if (index == 127) {
				if_index_max = true;
			}
			d = (power_raw - average) * hw;
		}

		index = (index + 1) % 128;

	}

	void draw(uint8_t y) {

		// ssd1312_showchar(64, 7, 0, num_6x8, 6, 8);

		if (d >= 0) {
			d /= (returnmax(list, 128) - average + 1);
			//* 样式1
			// ssd1312_drawSegs(64, 8, 64+d, 8, 0xff);

			//* 样式2
			for (uint8_t i = hw; i < hw+1+d; i++) {
				if (i%2==0) {
					if ((i-hw) % 10 == 0) {
						ssd1312_showchar(i-2, y+1, (i-hw)/10, num_6x8, 6, 8);
						ssd1312_drawSeg(i, y, 0x7f);
					} else {
						ssd1312_drawSeg(i, y, 0x78);
					}
				}
			}

		} else {
			d /= (average - returnmin(list, 128) + 1);

			//* 样式1
			// ssd1312_drawSegs(64+d, 8, 64, 8, 0xff);

			//* 样式2
			for (uint8_t i = hw; i > hw-1+d; i--) {
				if (i%2==0) {
					if ((hw-i) % 10 == 0) {
						ssd1312_showchar(i-2, y+1, (hw-i)/10, num_6x8, 6, 8);
						ssd1312_drawSeg(i, y, 0x7f);
					} else {
						ssd1312_drawSeg(i, y, 0x78);
					}
				}
			}
		}
	}

} power_bar;

class FLOW_CHART {
	float chart_data[128];
	int index_of_data = 0;
public:
	float max, min;
	void add_data(float data) {
		chart_data[index_of_data] = data;
		index_of_data = (index_of_data + 1) % 128;
	}

	void draw(void) {

		ssd1312_drawLine(0, 8, 0, 64, 1);
		ssd1312_drawLine(0, 63, 127, 63, 1);

		max = returnmax(chart_data, 128);
		min = returnmin(chart_data, 128);

		for (int i = 0, i_ = index_of_data; i < 127; i++, i_ = (i_+1) % 128) {
			if (((i+index_of_data)) % 8 == 1) {
				for (int j = 0; j < 7; j++) {
					ssd1312_drawPixel(i, 8*(j+1)+5, 1);
				}
			}

			if (i > 1)
			// 1. 点风格
			// ssd1312_drawPixel(i, 61-chart_data[i_]/max*58, 1);

			// 2. 线风格
			ssd1312_drawLine(i, 61-(chart_data[i_]-min)/(max-min)*40, i+1, 61-(chart_data[(i_+1)%128]-min)/(max-min)*40, 1);

			// 3. 矩形风格
			// ssd1312_drawLine(i, 61-chart_data[i_]/max*40, i, 61, 1);
		}
	}
} power_chart;

// 链接器选项 -u _printf_float
// 浮点数显示
void show_float(uint8_t x, uint8_t y, const float num) {
	// 位数控制，保证显示的数字个数为4个
	uint8_t fixed = 3;
	if (num >= 1000) {
		fixed = 0;
	} else if (num >= 100) {
		fixed = 1;
	} else if (num >= 10) {
		fixed = 2;
	}
	// -u _printf_float
	// str_len = sprintf(strbuf, "%.*f", fixed, num);         // 如果空间充足的话可以使用这个输出浮点数
	int out_num = num*1000;
	str_len = sprintf(strbuf, "%d.%0*d", out_num/1000, fixed, out_num%1000);
	// 因为画面中只能显示5位数字，所以这边设吹了人为上限为 5 位
	for (uint8_t i = 0; i < 5; i++) {
		if (isnum(strbuf[i])) {
			ssd1312_showchar(x, y, strbuf[i]-'0', num_10x24, 10, 24);
			x += 11;
		} else if (strbuf[i] == '.' && i != 4) {
			ssd1312_drawBox(x+1, y*8+18, 3, 3, 1);
			x += 6;
		}// 否则什么也不做
	}
}

// 表主界面UI
void meterUI(void) {
	uint8_t v_x, v_y;
	uint8_t c_x, c_y;
	uint8_t p_x, p_y;
	uint8_t pm_x, pm_y;
	uint8_t pb_y;
	static int str_x = 0;
	static bool str_reverse = false;

	if (ssd1312_rotation == 1 || ssd1312_rotation == 3) {
		v_x = 0;
		v_y = 0;
		c_x = 0;
		c_y = 3;
		p_x = 64;
		p_y = 0;
		pm_x = 64;
		pm_y = 3;
		pb_y = 6;
	} else {
		v_x = 0;
		v_y = 0;
		c_x = 0;
		c_y = 3;
		p_x = 0;
		p_y = 6;
		pm_x = 0;
		pm_y = 9;
		pb_y = 14;
	}
	if (ssd1312_rotation == 1 || ssd1312_rotation == 3) {
		power_bar.set_w(128);
	} else {
		power_bar.set_w(64);
	}

	// 电压
	show_float(v_x, v_y, bus_voltage);
	ssd1312_showchar(v_x+50, v_y+1, 0, char_V2, 9, 16);

	// 电流
	if (current < 1000) {
		show_float(c_x, c_y, current);
		ssd1312_showchar(c_x+50, c_y+1, 0, char_m, 6, 8);
		ssd1312_showchar(c_x+50+6, c_y+1, 0, char_A, 6, 8);
	} else {
		show_float(c_x, c_y, current/1000);
		ssd1312_showchar(c_x+50, c_y+1, 0, char_A2, 9, 16);
	}

	// 功率
	if (power < 10000) {
		show_float(p_x, p_y, power);
		// ssd1312_showchar(p_x+50, p_y+1, 0, char_W2, 9, 16);
		ssd1312_showchar(p_x+50, p_y+1, 0, char_m, 6, 8);
		ssd1312_showchar(p_x+50+6, p_y+1, 0, char_W, 6, 8);
	} else {
		show_float(p_x, p_y, power/1000);
		ssd1312_showchar(p_x+50, p_y+1, 0, char_W2, 9, 16);
	}

	// 累计功率（瓦分钟）
	show_float(pm_x, pm_y, power_sum/60.0);
	ssd1312_showchar(pm_x+50, pm_y+1, 0, char_W, 6, 8);
	ssd1312_showchar(pm_x+50+6, pm_y+1, 0, char_m, 6, 8);


	// 竖屏显示下会多一块区域可以用于额外信息的显示
	if (ssd1312_rotation == 0 || ssd1312_rotation == 2) {
		str_len = sprintf(strbuf, "THIS IS A SO FUCKING LONG SCENTANCE THAT CANNOT DISPLAY NORMALLY!!");
		int8_t lines = 2;
		// ssd1312_setFont(font_Fixedsys, 8, 16, 1, 2);
		// ssd1312_setFont(font0816, 8, 16, 1, 2);
		ssd1312_setFont(font_0507, 5, 7, 1, 2);
		ssd1312_showstr(str_x, 12, strbuf, str_len);
		if (str_reverse) {
			if (str_x >= 0) {
				str_reverse = false;
			} else {
				str_x ++;
			}
		} else {
			if (str_x+(str_len)*5 <= 64*lines) {
				str_reverse = true;
			} else {
				str_x--;
			}
		}

		ssd1312_showchar(0, 12, 0, usbmeter2, 64, 16);
	}

	// 功率条（当前功率与近一段时间平均功率比值）
	power_bar.draw(pb_y);

}

float chart_data[128];
int index_of_data = 0;
void meterUI_flowchart(void) {

	ssd1312_setFont(font_0507, 5, 7, 1, 1);

	int current_i = current, voltage_i = bus_voltage*1000, power_i = power;
	str_len = sprintf(strbuf, "%d.%dV %d.%dA %d.%dW", voltage_i/1000, voltage_i%1000, current_i/1000, current_i%1000, power_i/1000, power_i%1000);
	ssd1312_showstr(0, 0, strbuf, str_len);
	int min_power = power_chart.min;
	int max_power = power_chart.max;
	str_len = sprintf(strbuf, "max:%d.%d min:%d.%d", max_power/1000, max_power%1000, min_power/1000, min_power%1000);
	ssd1312_showstr(0, 1, strbuf, str_len);


	// power_chart.add_data(power);
	power_chart.draw();

}

void meterUI_pd_info(void) {


	uint8_t status0 = husb238_readReg(HUSB238_PD_STATUS0);
	uint8_t pdo_5v  = husb238_readReg(HUSB238_SRC_PDO_5V);
	uint8_t pdo_9v  = husb238_readReg(HUSB238_SRC_PDO_9V);
	uint8_t pdo_12v = husb238_readReg(HUSB238_SRC_PDO_12V);
	uint8_t pdo_15v = husb238_readReg(HUSB238_SRC_PDO_15V);
	uint8_t pdo_18v = husb238_readReg(HUSB238_SRC_PDO_18V);
	uint8_t pdo_20v = husb238_readReg(HUSB238_SRC_PDO_20V);

	int now_current = PD_SRC_CURRENT[status0&0x0f];
	// int now_voltage = PD_SRC_VOLTAGE[status0>>4];
	str_len = sprintf(strbuf, "NOW_PD:%dV, %d.%dA", PD_SRC_VOLTAGE[status0>>4]/1000, now_current/1000, now_current%1000);
	ssd1312_setFont(font_0507, 5, 7, 1, 2);
	ssd1312_showstr(0, 0, strbuf, str_len);

	ssd1312_setFont(font_0507, 5, 7, 1, 1);

	if (pdo_5v & 0x80) {
		int current = PD_SRC_CURRENT[pdo_5v&0x0f];
		str_len = sprintf(strbuf, "5V:%d.%dA", current/1000, current%1000);
		ssd1312_showstr(0, 2, strbuf, str_len);
	} else {
		ssd1312_showstr(0, 2, "5V:NULL", 7);
	}

	if (pdo_9v & 0x80) {
		int current = PD_SRC_CURRENT[pdo_9v&0x0f];
		str_len = sprintf(strbuf, "9V:%d.%dA", current/1000, current%1000);
		ssd1312_showstr(0, 3, strbuf, str_len);
	} else {
		ssd1312_showstr(0, 3, "9V:NULL", 7);
	}

	if (pdo_12v & 0x80) {
		int current = PD_SRC_CURRENT[pdo_12v&0x0f];
		str_len = sprintf(strbuf, "12V:%d.%dA", current/1000, current%1000);
		ssd1312_showstr(0, 4, strbuf, str_len);
	} else {
		ssd1312_showstr(0, 4, "12V:NULL", 8);
	}

	if (pdo_15v & 0x80) {
		int current = PD_SRC_CURRENT[pdo_15v&0x0f];
		str_len = sprintf(strbuf, "15V:%d.%dA", current/1000, current%1000);
		ssd1312_showstr(0, 5, strbuf, str_len);
	} else {
		ssd1312_showstr(0, 5, "15V:NULL", 8);
	}

	if (pdo_18v & 0x80) {
		int current = PD_SRC_CURRENT[pdo_18v&0x0f];
		str_len = sprintf(strbuf, "18V:%d.%dA", current/1000, current%1000);
		ssd1312_showstr(0, 6, strbuf, str_len);
	} else {
		ssd1312_showstr(0, 6, "18V:NULL", 8);
	}

	if (pdo_20v & 0x80) {
		int current = PD_SRC_CURRENT[pdo_20v&0x0f];
		str_len = sprintf(strbuf, "20V:%d.%dA", current/1000, current%1000);
		ssd1312_showstr(0, 7, strbuf, str_len);
	} else {
		ssd1312_showstr(0, 7, "20V:NULL", 8);
	}

}

void ui_lis2dw12(void) {

	ssd1312_setFont(font0608, 6, 8, 1, 1);

	// LIS2DW12 三轴加速度计ID
	str_len = sprintf(strbuf, "LIS2DW12: %x", lis2dw12_readReg(LIS2DW12_WHO_AM_I));
	ssd1312_showstr(0, 0, strbuf, str_len);


	// LIS2DW12 三轴加速度计数据
	str_len = sprintf(strbuf, "X: %d", lis2dw12_readX());
	ssd1312_showstr(0, 1, strbuf, str_len);
	str_len = sprintf(strbuf, "Y: %d", lis2dw12_readY());
	ssd1312_showstr(0, 2, strbuf, str_len);
	str_len = sprintf(strbuf, "Z: %d", lis2dw12_readZ());
	ssd1312_showstr(0, 3, strbuf, str_len);


	// LIS2DW12 温度传感器（两种分辨率）
	str_len = sprintf(strbuf, "TEMP1: %d", lis2dw12_readTemprature8bit()+25);
	ssd1312_showstr(0, 4, strbuf, str_len);
	int t = 100*(lis2dw12_readTemprature()/16.0+25);
	str_len = sprintf(strbuf, "TEMP2: %d.%d", t/100, t%100);
	// str_len = sprintf(strbuf, "TEMP1: %d", lis2dw12_readTemprature());
	ssd1312_showstr(0, 5, strbuf, str_len);


	// STM32G030 内部温度传感器
	HAL_ADC_Start(&hadc1);	//启动ADC转换
	HAL_ADC_PollForConversion(&hadc1,10);	//等待转换完成，10ms表示超时时间
	uint16_t AD_Value = HAL_ADC_GetValue(&hadc1);	//读取ADC转换数据（12位数据）
	uint16_t TS_CAL1 = *(__IO uint16_t *)(0x1FFF75A8);
	float Temperature = (30.0f) / (TS_CAL1) * ((float) AD_Value - TS_CAL1) + 30.0f;
	int t_i = Temperature*100;
	str_len = sprintf(strbuf, "TEMP3: %d.%d", t_i/100, t_i%100);
	ssd1312_setFont(font_0507, 5, 7, 1, 2);
	ssd1312_showstr(0, 6, strbuf, str_len);
}
// 电源控制
/**
 * @brief HUSB238 IIC 控制电压输出
 * @param voltage 设置所需的电压，单位V，可选 5/9/12/15/18/20
 */
void pd_control (uint8_t voltage) {
	static int husb238_count = 0;
	if (husb238_count > 8) {
		husb238_count = 0;
		float voltage_error = bus_voltage - voltage;
		if (voltage_error < 0) {
			voltage_error = -voltage_error;
		}
		if (voltage_error > 0.75) {
			husb238_set_voltage(voltage);
		}
	} else {
		husb238_count++;
	}
}

//***********加速度计处理*************//
uint8_t lis2dw12_orientation(void) {
	static uint8_t orientation = 1;
	uint8_t rx;
	rx = lis2dw12_readReg(LIS2DW12_OUT_X_H);
	if ((rx&0xf0) == 0x30) {
		orientation = 2;
	} else if ((rx&0xf0) == 0xc0) {
		orientation = 0;
	}
	rx = lis2dw12_readReg(LIS2DW12_OUT_Y_H);
	if ((rx&0xf0) == 0x30) {
		orientation = 3;
	} else if ((rx&0xf0) == 0xc0) {
		orientation = 1;
	}

	return orientation;
}

uint8_t tx = LIS2DW12_CTRL1, rx = 0;

uint8_t scene = 0;

void core(void) {

	// 开启定时器中断
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_Base_Start_IT(&htim17);

	// 初始化INA226
	// 16 次采样取平均值，VBUS和VSHUNT ADC转换时间 2.116ms
	ina226_init(INA226_AVG_16 | INA226_VBUS_2116uS | INA226_VSH_2116uS | INA226_MODE_CONT_SHUNT_AND_BUS, 20, 2500);

	// 初始化oled
	ssd1312_init(0);

	// 等待 lis2dw12 完成上电初始化
	HAL_Delay(20);
	lis2dw12_init();
	// lis2dw12_single_tap_init();
	lis2dw12_double_tap_init();

	// 判断 lis2dw12 能否正常通信
	rx = lis2dw12_readReg(LIS2DW12_WHO_AM_I);
	while (rx != 0x44) {
		rx = lis2dw12_readReg(LIS2DW12_WHO_AM_I);
		HAL_Delay(10);
	}

	husb238_writeReg(HUSB238_GO_COMMAND, HUSB238_COMMAND_GET_SRC_CAP);
	uint8_t voltage_select = 5;

	while (1) {

		// 读取加速度计数据旋转屏幕显示方向
		ssd1312_setRotation(lis2dw12_orientation());
		// 读取加速度计数据，判断是否存在双击事件，存在则清空功率累计
		if (lis2dw12_readReg(LIS2DW12_TAP_SRC) & 0x10) {
			power_sum = 0;
		}

		// 组合键手动切换屏幕显示方向
		if (key_pressed(2) && key_push(1) == KEY_UP) {
			ssd1312_rotation = (ssd1312_rotation + 1) % 4;
			ssd1312_setRotation(ssd1312_rotation);
		}
		// 组合键控制电压诱骗输出
		if (key_pressed(2) && key_push(3) == KEY_UP) {
			voltage_select = (voltage_select + 1) % 6;
		}

		// 根据选择的电压诱骗不同的挡位
		switch (voltage_select) {
			case 0: pd_control(5); break;
			case 1: pd_control(9); break;
			case 2: pd_control(12); break;
			case 3: pd_control(15); break;
			case 4: pd_control(18); break;
			case 5: pd_control(20); break;
			default: break;
		}

		// 清空oled屏幕缓存
		ssd1312_clear();
		// 绘制UI
		switch (scene) {
			case 0: {
				if (key_push(2) == 2) {
					scene = 1;
				}
				meterUI();
			} break;
			case 1: {
				if (key_push(2) == 2) {
					scene = 2;
				}
				meterUI_flowchart();
			} break;
			case 2: {
				if (key_push(2) == 2) {
					scene = 3;
				}
				meterUI_pd_info();
			} break;
			case 3: {
				if (key_push(2) == 2) {
					scene = 0;
				}
				ui_lis2dw12();
			} break;
			default: scene = 0; break;
		}
		// ssd1312_drawLine(0, 0, 127, 32, 0xff);
		// ssd1312_setFont(font_0507, 5, 7, 1, 2);
		// str_len = sprintf(strbuf, "only can be seen at specific situation");
		// ssd1312_showstr(0, 14, strbuf, str_len);

		// 刷新屏幕
		ssd1312_sendBuffer();


		HAL_Delay(33);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM16) {
		// 按键扫描
		key_scan();
	} else if (htim->Instance == TIM17) {
		ina226_sample();
		power_sum += power*0.00005;
		power_bar.calc(power_raw);
		power_chart.add_data(power);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin == INT1_Pin) {
		if (read(INT1) == GPIO_PIN_RESET) {
			// if (lis2dw12_readReg(LIS2DW12_TAP_SRC) & 0x20) {
				power_sum = 0;
			// }
		}
		__HAL_GPIO_EXTI_CLEAR_IT(INT1_Pin);
	}
}

bool my_compare(const char* a, const char* b) {
	for (; *a && *b; a++, b++) {
		if (*a != *b) {
			return false;
		}
	}
	return true;
}



// oled 控制函数
void ssd1312_clr_rst(void) {
	clr(OLED_RST);
}
void ssd1312_set_rst(void) {
	set(OLED_RST);
}
void ssd1312_delay(void) {
	HAL_Delay(100);
}
void ssd1312_write_byte(uint8_t addr, uint8_t data) {
	// uint8_t buffer[2] = {addr, data};
	// HAL_I2C_Master_Transmit(&hi2c1, SSD1312_IIC_ADDRESS, buffer, 2, 1000);
	HAL_I2C_Mem_Write(&hi2c1, SSD1312_IIC_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}
void ssd1312_write_bytes(uint8_t addr, uint8_t* data, int len) {
	// HAL_I2C_Mem_Write(&hi2c1, SSD1312_IIC_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, data, len, 1000);
	HAL_I2C_Mem_Write_DMA(&hi2c1, SSD1312_IIC_ADDRESS, addr, I2C_MEMADD_SIZE_8BIT, data, len);
}


uint8_t ina226_writeReg (uint16_t regAddress, uint16_t regValue) {
	uint8_t retval[2] = {(uint8_t)(regValue >> 8), (uint8_t)(regValue & 0xff)};
	return HAL_I2C_Mem_Write(&hi2c2, INA226_ADDRESS, regAddress, I2C_MEMADD_SIZE_8BIT, retval, 2, 1000);
}
// 无符号读取寄存器
uint16_t ina226_readReg (uint16_t regAddress) {
	uint8_t retval[2] = {0};
	if (HAL_I2C_Mem_Read(&hi2c2, INA226_ADDRESS, regAddress, I2C_MEMADD_SIZE_8BIT, retval, 2, 1000) != HAL_OK) {
		return 0xFFFF;
	} else {
		return retval[0] << 8 | retval[1];
	}
}

// ws2812 控制函数
void ws2812_display(void) {
	// HAL_SPI_Transmit_DMA(&hspi1, buffer, sizeof(colors)*8);
}



void husb238_writeReg(uint8_t regAddress, uint8_t regValue) {
	HAL_I2C_Mem_Write(&hi2c2, HUSB238_ADDR, regAddress, I2C_MEMADD_SIZE_8BIT, &regValue, 1, 1000);
}
uint8_t husb238_readReg(uint8_t regAddress) {
	uint8_t regValue = 0;
	HAL_I2C_Mem_Read(&hi2c2, HUSB238_ADDR, regAddress, I2C_MEMADD_SIZE_8BIT, &regValue, 1, 1000);
	return regValue;
}


void lis2dw12_writeReg(uint8_t reg, uint8_t value) {
	// bit0 = 0, write
	uint8_t data[2] = {reg&0x7f, value};
	clr(CS);
	HAL_SPI_Transmit(&hspi1, data, 2, 1000);
	set(CS);
}


uint8_t lis2dw12_readReg(uint8_t reg) {
	uint8_t data = 0;
	// bit0 = 1, read
	reg |= 0x80;
	clr(CS);
	HAL_SPI_Transmit(&hspi1, &reg, 1, 1000);
	HAL_SPI_Receive(&hspi1, &data, 1, 1000);
	set(CS);
	return data;
}