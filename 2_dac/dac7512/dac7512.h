#ifndef _DAC7512_H
#define _DAC7512_H

typedef struct {
    void (*writebyte_f)(unsigned char);
    void (*cs_f)(int);
} dac7512_cfg_t;

void DAC7512_Config(dac7512_cfg_t* cfg);
void DAC7512_Write(unsigned short data);

#endif
