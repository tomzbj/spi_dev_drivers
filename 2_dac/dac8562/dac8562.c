#include "dac8562.h"

static dac8562_cfg_t g;

static void WriteData(unsigned long val)
{
    g.cs_f(0);
    g.delay_f(1);
    g.readwrite_f(val >> 16);
    g.readwrite_f(val >> 8);
    g.readwrite_f(val);

    g.delay_f(1);
    g.cs_f(1);
}

void DAC8562_Init(dac8562_cfg_t* cfg)
{
    g = *cfg;
    WriteData(0x280001); //    DAC_WR_REG(CMD_RESET_ALL_REG, DATA_RESET_ALL_REG);      // reset
    WriteData(0x200003); //    DAC_WR_REG(CMD_PWR_UP_A_B, DATA_PWR_UP_A_B);        // power up
    WriteData(0x380000); //    DAC_WR_REG(CMD_INTERNAL_REF_EN, DATA_INTERNAL_REF_EN);      // enable internal reference
    WriteData(0x020003); //    DAC_WR_REG(CMD_GAIN, DATA_GAIN_B2_A2);            // set multiplier
    WriteData(0x300003); //    DAC_WR_REG(CMD_LDAC_DIS, DATA_LDAC_DIS);          // update the cache
}

void DAC8562_SetChannel1(unsigned short data)
{
    WriteData(0x180000UL | data);
}

void DAC8562_SetChannel2(unsigned short data)
{
    WriteData(0x190000UL | data);
}
