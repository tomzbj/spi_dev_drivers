#include "ad779x.h"

enum {
    REG_STAT = 0x00, REG_MODE = 0x01, REG_CONF = 0x2, REG_DATA = 0x3,
    REG_ID = 0x4, REG_IO = 0x5, REG_OFFSET = 0x6, REG_FULLSCALE = 0x7
};
//unsigned char (*WriteByte)(unsigned char);
//#define WriteByte(x) SPI2_WriteByte(x)
//#define WriteByte(x) AD779X_WriteByte(x)
writebyte_func_t WriteByte;

#define delay2() _delay_us(100UL)
#define ReadByte() WriteByte(0xff)
#define WriteWord(data) do {WriteByte(data >> 8); WriteByte(data);}while(0)
#define WriteDword(data) do {WriteByte(data >> 16); WriteByte(data >> 8); WriteByte(data);}while(0)

static unsigned long ReadDword(void)
{
    unsigned long data = 0;
    data = ReadByte();
    data <<= 8;
    data |= ReadByte();
    data <<= 8;
    data |= ReadByte();

    return data;
}

static unsigned short ReadWord(void)
{
    unsigned short data;
    data = ReadByte();
    data <<= 8;
    data |= ReadByte();
    delay2();
    return data;
}

static unsigned long ReadReg(unsigned char reg)
{
    reg &= 0x7;
    _delay_us(100);
    WriteByte(0x40 | (reg << 3));
    _delay_us(100);

    switch(reg) {
        case REG_STAT:
            case REG_ID:
            case REG_IO:
            return ReadByte();
        case REG_MODE:
            case REG_CONF:
            return ReadWord();
        case REG_DATA:
            case REG_OFFSET:
            case REG_FULLSCALE:
            return ReadDword();
    }
    return 0;
}

// Read all 8 registers.
void AD779X_ReadAllRegs(unsigned long* regs)
{
    regs[0] = ReadReg(REG_STAT);
    regs[1] = ReadReg(REG_MODE);
    regs[2] = ReadReg(REG_CONF);
    regs[3] = ReadReg(REG_DATA);
    regs[4] = ReadReg(REG_ID);
    regs[5] = ReadReg(REG_IO);
    regs[6] = ReadReg(REG_OFFSET);
    regs[7] = ReadReg(REG_FULLSCALE);
}

// Write data to given register.
static void WriteReg(unsigned char reg, unsigned long data)
{
    reg &= 0x7;
    _delay_us(100);

    WriteByte(0x00 | (reg << 3));
    _delay_us(100);

    switch(reg) {
        case REG_MODE:
            case REG_CONF:
            WriteWord(data);
            break;
        case REG_OFFSET:
            case REG_FULLSCALE:
            WriteDword(data);
            break;
        case REG_IO:
            WriteByte(data);
            break;
    }
    _delay_us(100);
}

// mode=4: Internal Zero-scale Calibration. 5: Internal Full-scale Calibration.
void AD779X_SelectMode(unsigned char mode)
{
    unsigned short reg;

    mode &= 0x7;
    reg = ReadReg(REG_MODE);
    reg &= 0x1fff;            // clear bits MR15~MR13 (MD2~MD0)
    reg |= ((unsigned short)mode << 13);
    WriteReg(REG_MODE, reg);
    AD779X_Wait();
}

// Performs a calibration to the specified channel, wait included.

void AD779X_Calibrate(unsigned char mode, unsigned char channel)
{
    unsigned short reg;
    AD779X_SelectOneChannel(channel);

    reg = ReadReg(REG_MODE);
    reg &= 0x1fff;            // clear bits MR15~MR13 (MD2~MD0)
    reg |= ((unsigned long)mode << 13);
    WriteReg(REG_MODE, reg);
    AD779X_Wait();
}

void AD779X_Init(writebyte_func_t writebyte_f)
{
    WriteByte = writebyte_f;
    for(int i = 0; i < 6; i++)
        WriteByte(0xff);
}

// Read Data
unsigned long AD779X_ReadData(void)
{
    return ReadReg(REG_DATA);
}

// Channel = 0,1,2 for Channel 1-3; 3 for short; 7 for AVDD monitor
void AD779X_SelectOneChannel(unsigned char channel)
{
    unsigned short reg;

    channel &= 0x7;

    reg = ReadReg(REG_CONF);
    reg &= 0xfff8;            // Clears bits CON2~CON0 (CH2~CH0)
    reg |= channel;
    WriteReg(REG_CONF, reg);
}

void AD779X_SetupPolarityGain(unsigned char polarity, unsigned char gain)
{
    unsigned short reg;

    polarity &= 0x1;
    gain &= 0x7;

    reg = ReadReg(REG_CONF);
    reg &= 0xe8ff;            // Clears bits CON12, CON10~CON8 (U/B, G2~G0)
    reg |= ((unsigned short)polarity << 12) | ((unsigned short)gain << 8);
    WriteReg(REG_CONF, reg);
}

void AD779X_SetupBias(unsigned char bias)
{
    unsigned short reg = ReadReg(REG_CONF);
    reg &= 0x3fff;
    reg |= (unsigned short)bias << 14;
    WriteReg(REG_CONF, reg);
}

void AD779X_SetupRef(unsigned char ref)
{
    unsigned short reg = ReadReg(REG_CONF);
    if(ref)
        reg |= (1 << 7);
    else
        reg &= ~(1 << 7);
    WriteReg(REG_CONF, reg);
}

// Setup DataRate

void AD779X_SetupDataRate(unsigned short data_rate)
{
    unsigned short reg;

    data_rate &= 0xf;

    reg = ReadReg(REG_MODE);
    reg &= 0xfff0;
    reg |= data_rate;
    WriteReg(REG_MODE, reg);
}

// Buffer = 1: Buffer on; 0: Buffer off

void AD779X_SetupBuffer(unsigned char buffer)
{
    unsigned short reg;

    buffer &= 0x1;

    reg = ReadReg(REG_CONF);
    reg &= 0xffef;            // Clears bits CON4
    reg |= ((unsigned short)buffer << 4);
    WriteReg(REG_CONF, reg);
}

void AD779X_SetupCurrentSource(unsigned char dir, unsigned char en)
{
    unsigned short reg;

    dir &= 0x3;
    en &= 0x3;

    reg = (dir << 2) | en;
    WriteReg(REG_IO, reg);
}

void AD779X_SetupDOUT(unsigned char dout_status)
{
    unsigned char reg;
    dout_status &= 0x1;
    reg = ReadReg(REG_IO);
    reg &= 0xbf;        // clear bit IOEN
    reg |= (dout_status << 6);
    WriteReg(REG_IO, reg);
}

void AD779X_DOUT(unsigned char p2, unsigned char p1)
{
    unsigned char reg;
    p2 &= 0x1;
    p1 &= 0x1;
    reg = ReadReg(REG_IO);
    reg &= 0xc0;        // clear bits IO2DAT & IO1DAT
    reg |= ((p2 << 5) | (p1 << 4));
    WriteReg(REG_IO, reg);
}

// Wait for DRY bit

void AD779X_Wait(void)
{
    unsigned char reg;
    do {
        reg = ReadReg(REG_STAT);
    } while(reg & 0x80);
}

// Performs a single converstion

unsigned long AD779X_SingleConversion(void)
{
    unsigned short reg;
    reg = (1UL << 13) | 0xa;   // Single Conv Mode, 16.7Hz
    WriteReg(REG_MODE, reg);
    AD779X_Wait();
    reg = ReadReg(REG_DATA);

    return reg;
}
