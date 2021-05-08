#include "ad7124.h"

static struct {
    ad7124_cfg_t cfg;
} g;

#define REG_COMM        0x00
#define REG_STATUS      0x00
#define REG_ADC_CTRL    0x01
#define REG_DATA        0x02
#define REG_IO_CTRL1    0x03
#define REG_IO_CTRL2    0x04
#define REG_ID          0x05
#define REG_ERR         0x06
#define REG_ERREN       0x07
#define REG_MCLK_CNT    0x08
#define REG_CHAN(x)     (0x09 + (x))    // x=0..15
#define REG_CFG(x)      (0x19 + (x))    // x=0..7
#define REG_FILT(x)     (0x21 + (x))    // x=0..7
#define REG_OFS(x)      (0x29 + (x))    // x=0..7
#define REG_GAIN(x)     (0x31 + (x))    // x=0..7
#define AIN_MASK 0xfc00
#define DATA_RATE_MASK 0xf800

#define bitval(x)       (1UL << (x))

void ad7124_reset(void)
{
    for(int i = 0; i < 10; i++)
        g.cfg.readwrite_f(0xff);
}

void ad7124_init(ad7124_cfg_t* cb)
{
    g.cfg = *cb;
    ad7124_reset();
}

static int get_reg_size(unsigned char reg)
{
    if(reg >= 0x21 || reg == 0x2 || reg == 0x3 || reg == 0x6 || reg == 0x7)
        return 3;
    else if(reg >= 0x9 || reg == 0x1 || reg == 0x4)
        return 2;
    else if(reg == 0 || reg == 5 || reg == 8)
        return 1;   // reg 0, 5, 8
    else
        return -1;
}

static unsigned long read_reg(unsigned char reg)
{
    unsigned long ret = 0;
    reg &= 0x3f;
    g.cfg.readwrite_f(0x40 | reg);
    switch(get_reg_size(reg)) {
        case 3:
            ret = g.cfg.readwrite_f(0xff);
        case 2:
            ret <<= 8;
            ret |= g.cfg.readwrite_f(0xff);
        case 1:
            ret <<= 8;
            ret |= g.cfg.readwrite_f(0xff);
    }
    return ret;
}

unsigned long ad7124_read_one_reg(unsigned char reg)
{
    return read_reg(reg);
}

// Write data to given register.
static void write_reg(unsigned char reg, unsigned long data)
{
    reg &= 0x3f;
    g.cfg.readwrite_f(0x00 | reg);
    switch(get_reg_size(reg)) {
        case 3:
            g.cfg.readwrite_f(data >> 16);
        case 2:
            g.cfg.readwrite_f(data >> 8);
        case 1:
            g.cfg.readwrite_f(data);
    }
}

void ad7124_set_data_status(int data_status)
{
    unsigned short reg = read_reg(REG_ADC_CTRL);
    if(data_status)
        reg |= bitval(10);
    else
        reg &= ~bitval(10);
    write_reg(REG_ADC_CTRL, reg);
}

void ad7124_set_iref(int iref_status)
{
    unsigned short reg = read_reg(REG_ADC_CTRL);
    if(iref_status)
        reg |= bitval(8);
    else
        reg &= ~bitval(8);
    write_reg(REG_ADC_CTRL, reg);
}

void ad7124_set_pwr_mode(int mode)
{
    unsigned short reg = read_reg(REG_ADC_CTRL);
    reg &= ~(bitval(7) | bitval(6));
    switch(mode) {
        case AD7124_PWR_MODE_LOW:
            break;
        case AD7124_PWR_MODE_MID:
            reg |= bitval(6);
            break;
        case AD7124_PWR_MODE_FULL:
            reg |= bitval(7);
            break;
    }
    write_reg(REG_ADC_CTRL, reg);
}

void ad7124_set_mode(int mode)
{
    if(mode >= AD7124_MODE_CONTINUOUS_CONV
        && mode <= AD7124_MODE_SYS_FULL_CALI) {
        unsigned short reg = read_reg(REG_ADC_CTRL);
        reg &= ~(bitval(5) | bitval(4) | bitval(3) | bitval(2));
        reg |= (mode << 2);
        write_reg(REG_ADC_CTRL, reg);
    }
}

