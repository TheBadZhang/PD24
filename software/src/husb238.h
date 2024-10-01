#ifndef __TBZ_HUSB238_H__
#define __TBZ_HUSB238_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HUSB238_ADDR 0x10

#define HUSB238_PD_STATUS0  0x00
// [7:4]  [3:0]
//  电压   电流
// 分别取出后用下面两个数组进行查表
#define HUSB238_PD_STATUS1  0x01
// [7]                            [6]               [5:3]               [2]       [1:0]
// CC方向                         ATTACH             PD状态           5V连接状态    5V电流状态
// 0=CC1连接到CC line或者未连接   HUSB238未连接状态    000=未反应          0=其他状态   00=默认电流
// 1=CC2连接到CC line          HUSB238非未连接状态   001=成功            1=5V        01=1.5A
//                                                010=非法的命令或者参数            10=3A
//                                                011=不支持的命令                 11=5A
//                                                100=传输错误
//                                                其他=保留

#define HUSB238_SRC_PDO_5V  0x02
#define HUSB238_SRC_PDO_9V  0x03
#define HUSB238_SRC_PDO_12V 0x04
#define HUSB238_SRC_PDO_15V 0x05
#define HUSB238_SRC_PDO_18V 0x06
#define HUSB238_SRC_PDO_20V 0x07
// [7]              [6:4]      [3:0]
// 1=detected       reserved   见下面的数组
// 0=not detected   reserved
// 下面的数组存储着上面六个寄存器返回值[3:0]读数对应的电流值，单位mA，可以直接通过下标进行读取
const uint16_t PD_SRC_CURRENT[] = {
	500, 700, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000, 3250, 3500, 4000, 4500, 5000
};
// 读取的电压值，单位mV
const uint16_t PD_SRC_VOLTAGE[] = {
	0/*意味着没有连接*/, 5000, 9000, 12000, 15000, 18000, 20000
};


// 此寄存器可用于手动设置PD诱骗电压
#define HUSB238_SRC_PDO     0x08
// [7:4]                              [3:0]
// 0000=未选择（此时根据电路状态进行输出）   保留
// 0001=5V
// 0010=9V
// 0011=12V
// 1000=15V
// 1001=18V
// 1010=20V
#define HUSB238_GO_COMMAND  0x09
// [7:5]  [4:0]
// 保留    0x01=通过SRC_PDO请求电压
//        0x04=发送 Get_SRC_Cap 命令
//        0x05=发送硬件复位命令
//        其他=保留

#define HUSB238_COMMAND_SET_SRC_PDO 0x01
#define HUSB238_COMMAND_GET_SRC_CAP 0x04

#define HUSB238_SRC_PDO_V_DEFAULT 0x00
#define HUSB238_SRC_PDO_V_5V 0x10
#define HUSB238_SRC_PDO_V_9V 0x20
#define HUSB238_SRC_PDO_V_12V 0x30
#define HUSB238_SRC_PDO_V_15V 0x80
#define HUSB238_SRC_PDO_V_18V 0x90
#define HUSB238_SRC_PDO_V_20V 0xA0




void husb238_init(void);
void husb238_set_5v(void);
void husb238_set_9v(void);
void husb238_set_12v(void);
void husb238_set_15v(void);
void husb238_set_18v(void);
void husb238_set_20v(void);
void husb238_set_voltage(uint8_t voltage);


void husb238_writeReg(uint8_t regAddress, uint8_t regValue);
uint8_t husb238_readReg(uint8_t regAddress);

#ifdef __cplusplus
}
#endif

#endif // __TBZ_HUSB238_H__