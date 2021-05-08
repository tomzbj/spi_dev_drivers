#include "ad719X.h"

static struct {
    unsigned char (*readwrite_f)(unsigned char);
} g;

enum {
    REG_STAT = 0x00,
    REG_MODE = 0x01,
    REG_CONF = 0x02,
    REG_DATA = 0x03,
    REG_ID = 0x04,
    REG_GPOCON = 0x05,
    REG_OFFSET = 0x06,
    REG_FULLSCALE = 0x07
};

static void WriteDword(unsigned long data)
{
    g.readwrite_f(data >> 16);
    g.readwrite_f(data >> 8);
    g.readwrite_f(data);
}

// Read 24bits from AD719X.
static unsigned long ReadDword(void)
{
    unsigned long data = 0;
    data = g.readwrite_f(0xff);
    data <<= 8;
    data |= g.readwrite_f(0xff);
    data <<= 8;
    data |= g.readwrite_f(0xff);

    return data;
}

// Read 8 bits or 24bits from given register.
static unsigned long ReadReg(unsigned char reg)
{
    reg &= 0x7;
    g.readwrite_f(0x40 | (reg << 3));

    switch(reg) {
        case REG_STAT:
        case REG_ID:
        case REG_GPOCON:
            return g.readwrite_f(0xff);
        case REG_MODE:
        case REG_CONF:
        case REG_DATA:
        case REG_OFFSET:
        case REG_FULLSCALE:
            return ReadDword();
    }
    return 0;
}

// Read all 8 registers.
void AD719X_ReadAllRegs(unsigned long* regs)
{
    regs[0] = ReadReg(REG_STAT);
    regs[1] = ReadReg(REG_MODE);
    regs[2] = ReadReg(REG_CONF);
    regs[3] = ReadReg(REG_DATA);
    regs[4] = ReadReg(REG_ID);
    regs[5] = ReadReg(REG_GPOCON);
    regs[6] = ReadReg(REG_OFFSET);
    regs[7] = ReadReg(REG_FULLSCALE);
}

// Write data to given register.
static void WriteReg(unsigned char reg, unsigned long data)
{
    reg &= 0x7;
    g.readwrite_f(0x00 | (reg << 3));

    switch(reg) {
        case REG_MODE:
        case REG_CONF:
        case REG_OFFSET:
        case REG_FULLSCALE:
            WriteDword(data);
            break;
        case REG_GPOCON:
            g.readwrite_f(data);
            break;
    }
//    _delay_us(100);
}

// mode = 4: Internal Zero-scale Calibration. 5: Internal Full-scale Calibration.
void AD719X_SelectMode(unsigned char mode)
{
    unsigned long reg;

    reg = ReadReg(REG_MODE);
    reg &= 0x1fffff;            // clear bits MR23~MR21 (MD2~MD0)
    reg |= ((unsigned long)mode << 21);
    WriteReg(REG_MODE, reg);
	AD719X_Wait();
}

// Performs a calibration to the specified channel, wait included.
void AD719X_Calibrate(unsigned char mode, unsigned char channel)
{
    unsigned long reg;
    AD719X_SelectOneChannel(channel);

    reg = ReadReg(REG_MODE);
    reg &= 0x1fffff;            // clear bits MR23~MR21 (MD2~MD0)
    reg |= ((unsigned long)mode << 21);
    WriteReg(REG_MODE, reg);
	AD719X_Wait();
}

// todo
void AD719X_Init(unsigned char (*readwrite_f)(unsigned char))
{
    g.readwrite_f = readwrite_f;
    for(int i = 0; i < 8; i++) {
        g.readwrite_f(0xff);
    }
}

unsigned long AD719X_ReadData(void)
{
    unsigned long data;
    data = ReadReg(REG_DATA);
    data <<= 8;
    data |= g.readwrite_f(0xff);
    return data;
}

// For AD7193:
// Channel = 0..9
// When Pseudo = 1: 0 ~ 7 for AIN0 ~ 7 & AINCOM (Single Ended)
// When Pseudo = 0: 0 ~ 3 for AIN0 / 1  ~  AIN6 / 7, 4 ~ 7 the same
// 8 for Temperature Sensor, 9 for Short.
// For AD7190:
// 0 ~ 1 for AIN0/1, AIN2/3; 2 for Temperature Sensor, 3 for Short.
// 4 ~ 7 for AIN4 ~ AIN7 & AINCOM (Single Ended)
void AD719X_SelectOneChannel(unsigned short channel)
{
    unsigned long reg;

    reg = ReadReg(REG_CONF);
#if (DEVICE == AD7193)
	reg &= 0xfc00ff;            // Clears bits CON17~CON8
#elif (DEVICE == AD7190)
    reg &= 0xff00ff;            // Clears bits CON15~CON8
#endif
    reg |= (1UL << (channel));
    WriteReg(REG_CONF, reg);
}

