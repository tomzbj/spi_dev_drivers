#include "dac7512.h"

dac7512_cfg_t g;

void DAC7512_Config(dac7512_cfg_t* cfg)
{
    g = *cfg;
}

void DAC7512_Write(unsigned short data)
{ 
    if(data > 4095)
        data = 4095;
    g.cs_f(0);
    g.writebyte_f(data >> 8);
    g.writebyte_f(data);
    g.cs_f(1);
}
