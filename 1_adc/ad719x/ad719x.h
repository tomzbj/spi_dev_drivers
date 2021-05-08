#ifndef _AD719X_H
#define _AD719X_H

#define AD7190 7190
#define AD7193 7193
#define DEVICE AD7190

void            AD719X_Init(unsigned char (*readwrite_f)(unsigned char));
void            AD719X_Wait(void);
void            AD719X_ReadAllRegs(unsigned long* regs);

enum {
 AD719X_MODE_CONTINUOUS_CONV=0,
 AD719X_MODE_SINGLE_CONV    =1,
 AD719X_MODE_IDLE           =2,
 AD719X_MODE_POWERDOWN      =3,
 AD719X_MODE_INT_ZERO_CALI  =4,
 AD719X_MODE_INT_FULL_CALI  =5,
 AD719X_MODE_SYS_ZERO_CALI  =6,
 AD719X_MODE_SYS_FULL_CALI  =7
};

#if (DEVICE == AD7193)
enum {
     AD719X_CHANNEL_AIN1AIN2    =0x1,
     AD719X_CHANNEL_AIN3AIN4    =0x2,
     AD719X_CHANNEL_AIN5AIN6    =0x4,
     AD719X_CHANNEL_AIN7AIN8    =0x8,
     AD719X_CHANNEL_AIN1        =0x1,
     AD719X_CHANNEL_AIN2        =0x2,
     AD719X_CHANNEL_AIN3        =0x4,
     AD719X_CHANNEL_AIN4        =0x8,
     AD719X_CHANNEL_AIN5        =0x10,
     AD719X_CHANNEL_AIN6        =0x20,
     AD719X_CHANNEL_AIN7        =0x40,
     AD719X_CHANNEL_AIN8        =0x80,
     AD719X_CHANNEL_TEMPERATURE =0x100,
     AD719X_CHANNEL_SHORT       =0x200,
     AD719X_CHANNEL_ALLDIFF     =0xf,
     AD719X_CHANNEL_ALLSINGLE   =0xff
};

#elif (DEVICE == AD7190)
enum {
    AD719X_CHANNEL_AIN1AIN2    = 0x1,
    AD719X_CHANNEL_AIN3AIN4    = 0x2,
    AD719X_CHANNEL_AIN1        = 0x10,
    AD719X_CHANNEL_AIN2        = 0x20,
    AD719X_CHANNEL_AIN3        = 0x40,
    AD719X_CHANNEL_AIN4        = 0x80,
    AD719X_CHANNEL_TEMPERATURE = 0x4,
    AD719X_CHANNEL_SHORT       = 0x8,
    AD719X_CHANNEL_ALLDIFF     = 0x3,
    AD719X_CHANNEL_ALLSINGLE   = 0xf0
};

#endif
void            AD719X_SelectMode(unsigned char mode);
void            AD719X_Calibrate(unsigned char mode, unsigned char channel);
void            AD719X_SelectOneChannel(unsigned short channel);
void            AD719X_SelectChannels(unsigned short channels);

enum {
AD719X_POLARITY_UNIPOLAR    =1,
AD719X_POLARITY_BIPOLAR    = 0,
AD719X_GAIN_1X             = 0,
AD719X_GAIN_8X             = 3,
AD719X_GAIN_16X            = 4,
AD719X_GAIN_32X            = 5,
AD719X_GAIN_64X            = 6,
AD719X_GAIN_128X           = 7,
};
void            AD719X_SetupPolarityGain(unsigned char polarity, 
        unsigned char gain);

#define AD719X_CHOP_DISABLED        0
#define AD719X_CHOP_ENABLED         1
void            AD719X_SetupChop(unsigned char chop);

#define AD719X_REF_REFIN1           0
#define AD719X_REF_REFIN2           1
void            AD719X_SelectReference(unsigned char ref);

#if (DEVICE == AD7193)
enum {
AD719X_PSEUDO_8_CHANNELS    =1, AD719X_PSEUDO_4_CHANNELS    =0
}
void            AD719X_SetupPseudo(unsigned char pseudo);
#endif

#define AD719X_DATSTA_ENABLED       1
#define AD719X_DATSTA_DISABLED      0
void            AD719X_SetupDatSta(unsigned char dat_sta);

#define AD719X_CLKSRC_EXT_XTAL      0
#define AD719X_CLKSRC_EXT_CLK       1
#define AD719X_CLKSRC_INT_CLK       2
#define AD719X_CLKSRC_INT_CLK_OUTPUT 3
void            AD719X_SelectClockSource(unsigned char clock_source);

#if (DEVICE == AD7193)
    enum  {
    AD719X_FASTSETTLING_OFF    = 0,
    AD719X_FASTSETTLING_AVERAGED_BY_2  = 1,
    AD719X_FASTSETTLING_AVERAGED_BY_8  = 2,
    AD719X_FASTSETTLING_AVERAGED_BY_16 = 3
    };
    void            AD719X_SetupFastSettling(unsigned char fast_settling);
#endif

enum {
 AD719X_DATA_RATE_4_7     =   1023,
 AD719X_DATA_RATE_7_5     =   640,
 AD719X_DATA_RATE_10      =   480,
 AD719X_DATA_RATE_12_5    =   384,
 AD719X_DATA_RATE_16_7    =   288,
 AD719X_DATA_RATE_25      =   192,
 AD719X_DATA_RATE_50      =   96,
 AD719X_DATA_RATE_60      =   80,
 AD719X_DATA_RATE_150     =   32,
 AD719X_DATA_RATE_300     =   16,
 AD719X_DATA_RATE_400     =   12,
 AD719X_DATA_RATE_480     =   10,
 AD719X_DATA_RATE_600     =   8,
 AD719X_DATA_RATE_800     =   6,
 AD719X_DATA_RATE_960     =   5,
 AD719X_DATA_RATE_1200    =   4,
 AD719X_DATA_RATE_1600    =   3,
 AD719X_DATA_RATE_2400    =   2,
 AD719X_DATA_RATE_4800    =   1
};
void            AD719X_SetupDataRate(unsigned short data_rate);

unsigned long   AD719X_SingleConversion(void);
unsigned long   AD719X_ReadData(void);
int AD719X_DRDY(void);

#endif 
