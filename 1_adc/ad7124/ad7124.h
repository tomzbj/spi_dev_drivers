#ifndef _AD7124_H
#define _AD7124_H

#include "misc.h"

typedef struct {
    unsigned char (*readwrite_f)(unsigned char data);
} ad7124_cfg_t;

enum {
    AD7124_DATA_STATUS_OFF = 0, AD7124_DATA_STATUS_ON = 1
};
enum {
    AD7124_PWR_MODE_LOW = 0, AD7124_PWR_MODE_MID = 1, AD7124_PWR_MODE_FULL = 2
};
enum {
    AD7124_MODE_CONTINUOUS_CONV = 0, AD7124_MODE_SINGLE_CONV = 1,
    AD7124_MODE_STANDBY = 2, AD7124_MODE_PWR_DOWN = 3, AD7124_MODE_IDLE = 4,
    AD7124_MODE_INT_ZERO_CALI = 5, AD7124_MODE_INT_FULL_CALI = 6,
    AD7124_MODE_SYS_ZERO_CALI = 7, AD7124_MODE_SYS_FULL_CALI = 8
};
enum {
    AD7124_GPIO_1 = 0, AD7124_GPIO_2 = 1, AD7124_GPIO_3 = 2, AD7124_GPIO_4 = 3
};
enum {
    AD7124_IOUT_OFF = 0, AD7124_IOUT_50UA = 1, AD7124_IOUT_100UA = 2,
    AD7124_IOUT_250UA = 3, AD7124_IOUT_500UA = 4, AD7124_IOUT_750UA = 5,
    AD7124_IOUT_1000UA = 6
};
enum {
    AD7124_IOUT0 = 0, AD7124_IOUT1 = 1
};
enum {
    // normal ains
    AD7124_AIN0 = 0, AD7124_AIN1 = 1, AD7124_AIN2 = 2, AD7124_AIN3 = 3,
    AD7124_AIN4 = 4, AD7124_AIN5 = 5, AD7124_AIN6 = 6, AD7124_AIN7 = 7,
    AD7124_AIN8 = 8, AD7124_AIN9 = 9, AD7124_AIN10 = 10, AD7124_AIN11 = 11,
    AD7124_AIN12 = 12, AD7124_AIN13 = 13, AD7124_AIN14 = 14, AD7124_AIN15 = 15,
    // special ains
    AD7124_AIN_TEMP = 16, AD7124_AIN_AVSS = 17, AD7124_AIN_IREF = 18,
    AD7124_AIN_DGND = 19, AD7124_AIN_AVDD_AVSS_DIV6_P = 20,
    AD7124_AIN_AVDD_AVSS_DIV6_N = 21, AD7124_AIN_IOVDD_DGND_DIV6_P = 22,
    AD7124_AIN_IOVDD_DGND_DIV6_N = 23, AD7124_AIN_ALDO_AVSS_DIV6_P = 24,
    AD7124_AIN_ALDO_AVSS_DIV6_N = 25, AD7124_AIN_DLDO_AVSS_DIV6_P = 26,
    AD7124_AIN_DLDO_AVSS_DIV6_N = 27, AD7124_AIN_V20MV_P = 28,
    AD7124_AIN_V20MV_N = 29
};
enum {
    AD7124_DISABLE = 0, AD7124_ENABLE = 1
};
enum {
    AD7124_CHANNEL0 = 0, AD7124_CHANNEL1 = 1, AD7124_CHANNEL2 = 2,
    AD7124_CHANNEL3 = 3, AD7124_CHANNEL4 = 4, AD7124_CHANNEL5 = 5,
    AD7124_CHANNEL6 = 6, AD7124_CHANNEL7 = 7, AD7124_CHANNEL8 = 8,
    AD7124_CHANNEL9 = 9, AD7124_CHANNEL10 = 10, AD7124_CHANNEL11 = 11,
    AD7124_CHANNEL12 = 12, AD7124_CHANNEL13 = 13, AD7124_CHANNEL14 = 14,
    AD7124_CHANNEL15 = 15
};
enum {
    AD7124_CONFIG0 = 0, AD7124_CONFIG1 = 1, AD7124_CONFIG2 = 2,
    AD7124_CONFIG3 = 3, AD7124_CONFIG4 = 4, AD7124_CONFIG5 = 5,
    AD7124_CONFIG6 = 6, AD7124_CONFIG7 = 7
};
enum {
    AD7124_BURNOUT_CURR_OFF = 0, AD7124_BURNOUT_CURR_0_5UA = 1,
    AD7124_BURNOUT_CURR_2UA = 2, AD7124_BURNOUT_CURR_4UA = 3
};
enum {
    AD7124_REF_REFIN1 = 0, AD7124_REF_REFIN2 = 1, AD7124_REF_IREF = 2,
    AD7124_REF_AVDD = 3
};
enum {
    AD7124_GAIN_1X = 0, AD7124_GAIN_2X = 1, AD7124_GAIN_4X = 2,
    AD7124_GAIN_8X = 3, AD7124_GAIN_16X = 4, AD7124_GAIN_32X = 5,
    AD7124_GAIN_64X = 6, AD7124_GAIN_128X = 7
};
enum {
    AD7124_FILTER0 = 0, AD7124_FILTER1 = 1, AD7124_FILTER2 = 2,
    AD7124_FILTER3 = 3, AD7124_FILTER4 = 4, AD7124_FILTER5 = 5,
    AD7124_FILTER6 = 6, AD7124_FILTER7 = 7
};
enum {
    AD7124_FILTER_SINC4 = 0, AD7124_FILTER_SINC3 = 2,
    AD7124_FILTER_SINC4_FAST = 4, AD7124_FILTER_SINC3_FAST = 5,
    AD7124_FILTER_POST = 7
};
enum {
    AD7124_FILTER_POST_27_27SPS = 2, AD7124_FILTER_POST_25SPS = 3,
    AD7124_FILTER_POST_20SPS = 5, AD7124_FILTER_POST_16_7SPS = 6
};
enum {
    AD7124_UNIPOLAR = 0, AD7124_BIPOLAR = 1
};

