#ifndef _DAC8562_H
#define _DAC8562_H

typedef struct {
    unsigned char (*readwrite_f)(unsigned char);
    void (*cs_f)(int);
    void (*delay_f)(volatile int);
} dac8562_cfg_t;

void DAC8562_Init(dac8562_cfg_t* cfg);
void DAC8562_SetChannel1(unsigned short data);
void DAC8562_SetChannel2(unsigned short data);

#endif
