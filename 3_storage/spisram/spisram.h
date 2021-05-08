#ifndef _SPISRAM_H
#define _SPISRAM_H

typedef struct {
    unsigned char (*writebyte_f)(unsigned char);
    void (*cs_f)(int);
    void (*fastread_f)(void* buf, int nbytes);
    void (*fastwrite_f)(void* buf, int nbytes);
} spisram_cfg_t;

unsigned long SPISRAM_ReadJedecID(void);
void SPISRAM_Init(spisram_cfg_t* cfg);
int SPISRAM_Read(unsigned long addr, int nbytes, void* buf);
int SPISRAM_Write(unsigned long addr, int nbytes, const void* buf);
int SPISRAM_FastRead(unsigned long addr, int num, void* buf);
int SPISRAM_FastWrite(unsigned long addr, int num, void* buf);

#endif
