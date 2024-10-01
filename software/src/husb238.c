#include "husb238.h"

void husb238_set_5v(void) {
	husb238_set_voltage(5);
}

void husb238_set_9v(void) {
	husb238_set_voltage(9);
}

void husb238_set_12v(void) {
	husb238_set_voltage(12);
}

void husb238_set_15v(void) {
	husb238_set_voltage(15);
}

void husb238_set_18v(void) {
	husb238_set_voltage(18);
}

void husb238_set_20v(void) {
	husb238_set_voltage(20);
}


void husb238_set_voltage(uint8_t voltage) {
	uint8_t regValue = 0x00;
	switch (voltage) {
		case 5: regValue  = HUSB238_SRC_PDO_V_5V; break;
		case 9: regValue  = HUSB238_SRC_PDO_V_9V; break;
		case 12: regValue = HUSB238_SRC_PDO_V_12V; break;
		case 15: regValue = HUSB238_SRC_PDO_V_15V; break;
		case 18: regValue = HUSB238_SRC_PDO_V_18V; break;
		case 20: regValue = HUSB238_SRC_PDO_V_20V; break;
		default: regValue = HUSB238_SRC_PDO_V_DEFAULT; break;
	}
	husb238_writeReg(HUSB238_SRC_PDO, regValue);
	husb238_writeReg(HUSB238_GO_COMMAND, HUSB238_COMMAND_SET_SRC_PDO);
}