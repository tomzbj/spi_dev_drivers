#ifndef _AD9834_H
#define _AD9834_H

typedef struct {
    void (*writebyte_f)(unsigned char data);
    void (*cs_f)(int state);
    unsigned long mclk;
} ad9834_cfg_t;

typedef enum {
    AD9834_FREQ0, AD9834_FREQ1
} ad9834_freqreg_t;

void AD9834_Init(ad9834_cfg_t* cfg);
void AD9834_SetFreq(ad9834_freqreg_t reg, unsigned long freq);
void AD9834_UpdateFreq(ad9834_freqreg_t reg);

#endif
