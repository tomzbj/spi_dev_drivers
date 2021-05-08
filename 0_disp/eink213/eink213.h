#ifndef _EPD2IN13_H
#define _EPD2IN13_H

typedef struct {
    void (*reset_f)(int);
    void (*cs_f)(int);
    void (*rs_f)(int);
    int (*busy_f)(void);
    void (*fastwrite_f)(void*, int);
    void (*delay_ms_f)(volatile int);
    int width, height;
} eink213_cfg_t;

// Display resolution
#define EINK213_WIDTH       122
#define EINK213_HEIGHT      250

#define EINK213_FULL			0
#define EINK213_PART			1

void EINK213_Init(eink213_cfg_t* cfg, int Mode);
void EINK213_Clear(unsigned char data);
void EINK213_Write(void* buf, int count);
void EINK213_Sleep(void);

#endif
