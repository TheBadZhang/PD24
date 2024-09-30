#ifndef __TBZ_TRICK_H_
#define __TBZ_TRICK_H_
// 特殊语法

/**
 * @brief 逻辑与
 */
#define AND(a,b) ((a)&&(b))
/**
 * @brief 逻辑或
 */
#define OR(a,b) ((a)||(b))
/**
 * @brief 逻辑非
 */
#define NOT(x) (!(x))
/**
 * @brief 将一个int的数据赋值给高低两个字节
 * @param high 高字节
 * @param low 低字节
 * @param value 16-bit 的数据
 */
#define ASSIGNMENT16(high, low, value) \
	(high) = (unsigned char)((value) >> 8);\
	(low)  = (unsigned char)((value) & 0xff);
/**
 * @brief 将高低两个字节的数据合成为两个字节的int
 * @param high 高字节
 * @param low 低字节
 */
#define BIND2BYTE(high, low)\
	((high) << 8) | (low)
#endif
/**
 * @brief 置位某整型变量第 y 位
 * @param x 变量
 * @param y 第 y 位
 */
#define sbi(x, y)  ((x) |= (1 << (y)))
/**
 * @brief 清零某整型变量第 y 位
 * @param x 变量
 * @param y 第 y 位
 */
#define cbi(x, y)  ((x) &= ~(1 << (y)))

#ifdef SDCC_8051
#define HIGH 1
#define LOW  0
/**
 * @brief 设置一个引脚为高电位
 * @param v 要设置的引脚
 */
#define set(v) v = HIGH
/**
 * @brief 设置一个引脚为低电位
 * @param v 要设置的引脚
 */
#define clr(v) v = LOW

#define INT0 0
#define TMR0 1
#define INT1 2
#define TMR1 3
#define TMR2 5



/**
 * @brief 简写的 for 循环
 * @param i 要被循环的变量
 * @param x 循环次数 [0,x)
 */
#define FOR0(i,s,e) for (i = s; i < e; ++i)
#define FOR(i,s,e) for (i = s; i <= e; ++i)
#define ROF0(i,s,e) for(i = s; i > e; --i)
#define ROF(i,s,e) for(i = s; i >= e; --i)
#define DO  {
#define END }


/**
 * @brief 什么也不做的一条语句
 */
#define _nop_() __asm nop __endasm
#elif defined(USE_HAL_DRIVER)

/**
 * @brief 简写的 for 循环
 * @param i 要被循环的变量
 * @param x 循环次数 [0,x)
 */
#define FOR0(i,s,e) for(int i = s; i < e; ++i)
#define FOR(i,s,e) for(int i = s; i <= e; ++i)
#define ROF0(i,s,e) for(int i = s; i > e; --i)
#define ROF(i,s,e) for(int i = s; i >= e; --i)
#define DO  {
#define END }


// GPIO 操作

#define HIGH GPIO_PIN_SET
#define LOW GPIO_PIN_RESET
#define Pin(name) GPIO_Port(name)##,##Pin(name)
#define GPIO_Port(name) name##_GPIO_Port
#define GPIO_Pin(name) name##_Pin
#define set(name) HAL_GPIO_WritePin(GPIO_Port(name), GPIO_Pin(name), HIGH)
#define clr(name) HAL_GPIO_WritePin(GPIO_Port(name), GPIO_Pin(name), LOW)
#define flip(name) HAL_GPIO_TogglePin(GPIO_Port(name), GPIO_Pin(name))
#define read(name) HAL_GPIO_ReadPin(GPIO_Port(name), GPIO_Pin(name))

/**
 * @brief 位带操作，实现51类似的GPIO控制功能
 * 具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
 * 部分新的stm32取消了位带的设计，可能是觉得这个也比较鸡肋吧
 */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入


#endif


