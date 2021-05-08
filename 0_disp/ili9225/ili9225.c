#include "ili9225.h"

static ili9225_cfg_t g;
enum {
    REG_DRIVER_OUTPUT_CTRL = 0x01, REG_LCD_AC_DRIVING_CTRL = 0x02,
    REG_ENTRY_MODE = 0x03, REG_DISP_CTRL1 = 0x07, REG_BLANK_PERIOD_CTRL1 = 0x08,
    REG_FRAME_CYCLE_CTRL = 0x0B, REG_INTERFACE_CTRL = 0x0C, REG_OSC_CTRL = 0x0F,
    REG_POWER_CTRL1 = 0x10, REG_POWER_CTRL2 = 0x11, REG_POWER_CTRL3 = 0x12,
    REG_POWER_CTRL4 = 0x13, REG_POWER_CTRL5 = 0x14, REG_VCI_RECYCLING = 0x15,
    REG_RAM_ADDR_SET1 = 0x20, REG_RAM_ADDR_SET2 = 0x21,
    REG_GRAM_DATA_REG = 0x22, REG_GATE_SCAN_CTRL = 0x30,
    REG_VERTICAL_SCROLL_CTRL1 = 0x31, REG_VERTICAL_SCROLL_CTRL2 = 0x32,
    REG_VERTICAL_SCROLL_CTRL3 = 0x33, REG_PARTIAL_DRIVING_POS1 = 0x34,
    REG_PARTIAL_DRIVING_POS2 = 0x35, REG_HORIZONTAL_WINDOW_ADDR1 = 0x36,
    REG_HORIZONTAL_WINDOW_ADDR2 = 0x37, REG_VERTICAL_WINDOW_ADDR1 = 0x38,
    REG_VERTICAL_WINDOW_ADDR2 = 0x39, REG_GAMMA_CTRL1 = 0x50,
    REG_GAMMA_CTRL2 = 0x51, REG_GAMMA_CTRL3 = 0x52, REG_GAMMA_CTRL4 = 0x53,
    REG_GAMMA_CTRL5 = 0x54, REG_GAMMA_CTRL6 = 0x55, REG_GAMMA_CTRL7 = 0x56,
    REG_GAMMA_CTRL8 = 0x57, REG_GAMMA_CTRL9 = 0x58, REG_GAMMA_CTRL10 = 0x59
};

#define min(x,y) (x) < (y) ? (x) : (y)
#define max(x,y) (x) > (y) ? (x) : (y)

static void WriteCmd(unsigned char cmd)
{
    g.rs_f(0);
    g.cs_f(0);
    g.write_byte_f(cmd);
    g.cs_f(1);
}
static void WriteData(unsigned short data)
{
    g.rs_f(1);
    g.cs_f(0);
//    g.write_byte_f(data >> 8);
//    g.cs_f(1);
//    g.cs_f(0);
    g.write_byte_f(data);       // & 0xff);
    g.cs_f(1);
}

static void WriteReg(unsigned short reg, unsigned short data)
{
    WriteCmd(reg);
    WriteData(data);
}

