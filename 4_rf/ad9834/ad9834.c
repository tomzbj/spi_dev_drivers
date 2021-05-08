#include "ad9834.h"

static ad9834_cfg_t g;
static unsigned short reg_ctrl = 0x2000;

static void writeword(unsigned short data)
{
    g.cs_f(0);
    g.writebyte_f(data >> 8);
    g.writebyte_f(data);
    g.cs_f(1);
}

void AD9834_Init(ad9834_cfg_t* cfg)
{
    g = *cfg;
    reg_ctrl |= 0x0100;
    writeword(reg_ctrl);
    reg_ctrl &= ~0x0100;
    writeword(reg_ctrl);
}

void AD9834_SetFreq(ad9834_freqreg_t reg, unsigned long freq)
{
    unsigned long val = (unsigned long long)freq * (1ULL << 28) / g.mclk;
    unsigned long valh = (val >> 14) & 0x3fff;
    unsigned long vall = val & 0x3fff;
    if(reg == AD9834_FREQ0) {
        writeword(0x4000 | vall);
        writeword(0x4000 | valh);
    }
    else {
        writeword(0x8000 | vall);
        writeword(0x8000 | valh);
    }
}

void AD9834_UpdateFreq(ad9834_freqreg_t reg)
{
    if(reg == AD9834_FREQ1)
        reg_ctrl |= 0x0800;
    else
        reg_ctrl &= ~0x0800;
    writeword(reg_ctrl);
//    AD9834_Reset();
}
