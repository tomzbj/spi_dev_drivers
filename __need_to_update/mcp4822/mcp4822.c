#include "mcp4822.h"
#include "misc.h"

#define delay() _delay_us(100)
#define SET_CS   do { sbi(MCP4822_PORT, MCP4822_CS);    delay(); } while(0)
#define SET_SDI  do { cbi(MCP4822_PORT, MCP4822_SDI);   delay(); } while(0)
#define SET_SCK  do { cbi(MCP4822_PORT, MCP4822_SCK);   delay(); } while(0)
#define CLR_CS   do { cbi(MCP4822_PORT, MCP4822_CS);    delay(); } while(0)
#define CLR_SDI  do { sbi(MCP4822_PORT, MCP4822_SDI);   delay(); } while(0)
#define CLR_SCK  do { sbi(MCP4822_PORT, MCP4822_SCK);   delay(); } while(0)

static void WriteWord(u16 dword);
extern void MCP4822_WriteDacA(u16 data)
{
    data = (data & 0xfff) | 0x7000; // DACA, GAIN 1X, DAC ON
    WriteWord(data);
}

extern void MCP4822_WriteDacB(u16 data)
{
    data = (data & 0xfff) | 0xd000; // DACB, GAIN 1X, DAC ON
    WriteWord(data);
}

// MCP4822 output = data * vref / 65536, vref = 2.048V
static void WriteWord(u16 data)
{
    u8 i; 
    SET_CS;
    CLR_CS;
    for (i = 0; i < 16; i++) {
        CLR_SCK;
        if (data & 0x8000)
            SET_SDI;
        else
            CLR_SDI;
        SET_SCK;
        data <<= 1;
    }
    SET_CS;
}
