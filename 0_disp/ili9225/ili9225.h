#ifndef _ILI9225_H
#define _ILI9225_H

typedef struct {
    void (*write_byte_f) (unsigned short data);
    void (*fast_write_data_f) (void *buf, int n);
    void (*reset_f) (int state);
    void (*rs_f) (int state);
    void (*cs_f) (int state);
    void (*delayms_f) (volatile int nms);
    int height;
    int width;
} ili9225_cfg_t;

void ILI9225_Init(ili9225_cfg_t * cfg);
void ILI9225_Clear(unsigned short color);
void ILI9225_PutPixel(int x, int y, unsigned short color);
void ILI9225_FillRect(int x1, int y1, int x2, int y2, unsigned short color);
void ILI9225_DrawBitmap(int x, int y, int width, int height,
                        unsigned short *buf);

#endif
