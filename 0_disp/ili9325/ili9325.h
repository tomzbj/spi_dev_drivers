#ifndef _ILI9325_H
#define _ILI9325_H

typedef struct {
    void (*write_reg_f) (unsigned short reg);
    void (*write_data_f) (unsigned short data);
    void (*fast_write_data_f) (unsigned short *buf, int n);
    void (*fast_fill_mem_f) (unsigned short data, int n);
    void (*reset_f) (void);
    void (*delayms_f) (volatile int nms);
    int height;
    int width;
} ili9325_cfg_t;

void ILI9325_Init(ili9325_cfg_t * cfg);
void ILI9325_Test(void);
void ILI9325_Clear(unsigned short color);
void ILI9325_Putpixel(unsigned short x, unsigned short y, unsigned short color);
void ILI9325_DrawBitmap(unsigned short x, unsigned short y,
                        unsigned short width, unsigned short height,
                        unsigned short *buf);

#endif