unsigned long ad7124_read_data(void)
{
    unsigned short reg = read_reg(REG_ADC_CTRL);
    unsigned long data = read_reg(REG_DATA);
    if(reg & bitval(10)) { // DATA_STATUS_EN
        data <<= 8;
        data |= g.cfg.readwrite_f(0xff);
    }
    return data;
}

void ad7124_gpio_set_state(int gpio, int state)
{
    if(gpio >= AD7124_GPIO_1 && gpio <= AD7124_GPIO_4) {
        unsigned long reg = read_reg(REG_IO_CTRL1);
        if(state)
            reg |= bitval(gpio + 16);
        else
            reg &= ~bitval(gpio + 16);
        write_reg(REG_IO_CTRL1, reg);
    }
}

void ad7124_gpio_bit_write(int gpio, int val)
{
    if(gpio >= AD7124_GPIO_1 && gpio <= AD7124_GPIO_4) {
        unsigned long reg = read_reg(REG_IO_CTRL1);
        if(val)
            reg |= bitval(gpio + 20);
        else
            reg &= ~bitval(gpio + 20);
        write_reg(REG_IO_CTRL1, reg);
    }
}

void ad7124_iout_set(int iout_id, int iout_curr)
{
    if(iout_curr >= AD7124_IOUT_OFF && iout_curr <= AD7124_IOUT_1000UA) {
        unsigned long reg = read_reg(REG_IO_CTRL1);
        switch(iout_id) {
            case AD7124_IOUT0:
                reg &= ~(bitval(8) | bitval(9) | bitval(10));
                reg |= bitval(iout_curr + 8);
                break;
            case AD7124_IOUT1:
                reg &= ~(bitval(11) | bitval(12) | bitval(13));
                reg |= bitval(iout_curr + 11);
                break;
        }
        write_reg(REG_IO_CTRL1, reg);
    }
}

void ad7124_iout_ain_set(int iout_id, int ain)
{
    if(ain >= AD7124_AIN0 && ain <= AD7124_AIN15) {
        unsigned long reg = read_reg(REG_IO_CTRL1);
        switch(iout_id) {
            case AD7124_IOUT0:
                reg &= ~(bitval(0) | bitval(1) | bitval(2) | bitval(3));
                reg |= bitval(ain);
                break;
            case AD7124_IOUT1:
                reg &= ~(bitval(4) | bitval(5) | bitval(6) | bitval(7));
                reg |= bitval(ain + 4);
                break;
        }
        write_reg(REG_IO_CTRL1, reg);
    }
}

void ad7124_vbias_set(int ain, int status)
{
    if(ain >= AD7124_AIN0 && ain <= AD7124_AIN15) {
        unsigned long reg = read_reg(REG_IO_CTRL2);
        if(status)
            reg |= bitval(ain);
        else
            reg &= ~bitval(ain);
        write_reg(REG_IO_CTRL2, reg);
    }
}

unsigned char ad7124_read_id(void)
{
    return read_reg(REG_ID);
}

unsigned long ad7124_read_err(void)
{
    return read_reg(REG_ERR);
}

unsigned char ad7124_get_mclk_cnt(void)
{
    return read_reg(REG_MCLK_CNT);
}

void ad7124_set_channel_state(int channel, int state)
{
    if(channel >= AD7124_CHANNEL0 && channel <= AD7124_CHANNEL15) {
        unsigned short reg = read_reg(REG_CHAN(channel));
        if(state)
            reg |= bitval(15);
        else
            reg &= ~bitval(15);
        write_reg(REG_CHAN(channel), reg);
    }
}

void ad7124_set_channel_config(int channel, int config)
{
    if(channel >= AD7124_CHANNEL0 && channel <= AD7124_CHANNEL15) {
        unsigned short reg = read_reg(REG_CHAN(channel));
        reg &= ~(bitval(14) | bitval(13) | bitval(12));
        reg |= (config << 12);
        write_reg(REG_CHAN(channel), reg);
    }
}

#define AIN_MASK 0xfc00

void ad7124_set_channel_ain(int channel, int ain_p, int ain_n)
{
    if(channel >= AD7124_CHANNEL0 && channel <= AD7124_CHANNEL15) {
        unsigned short reg = read_reg(REG_CHAN(channel));
        reg &= AIN_MASK;
        reg |= (ain_p << 5);
        reg |= ain_n;
        write_reg(REG_CHAN(channel), reg);
    }
}

