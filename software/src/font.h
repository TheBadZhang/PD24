#ifndef __FONT_H__
#define __FONT_H__


#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

extern const uint8_t num_10x24[];
extern const uint8_t num_6x8[];

// 6*8
extern const uint8_t char_m[];
extern const uint8_t char_s[];
extern const uint8_t char_V[];
extern const uint8_t char_A[];
extern const uint8_t char_W[];
extern const uint8_t char_H[];

extern const unsigned char font_0507 [];
extern const unsigned char font0608 [];


// 8*16
extern const unsigned char font_Fixedsys [];
extern const unsigned char font0816 [];

// 9*16
extern const uint8_t char_V2[];
extern const uint8_t char_A2[];
extern const uint8_t char_W2[];


extern const uint8_t usbmeter1[];
extern const uint8_t usbmeter2[];
extern const uint8_t usbmeter3[];


#ifdef __cplusplus
}
#endif


#endif // __FONT_H__