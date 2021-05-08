#include "eink213.h"

static eink213_cfg_t g;

const unsigned char EINK213_lut_full_update[] = {
    //
    0x22, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00    //
    };

const unsigned char EINK213_lut_partial_update[] = {
    //
    0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00    //
    };

static void reset(void)
{
    g.reset_f(1);
    g.delay_ms_f(200);
    g.reset_f(0);
    g.delay_ms_f(2);
    g.reset_f(1);
    g.delay_ms_f(200);
}

static void write_cmd(unsigned char Reg)
{
    g.rs_f(0);
    g.cs_f(0);
    g.fastwrite_f(&Reg, 1);
    g.cs_f(1);
}

static void write_data(unsigned char Data)
{
    g.rs_f(1);
    g.cs_f(0);
    g.fastwrite_f(&Data, 1);
    g.cs_f(1);
}

void wait_busy(void)
{
    int count = 20;
    xprintf("e-Paper busy\r\n");
    while(g.busy_f() && (--count > 0)) {    //LOW: idle, HIGH: busy
        xprintf("%d\n", count);
        g.delay_ms_f(1);
    }
    xprintf("e-Paper busy release\r\n");
}

static void display_on(void)
{
    write_cmd(0x22);    // DISPLAY_UPDATE_CONTROL_2
    write_data(0xC4);
    write_cmd(0X20);    // MASTER_ACTIVATION
    write_cmd(0xFF);    // TERMINATE_FRAME_READ_WRITE
    wait_busy();
}

static void set_window(int x_start, int y_start, int x_end, int y_end)
{
    write_cmd(0x44);

    write_data((x_start >> 3) & 0xFF);
    write_data((x_end >> 3) & 0xFF);
    write_cmd(0x45);
    write_data(y_start & 0xFF);
    write_data((y_start >> 8) & 0xFF);
    write_data(y_end & 0xFF);
    write_data((y_end >> 8) & 0xFF);
}

static void set_cursor(int x, int y)
{
    write_cmd(0x4E);

    write_data((x >> 3) & 0xFF);
    write_cmd(0x4F);
    write_data(y & 0xFF);
    write_data((y >> 8) & 0xFF);
    //    EINK213_ReadBusy();
}

void EINK213_Init(eink213_cfg_t* cfg, int Mode)
{
    g = *cfg;

    reset();

    write_cmd(0x01);    // DRIVER_OUTPUT_CONTROL

    write_data((g.height - 1) & 0xFF);
    write_data(((g.height - 1) >> 8) & 0xFF);

    write_data(0x00);			// GD = 0; SM = 0; TB = 0;
    write_cmd(0x0C);    // BOOSTER_SOFT_START_CONTROL
    write_data(0xD7);

    write_data(0xD6);
    write_data(0x9D);
    write_cmd(0x2C);    // WRITE_VCOM_REGISTER

    write_data(0xA8);     // VCOM 7C
    write_cmd(0x3A);    // SET_DUMMY_LINE_PERIOD
    write_data(0x1A);			// 4 dummy lines per gate

    write_cmd(0x3B);    // SET_GATE_TIME
    write_data(0x08);			// 2us per line
    write_cmd(0X3C);    // BORDER_WAVEFORM_CONTROL

    write_data(0x03);
    write_cmd(0X11);    // DATA_ENTRY_MODE_SETTING
    write_data(0x03);			// X increment; Y increment

    //set the look-up table register
    write_cmd(0x32);
    if(Mode == EINK213_FULL) {
        for(int i = 0; i < 30; i++)
            write_data(EINK213_lut_full_update[i]);
    }

    else if(Mode == EINK213_PART) {
        for(int i = 0; i < 30; i++)
            write_data(EINK213_lut_partial_update[i]);
    }
    else
        xprintf("error, the Mode is EINK213_FULL or EINK213_PART");

}

void EINK213_Clear(unsigned char data)
{
    int width, height;
    width = (g.width % 8 == 0) ? (g.width / 8) : (g.width / 8 + 1);
    height = g.height;

    set_window(0, 0, g.width, g.height);
    for(int j = 0; j < height; j++) {
        set_cursor(0, j);
        write_cmd(0x24);
        for(int i = 0; i < width; i++)
            write_data(data);
    }
    display_on();
}

void EINK213_Write(void* buf, int count)
{
    int width, height;
    width = (g.width % 8 == 0) ? (g.width / 8) : (g.width / 8 + 1);
    height = g.height;
    unsigned char* pbuf = buf;

    set_window(0, 0, g.width, g.height);
    for(int j = 0; j < height; j++) {
        set_cursor(0, j);
        write_cmd(0x24);
        for(int i = 0; i < width; i++)
            write_data(*pbuf++);
//            write_data(pbuf[i + j * width]);
    }
    display_on();
}

void EINK213_Sleep(void)
{
    write_cmd(0x10);    //DEEP_SLEEP_MODE
    write_data(0x01);
}
