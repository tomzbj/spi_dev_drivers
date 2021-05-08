#ifndef _AD779X_H
#define _AD779X_H

#include "misc.h"

typedef unsigned char (*writebyte_func_t)(unsigned char);

void AD779X_Init(writebyte_func_t writebyte_f);
void AD779X_Wait(void);
void AD779X_ReadAllRegs(unsigned long* regs);

enum {
    AD779X_MODE_CONTINUOUS_CONV = 0,
    AD779X_MODE_SINGLE_CONV = 1,
    AD779X_MODE_IDLE = 2,
    AD779X_MODE_POWERDOWN = 3,
    AD779X_MODE_INT_ZERO_CALI = 4,
    AD779X_MODE_INT_FULL_CALI = 5,
    AD779X_MODE_SYS_ZERO_CALI = 6,
    AD779X_MODE_SYS_FULL_CALI = 7
};

enum {
    AD779X_CHANNEL_AIN1 = 0x0,
    AD779X_CHANNEL_AIN2 = 0x1,
    AD779X_CHANNEL_AIN3 = 0x2,
    AD779X_CHANNEL_SHORT = 0x3,
    AD779X_CHANNEL_AVDDMON = 0x7
};

void AD779X_SelectMode(unsigned char mode);
void AD779X_Calibrate(unsigned char mode, unsigned char channel);
void AD779X_SelectOneChannel(unsigned char channel);

enum {
    AD779X_POLARITY_UNIPOLAR = 1,
    AD779X_POLARITY_BIPOLAR = 0,
    AD779X_GAIN_1X = 0,
    AD779X_GAIN_2X = 1,
    AD779X_GAIN_4X = 2,
    AD779X_GAIN_8X = 3,
    AD779X_GAIN_16X = 4,
    AD779X_GAIN_32X = 5,
    AD779X_GAIN_64X = 6,
    AD779X_GAIN_128X = 7
};

void AD779X_SetupPolarityGain(unsigned char polarity, unsigned char gain);

enum {
    AD779X_BUFFERED = 1,
    AD779X_UNBUFFERED = 0
};

void AD779X_SetupBuffer(unsigned char buffer);

enum {
    AD779X_DATA_RATE_4_7 = 0xf,
    AD779X_DATA_RATE_10 = 0xc,
    AD779X_DATA_RATE_16_7 = 0xa,
    AD779X_DATA_RATE_50 = 0x5,
    AD779X_DATA_RATE_123 = 0x3,
    AD779X_DATA_RATE_470 = 0x1
};
void AD779X_SetupDataRate(unsigned short data_rate);

enum {
    AD779X_CURRENT_SOURCE_DIR_11_22 = 0,
    AD779X_CURRENT_SOURCE_DIR_12_21 = 1,
    AD779X_CURRENT_SOURCE_DIR_11_21 = 2,
    AD779X_CURRENT_SOURCE_DIR_12_22 = 3,
    AD779X_CURRENT_SOURCE_DISABLED = 0,
    AD779X_CURRENT_SOURCE_ENABLED_10UA = 1,
    AD779X_CURRENT_SOURCE_ENABLED_210UA = 2,
    AD779X_CURRENT_SOURCE_ENABLED_1MA = 3
};

void AD779X_SetupCurrentSource(unsigned char dir, unsigned char en);

enum {
    AD779X_DOUT_ENABLE = 1,
    AD779X_DOUT_DISABLE = 0
};
// for ad7799

void AD779X_SetupDOUT(unsigned char dout_status);

enum {
    AD779X_P2_SET = 1,
    AD779X_P2_RESET = 0
};

enum {
    AD779X_P1_SET = 1,
    AD779X_P1_RESET = 0
};

void AD779X_DOUT(unsigned char p2, unsigned char p1);

unsigned long AD779X_SingleConversion(void);
unsigned long AD779X_ReadData(void);

#endif
