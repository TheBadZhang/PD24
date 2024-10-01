#ifndef __TZB_LIS2DW12_H__
#define __TZB_LIS2DW12_H__

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif


#define LIS2DW12_OUT_T_L     0x0D   // 温度数据寄存器（低8位）
#define LIS2DW12_OUT_T_H     0x0E   // 温度数据寄存器（高8位）

#define LIS2DW12_WHO_AM_I    0x0F   // WHO_AM_I 寄存器

#define LIS2DW12_CTRL1       0x20   // 控制寄存器1
#define LIS2DW12_CTRL2       0x21   // 控制寄存器2
#define LIS2DW12_CTRL3       0x22   // 控制寄存器3
#define LIS2DW12_CTRL4       0x23   // 控制寄存器4（INT1_PAD_CTRL）
#define LIS2DW12_CTRL5       0x24   // 控制寄存器5（INT2_PAD_CTRL）
#define LIS2DW12_CTRL6       0x25   // 控制寄存器6

#define LIS2DW12_OUT_T       0x26   // 温度数据寄存器

#define LIS2DW12_STATUS      0x27   // 状态寄存器

#define LIS2DW12_OUT_X_L     0x28   // X轴数据寄存器（低8位）
#define LIS2DW12_OUT_X_H     0x29   // X轴数据寄存器（高8位）
#define LIS2DW12_OUT_Y_L     0x2A   // Y轴数据寄存器（低8位）
#define LIS2DW12_OUT_Y_H     0x2B   // Y轴数据寄存器（高8位）
#define LIS2DW12_OUT_Z_L     0x2C   // Z轴数据寄存器（低8位）
#define LIS2DW12_OUT_Z_H     0x2D   // Z轴数据寄存器（高8位）

#define LIS2DW12_FIFO_CTRL   0x2E   // FIFO控制寄存器
#define LIS2DW12_FIFO_SMPL   0x2F   // FIFO采样控制寄存器

#define LIS2DW12_TAP_THS_X   0x30   // X轴点击阈值寄存器
#define LIS2DW12_TAP_THS_Y   0x31   // Y轴点击阈值寄存器
#define LIS2DW12_TAP_THS_Z   0x32   // Z轴点击阈值寄存器

#define LIS2DW12_INT_DUR     0x33   // 中断持续时间寄存器

#define LIS2DW12_WAKE_UP_THS 0x34   // 唤醒阈值寄存器
#define LIS2DW12_WAKE_UP_DUR 0x35   // 唤醒持续时间寄存器
#define LIS2DW12_FREE_FALL   0x36   // 自由落体阈值寄存器
#define LIS2DW12_STATUS_DUP  0x37   // 事件状态检测寄存器
// [7] [6]  [5] [4] [3] [2] [1]   [0]
// OVR 温度 睡眠 双击 单击 朝向 跌落 数据ready
#define LIS2DW12_WAKE_UP_SRC 0x38   // 唤醒事件源寄存器
// [7:6] [5] [4]  [3]  [2]  [1]  [0]
// 保留   跌落 睡眠 唤醒 X唤醒 Y唤醒 Z唤醒
#define LIS2DW12_TAP_SRC     0x39   // 点击源寄存器
// [7]  [6] [5] [4]   [3]   [2]   [1]  [0]
// 保留 点击 单击 双击 点击方向 X点击 Y点击 Z点击
#define LIS2DW12_SIXD_SRC    0x3A   // 6D事件源寄存器
#define LIS2DW12_ALL_INT_SRC 0x3B   // 所有事件中断寄存器
#define LIS2DW12_X_OFS_USR   0x3C   // X轴用户偏移寄存器
#define LIS2DW12_Y_OFS_USR   0x3D   // Y轴用户偏移寄存器
#define LIS2DW12_Z_OFS_USR   0x3E   // Z轴用户偏移寄存器
#define LIS2DW12_CTRL7       0x3F   // 控制寄存器7



void lis2dw12_init(void);
int16_t lis2dw12_readTemprature(void);
int8_t lis2dw12_readTemprature8bit(void);

int16_t lis2dw12_readX(void);
int16_t lis2dw12_readY(void);
int16_t lis2dw12_readZ(void);

void lis2dw12_single_tap_init(void);
void lis2dw12_double_tap_init(void);


uint8_t lis2dw12_single_tap_event_status(void);


// 识别方向
uint8_t lis2dw12_6d_detection(void);

uint8_t lis2dw12_readReg(uint8_t reg);
void lis2dw12_writeReg(uint8_t reg, uint8_t value);


#ifdef __cplusplus
}
#endif

#endif // __TZB_LIS2DW12_H__