void ad7124_init(ad7124_cfg_t* cfg);
void ad7124_reset(void);
unsigned long ad7124_read_one_reg(unsigned char reg);
void ad7124_set_data_status(int data_status);
void ad7124_set_iref(int iref_status);
void ad7124_set_pwr_mode(int mode);
void ad7124_set_mode(int mode);
unsigned long ad7124_read_data(void);
void ad7124_gpio_set_state(int gpio, int state);
void ad7124_gpio_bit_write(int gpio, int val);
void ad7124_iout_set(int iout_id, int iout_curr);
void ad7124_iout_ain_set(int iout_id, int ain);
void ad7124_vbias_set(int ain, int state);
unsigned char ad7124_read_id(void);
unsigned long ad7124_read_err(void);
unsigned char ad7124_get_mclk_cnt(void);
void ad7124_set_channel_state(int channel, int state);
void ad7124_set_channel_config(int channel, int config);
void ad7124_set_config_gain(int config_id, int gain);
void ad7124_set_channel_ain(int channel, int ain_p, int ain_n);
void ad7124_set_config_polarity(int config_id, int state);
void ad7124_set_config_burnout(int config_id, int burnout);
void ad7124_set_config_buffer(int config_id, int ref_buf_p, int ref_buf_n,
    int ain_buf_p, int ain_buf_n);
void ad7124_set_config_ref(int config_id, int ref);
void ad7124_set_filter_type(int filter_id, int filter_type);
void ad7124_set_filter_rej60(int filter_id, int state);
void ad7124_set_filter_post_filter(int filter_id, int post_filter_type);
void ad7124_set_filter_data_rate(int filter_id, int data_rate);
unsigned char ad7124_read_status(void);

#endif