void ILI9225_Init(ili9225_cfg_t * cfg)
{
    g = *cfg;

    g.reset_f(1);
    g.delayms_f(1);
    g.reset_f(0);
    g.delayms_f(100);
    g.reset_f(1);
    g.delayms_f(50);

    WriteReg(0x02, 0x0100);     // set 1 line inversion
#define USE_HORIZONTAL 1

#if USE_HORIZONTAL              //如果定义了横屏
    //R01H:SM=0,GS=0,SS=0 (for details,See the datasheet of ILI9225)
    WriteReg(0x01, 0x001C);     // set the display line number and display direction
    //R03H:BGR=1,ID0=1,ID1=1,AM=1 (for details,See the datasheet of ILI9225)
    WriteReg(0x03, 0x1038);     // set GRAM write direction .
#else //竖屏
    //R01H:SM=0,GS=0,SS=1 (for details,See the datasheet of ILI9225)
    WriteReg(0x01, 0x011C);     // set the display line number and display direction
    //R03H:BGR=1,ID0=1,ID1=1,AM=0 (for details,See the datasheet of ILI9225)
    WriteReg(0x03, 0x1030);     // set GRAM write direction.
#endif

    WriteReg(0x08, 0x0808);     // set BP and FP
    WriteReg(0x0B, 0x1100);     //frame cycle
    WriteReg(0x0C, 0x0000);     // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
    WriteReg(0x0F, 0x1401);     // Set frame rate----0801
    WriteReg(0x15, 0x0000);     //set system interface
    WriteReg(0x20, 0x0000);     // Set GRAM Address
    WriteReg(0x21, 0x0000);     // Set GRAM Address
    //*************Power On sequence ****************//
    g.delayms_f(50);            // Delay 50ms
    WriteReg(0x10, 0x0800);     // Set SAP,DSTB,STB----0A00
    WriteReg(0x11, 0x1F3F);     // Set APON,PON,AON,VCI1EN,VC----1038
    g.delayms_f(50);            // Delay 50ms
    WriteReg(0x12, 0x0121);     // Internal reference voltage= Vci;----1121
    WriteReg(0x13, 0x006F);     // Set GVDD----0066
    WriteReg(0x14, 0x4349);     // Set VCOMH/VCOML voltage----5F60
    //-------------- Set GRAM area -----------------//
    WriteReg(0x30, 0x0000);
    WriteReg(0x31, 0x00DB);
    WriteReg(0x32, 0x0000);
    WriteReg(0x33, 0x0000);
    WriteReg(0x34, 0x00DB);
    WriteReg(0x35, 0x0000);
    WriteReg(0x36, 0x00AF);
    WriteReg(0x37, 0x0000);
    WriteReg(0x38, 0x00DB);
    WriteReg(0x39, 0x0000);
    // ----------- Adjust the Gamma Curve ----------//
    WriteReg(0x50, 0x0001);     //0400
    WriteReg(0x51, 0x200B);     //060B
    WriteReg(0x52, 0x0000);     //0C0A
    WriteReg(0x53, 0x0404);     //0105
    WriteReg(0x54, 0x0C0C);     //0A0C
    WriteReg(0x55, 0x000C);     //0B06
    WriteReg(0x56, 0x0101);     //0004
    WriteReg(0x57, 0x0400);     //0501
    WriteReg(0x58, 0x1108);     //0E00
    WriteReg(0x59, 0x050C);     //000E
    g.delayms_f(50);            // Delay 50ms
    WriteReg(0x07, 0x1017);
    WriteCmd(0x22);
}

static void SetWindow(int x1, int y1, int x2, int y2)
{
    WriteReg(REG_HORIZONTAL_WINDOW_ADDR1, y2);
    WriteReg(REG_HORIZONTAL_WINDOW_ADDR2, y1);
    WriteReg(REG_VERTICAL_WINDOW_ADDR1, x2);
    WriteReg(REG_VERTICAL_WINDOW_ADDR2, x1);
    WriteReg(REG_RAM_ADDR_SET1, y1);
    WriteReg(REG_RAM_ADDR_SET2, x1);
    WriteCmd(0x22);
}

//static void SetXy(int x, int y) { SetWindow(x, y, g.width - 1, g.height - 1); }
void ILI9225_Clear(unsigned short color)
{
    unsigned short buf[240];
    for (int i = 0; i < 240; i++)
        buf[i] = color;
    SetWindow(0, 0, g.width - 1, g.height - 1);
    g.rs_f(1);
    g.cs_f(0);
//    for(int i = 0; i < g.width * g.height; i++) {
    for (int i = 0; i < g.height; i++) {
        g.fast_write_data_f(buf, 240);
//        WriteData(color);
    }
    g.cs_f(1);
}

void ILI9225_FillRect(int x1, int y1, int x2, int y2, unsigned short color)
{
    int left = min(x1, x2);
    int right = max(x1, x2);
    int top = min(y1, y2);
    int bottom = max(y1, y2);
    int num_pixels = (right - left + 1) * (bottom - top + 1);

    SetWindow(left, top, right, bottom);
    for (int i = 0; i < num_pixels; i++)
        g.write_byte_f(color);
    SetWindow(0, 0, g.width - 1, g.height - 1);
}

void ILI9225_DrawBitmap(int x, int y, int width, int height,
                        unsigned short *buf)
{
    SetWindow(x, y, x + width - 1, y + height - 1);
    for (int i = 0; i < width * height; i++) {
        g.write_byte_f(*buf);
        buf++;
    }
}

//void ILI9225_PutPixel(int x, int y, unsigned short color) { SetXy(x, y);         //设置光标位置 g.write_word_f(color); }
