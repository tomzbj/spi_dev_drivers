#include "ili9325.h"
#include "fsmc_lcd.h"
#include "misc.h"

static ili9325_cfg_t g;

#define WriteCmd(x, y) do { g.write_reg_f(x); g.write_data_f(y); } while(0)

static void SetWindow(unsigned short x1, unsigned short y1, unsigned short x2,
                      unsigned short y2);
static void SetXy(unsigned short x, unsigned short y);

void ILI9325_Clear(unsigned short color)
{
    SetXy(0, 0);
    int tot_n = g.width * g.height;
    int n = 65000;
    do {
        if (tot_n < n)
            n = tot_n;
        g.fast_fill_mem_f(color, n);
        tot_n -= n;
    } while (tot_n > 0);
}

void ILI9325_Init(ili9325_cfg_t * cfg)
{
    g = *cfg;
    g.reset_f();

    const struct {
        unsigned short reg, cmd;
    } cmd_list[] = { {
    0x01, 0x0000}, {
    0x02, 0x0700}, {
    0x03, 0x1038}, {
    0x04, 0x0000}, {
    0x08, 0x0207}, {
    0x09, 0x0000}, {
    0x0A, 0x0000}, {
    0x0C, 0x0000}, {
    0x0D, 0x0000}, {
    0x0F, 0x0000}, {
    0x10, 0x0000}, {
    0x11, 0x0007}, {
    0x12, 0x0000}, {
    0x13, 0x0000}, {
    0x10, 0x1290}, {
    0x11, 0x0227}, {
    0x12, 0x001d}, {
    0x13, 0x1500}, {
    0x29, 0x0018}, {
    0x2B, 0x000D}, {
    0x30, 0x0004}, {
    0x31, 0x0307}, {
    0x32, 0x0002}, {
    0x35, 0x0206}, {
    0x36, 0x0408}, {
    0x37, 0x0507}, {
    0x38, 0x0204}, {
    0x39, 0x0707}, {
    0x3C, 0x0405}, {
    0x3D, 0x0F02}, {
    0x50, 0x0000}, {
    0x51, 0x00EF}, {
    0x52, 0x0000}, {
    0x53, 0x013F}, {
    0x60, 0xa700}, {
    0x61, 0x0001}, {
    0x6A, 0x0000}, {
    0x80, 0x0000}, {
    0x81, 0x0000}, {
    0x82, 0x0000}, {
    0x83, 0x0000}, {
    0x84, 0x0000}, {
    0x85, 0x0000}, {
    0x90, 0x0010}, {
    0x92, 0x0600}, {
    0x93, 0x0003}, {
    0x95, 0x0110}, {
    0x97, 0x0000}, {
    0x98, 0x0000}, {
    0x07, 0x0133}};
    for (int i = 0; i < sizeof(cmd_list) / sizeof(cmd_list[0]); i++) {
        WriteCmd(cmd_list[i].reg, cmd_list[i].cmd);
        if (i == 0x11 || i == 0x12 || i == 0x13)
            g.delayms_f(1);
    }
}

static void SetWindow(unsigned short x1, unsigned short y1, unsigned short x2,
                      unsigned short y2)
{
    unsigned short left, right, top, bottom;
    left = min(x1, x2);
    right = max(x1, x2);
    top = min(y1, y2);
    bottom = max(y1, y2);
    WriteCmd(82, left);
    WriteCmd(83, right);
    WriteCmd(80, top);
    WriteCmd(81, bottom);
}

static void SetXy(unsigned short x, unsigned short y)
{
    WriteCmd(33, x);
    WriteCmd(32, y);
    g.write_reg_f(34);
}

void ILI9325_FillRect(unsigned short x1, unsigned short y1, unsigned short x2,
                      unsigned short y2, unsigned short color)
{
    unsigned short left, right, top, bottom;
    int i, num_pixels;

    left = min(x1, x2);
    right = max(x1, x2);
    top = min(y1, y2);
    bottom = max(y1, y2);
    num_pixels = (right - left + 1) * (bottom - top + 1);

    SetWindow(left, top, right, bottom);
    SetXy(left, top);
    for (i = 0; i < num_pixels; i++)
        g.write_data_f(color);
    SetWindow(0, 0, g.width - 1, g.height - 1);
}

void ILI9325_DrawBitmap(unsigned short x, unsigned short y,
                        unsigned short width, unsigned short height,
                        unsigned short *buf)
{
    SetWindow(x, y, x + width - 1, y + height - 1);
    SetXy(x, y);
    int tot_n = width * height;
    int n = 65000;
    do {
        if (tot_n < n)
            n = tot_n;
        g.fast_write_data_f(buf, n);
        buf += n;
        tot_n -= n;
    } while (tot_n > 0);
//    for(int i = 0; i < tot_n; i++) { g.write_data_f(*buf); buf++; }
    SetWindow(0, 0, g.width - 1, g.height - 1);
}

void ILI9325_Rect(unsigned short x1, unsigned short y1, unsigned short x2,
                  unsigned short y2, unsigned short color)
{
    unsigned short i, left, right, top, bottom;
    left = min(x1, x2);
    right = max(x1, x2);
    top = min(y1, y2);
    bottom = max(y1, y2);

    SetXy(left, top);
    for (i = left; i <= right; i++)
        g.write_data_f(color);
    SetXy(left, bottom);
    for (i = left; i <= right; i++)
        g.write_data_f(color);
    SetWindow(left, top, left, bottom);
    SetXy(left, top);
    for (i = top; i <= bottom; i++)
        g.write_data_f(color);
    SetWindow(right, top, right, bottom);
    SetXy(right, top);
    for (i = top; i <= bottom; i++)
        g.write_data_f(color);
    SetWindow(0, 0, g.width - 1, g.height - 1);
}

void ILI9325_Putpixel(unsigned short x, unsigned short y, unsigned short color)
{
    SetXy(x, y);
    g.write_data_f(color);
}