// Polarity = 0 for Bipolar operation, 1 for Unipolar operation
// Gain = 0, 3 ~ 7 for Gain 1x, 8x ~ 128x
void AD719X_SetupPolarityGain(unsigned char polarity, unsigned char gain)
{
    unsigned long reg;

    polarity &= 0x1;
    gain &= 0x7;

    reg = ReadReg(REG_CONF);
    reg &= 0xfffff0;            // Clears bits CON3~CON0
    reg |= (polarity << 3) | gain;
    WriteReg(REG_CONF, reg);
}

// 1: Chop Enabled; 0: Chop Disabled
void AD719X_SetupChop(unsigned char chop)
{
    unsigned long reg;

    chop &= 0x1;

    reg = ReadReg(REG_CONF);
    reg &= 0x7fffff;            // Clears bits CON23
    reg |= (unsigned long)chop << 23;
    WriteReg(REG_CONF, reg);
}

// 0: REF1+/REF1-;  1: REF2+/REF2-
void AD719X_SelectReference(unsigned char ref)
{
    unsigned long reg;

    ref &= 0x1;

    reg = ReadReg(REG_CONF);
    reg &= ~(1UL << 20);            // Clears bits CON20
    reg |= (unsigned long)ref << 20;
    WriteReg(REG_CONF, reg);
}

#if (DEVICE == AD7193)
// 0: 4 Differential Channels;  1: 8 Single Ended Channels
void AD719X_SetupPseudo(unsigned char pseudo) {
	unsigned long reg;

	pseudo &= 0x1;

	reg = ReadReg(REG_CONF);
	reg &= ~(1UL << 18);            // Clears bits CON18
	reg |= (unsigned long) pseudo << 18;
	WriteReg(REG_CONF, reg);
}
#endif

// Select Multiple Channels. Use bit mask to select AIN1 to AIN8, TEMP, SHORT.
void AD719X_SelectChannels(unsigned short channels)
{
    unsigned long reg;

    channels &= 0x3ff;

    reg = ReadReg(REG_CONF);
    reg &= 0xfc00ff;                // Clears bits CON17~CON8
    reg |= (unsigned long)channels << 8;
    WriteReg(REG_CONF, reg);
}

// 1: REG_STAT will be transfered with REG_DATA. Will be useful when multiple
// channels selected.
void AD719X_SetupDatSta(unsigned char dat_sta)
{
    unsigned long reg;

    dat_sta &= 0x1;

    reg = ReadReg(REG_MODE);
    reg &= ~(1UL << 20);
    reg |= ((unsigned long)dat_sta << 20);
    WriteReg(REG_MODE, reg);
}

// 0 for Ext. XTAL; 1 for Ext. CLK; 2 for 4.92MHz Int. CLK;
// 3 for Ext. CLK, and can get CLK from MCLK2 pin.
void AD719X_SelectClockSource(unsigned char clock_source)
{
    unsigned long reg;

    clock_source &= 0x3;
    reg = ReadReg(REG_MODE);
    reg &= ~(3UL << 18);
    reg |= ((unsigned long)clock_source << 18);
    WriteReg(REG_MODE, reg);
	AD719X_Wait();
}

#if (DEVICE == AD7193)
// 0: Fast settling disabled; 1: Fast settling averaged based 2
// 2: Based 8; 3: Based 16
void AD719X_SetupFastSettling(unsigned char fast_settling) {
	unsigned long reg;

	fast_settling &= 0x3;

	reg = ReadReg(REG_MODE);
	reg &= ~(3UL << 16);
	reg |= ((unsigned long) fast_settling << 16);
	WriteReg(REG_MODE, reg);
}
#endif

void AD719X_SetupDataRate(unsigned short data_rate)
{
    unsigned long reg;

    data_rate &= 0x3ff;

    reg = ReadReg(REG_MODE);
    reg &= 0xfffc00;
    reg |= data_rate;
    WriteReg(REG_MODE, reg);
}

// Wait for DRY bit
void AD719X_Wait(void)
{
    unsigned char reg;
    do {
        reg = ReadReg(REG_STAT);
    } while(reg & 0x80);

}

// Performs a single converstion
unsigned long AD719X_SingleConversion(void)
{
    unsigned long reg;
    reg = (1UL << 21) | (2UL << 18) | 96;    // Single Conv Mode, Int. CLK, 50Hz
    WriteReg(REG_MODE, reg);
    AD719X_Wait();
    reg = ReadReg(REG_DATA);

    return reg;
}

int AD719X_DRDY(void)
{
    unsigned char reg = ReadReg(REG_STAT);
    if(reg & 0x80)
        return 0;
    else
        return 1;
}