void ad7124_set_config_polarity(int config_id, int state)
{
    if(config_id >= AD7124_CONFIG0 && config_id <= AD7124_CONFIG7) {
        unsigned short reg = read_reg(REG_CFG(config_id));
        if(state)
            reg |= bitval(11);
        else
            reg &= ~bitval(11);
        write_reg(REG_CFG(config_id), reg);
    }
}

void ad7124_set_config_burnout(int config_id, int burnout)
{
    if(config_id >= AD7124_CONFIG0 && config_id <= AD7124_CONFIG7) {
        unsigned short reg = read_reg(REG_CFG(config_id));
        reg &= ~(bitval(10) | bitval(9));
        reg |= (burnout << 9);
        write_reg(REG_CFG(config_id), reg);
    }
}

void ad7124_set_config_buffer(int config_id, int ref_buf_p, int ref_buf_n,
    int ain_buf_p, int ain_buf_n)
{
    if(config_id >= AD7124_CONFIG0 && config_id <= AD7124_CONFIG7) {
        unsigned short reg = read_reg(REG_CFG(config_id));
        if(ref_buf_p)
            reg |= bitval(8);
        else
            reg &= ~bitval(8);
        if(ref_buf_n)
            reg |= bitval(7);
        else
            reg &= ~bitval(7);
        if(ain_buf_p)
            reg |= bitval(6);
        else
            reg &= ~bitval(6);
        if(ain_buf_n)
            reg |= bitval(5);
        else
            reg &= ~bitval(5);
        write_reg(REG_CFG(config_id), reg);
    }
}

void ad7124_set_config_ref(int config_id, int ref)
{
    if(config_id >= AD7124_CONFIG0 && config_id <= AD7124_CONFIG7) {
        unsigned short reg = read_reg(REG_CFG(config_id));
        reg &= ~(bitval(4) | bitval(3));
        reg |= (ref << 3);
        write_reg(REG_CFG(config_id), reg);
    }
}

void ad7124_set_config_gain(int config_id, int gain)
{
    if(config_id >= AD7124_CONFIG0 && config_id <= AD7124_CONFIG7) {
        unsigned short reg = read_reg(REG_CFG(config_id));
        reg &= ~(bitval(2) | bitval(1) | bitval(0));
        reg |= gain;
        write_reg(REG_CFG(config_id), reg);
    }
}

void ad7124_set_filter_type(int filter_id, int filter_type)
{
    if(filter_id >= AD7124_FILTER0 && filter_id <= AD7124_FILTER7) {
        unsigned long reg = read_reg(REG_FILT(filter_id));
        reg &= ~(bitval(23) | bitval(22) | bitval(21));
        reg |= (filter_type << 21);
        write_reg(REG_FILT(filter_id), reg);
    }
}

void ad7124_set_filter_rej60(int filter_id, int state)
{
    if(filter_id >= AD7124_FILTER0 && filter_id <= AD7124_FILTER7) {
        unsigned long reg = read_reg(REG_FILT(filter_id));
        if(state)
            reg |= bitval(20);
        else
            reg &= ~bitval(20);
        write_reg(REG_FILT(filter_id), reg);
    }
}

void ad7124_set_filter_post_filter(int filter_id, int post_filter_type)
{
    if(filter_id >= AD7124_FILTER0 && filter_id <= AD7124_FILTER7) {
        unsigned long reg = read_reg(REG_FILT(filter_id));
        reg &= ~(bitval(19) | bitval(18) | bitval(17));
        reg |= (post_filter_type << 17);
        write_reg(REG_FILT(filter_id), reg);
    }
}

void ad7124_set_filter_data_rate(int filter_id, int data_rate)
{
    if(data_rate >= 10 && data_rate <= 19200) {
        data_rate = 19200 / data_rate;
        if(filter_id >= AD7124_FILTER0 && filter_id <= AD7124_FILTER7) {
            unsigned long reg = read_reg(REG_FILT(filter_id));
            reg &= DATA_RATE_MASK;
            reg |= data_rate;
            write_reg(REG_FILT(filter_id), reg);
        }
    }
}

unsigned char ad7124_read_status(void)
{
    return read_reg(REG_STATUS);
}
