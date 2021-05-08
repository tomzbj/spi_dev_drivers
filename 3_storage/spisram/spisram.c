#include "spisram.h"
#include <stdlib.h>

typedef enum {
    CMD_READ = 0x03,
    CMD_FAST_READ = 0x0b,
    CMD_BYTE_PROGRAM = 0x02,
    CMD_JEDEC_ID = 0x9f
} spisram_cmd_t;

#define ReadByte() g.writebyte_f(0xff)

static spisram_cfg_t g;

static void FastWrite(void* buf, int num)
{
    while(num > 0) {
        g.writebyte_f(*(unsigned char*) buf);
        buf++;
        num--;
    }
}

void SPISRAM_Init(spisram_cfg_t* cfg)
{
    g = *cfg;
    if(g.fastwrite_f == NULL)
        g.fastwrite_f = FastWrite;
    g.cs_f(0);
    g.writebyte_f(0x66);
    g.cs_f(1);
    g.cs_f(0);
    g.writebyte_f(0x99);
    g.cs_f(1);
}

unsigned long SPISRAM_ReadJedecID(void)
{
    unsigned long ret;

    g.cs_f(0);
    g.writebyte_f(CMD_JEDEC_ID);
    g.writebyte_f(0xff);
    g.writebyte_f(0xff);
    g.writebyte_f(0xff);
    ret = ReadByte();
    ret <<= 8;
    ret |= ReadByte();
    ret <<= 8;
    ret |= ReadByte();
    ret <<= 8;
    ret |= ReadByte();

    g.cs_f(1);
    return ret;
}

int SPISRAM_FastWrite(unsigned long addr, int num, void* buf)
{
    g.cs_f(0);
    g.writebyte_f(CMD_BYTE_PROGRAM);
    g.writebyte_f(addr >> 16);
    g.writebyte_f(addr >> 8);
    g.writebyte_f(addr);

    g.fastwrite_f(buf, num);
    g.cs_f(1);

    return 0;
}

int SPISRAM_FastRead(unsigned long addr, int num, void* buf)
{
    g.cs_f(0);
    g.writebyte_f(CMD_FAST_READ);
    g.writebyte_f(addr >> 16);
    g.writebyte_f(addr >> 8);
    g.writebyte_f(addr);
    g.writebyte_f(0xff);
    g.fastread_f(buf, num);
    g.cs_f(1);
    return 0;
}

int SPISRAM_Read(unsigned long addr, int num, void* buf)
{
    g.cs_f(0);
    g.writebyte_f(CMD_FAST_READ);
    g.writebyte_f(addr >> 16);
    g.writebyte_f(addr >> 8);
    g.writebyte_f(addr);
    g.writebyte_f(0xff);

    for(int i = 0; i < num; i++) {
        *(unsigned char*) buf = ReadByte();
        buf++;
    }
    g.cs_f(1);
    return 0;
}

int SPISRAM_Write(unsigned long addr, int num, const void* buf)
{
    while(num > 0) {
        g.cs_f(0);
        g.writebyte_f(0x02);
        g.writebyte_f(addr >> 16);
        g.writebyte_f(addr >> 8);
        g.writebyte_f(addr);
        do {
            g.writebyte_f(*(unsigned char*) buf);
            addr++;
            buf++;
            num--;
        } while((addr & 0xff) && (num > 0));
        g.cs_f(1);
    }
    return 0;
}
