#include "nrf24l01.h"
#include "misc.h"
#include <string.h>

#define CMD_READ_REG            0x00    // 读配置寄存器,低5位为寄存器地址
#define CMD_WRITE_REG           0x20    // 写配置寄存器,低5位为寄存器地址
#define CMD_READ_RX_PL          0x61    // 读RX有效数据,1~32字节
#define CMD_WRITE_TX_PL         0xa0    // 写TX有效数据,1~32字节
#define CMD_FLUSH_TX            0xe1    // 清除TX FIFO寄存器.发射模式下用
#define CMD_FLUSH_RX            0xe2    // 清除RX FIFO寄存器.接收模式下用
#define CMD_REUSE_TX_PL         0xe3    // 重新使用上一包数据,CE为高,数据包被不断发送.
#define CMD_READ_RX_PL_WIDTH    0x60    // R_RX_PL_WID
#define CMD_WRITE_ACK_PL        0xa8    // W_ACK_PAYLOAD, +3bit ACK pipe no(000 to 101)
#define CMD_WRITE_TX_PL_NOACK   0xb0    // W_TX_PAYLOAD_NO_AC
#define CMD_NOP                 0xff    // 空操作,可以用来读状态寄存器
#define REG_CONFIG              0x00    // 配置寄存器地址
#define REG_EN_AA               0x01    // 使能自动应答功能
#define REG_EN_RXADDR           0x02    // 接收地址允许
#define REG_SETUP_AW            0x03    // 设置地址宽度(所有数据通道)
#define REG_SETUP_RETR          0x04    // 建立自动重发
#define REG_RF_CH               0x05    // RF通道
#define REG_RF_SETUP            0x06    // RF寄存器
#define REG_STATUS              0x07    // 状态寄存器
#define REG_OBSERVE_TX          0x08    // 发送检测寄存器
#define REG_CD                  0x09    // 载波检测寄存器
#define REG_RX_ADDR_P0          0x0a    // 数据通道0接收地址
#define REG_RX_ADDR_P1          0x0b    // 数据通道1接收地址
#define REG_RX_ADDR_P2          0x0c    // 数据通道2接收地址
#define REG_RX_ADDR_P3          0x0d    // 数据通道3接收地址
#define REG_RX_ADDR_P4          0x0e    // 数据通道4接收地址
#define REG_RX_ADDR_P5          0x0f    // 数据通道5接收地址
#define REG_TX_ADDR             0x10    // 发送地址寄存器
#define REG_RX_PW_P0            0x11    // 接收数据通道0有效数据宽度(1~32字节)
#define REG_RX_PW_P1            0x12    // 接收数据通道1有效数据宽度(1~32字节)
#define REG_RX_PW_P2            0x13    // 接收数据通道2有效数据宽度(1~32字节)
#define REG_RX_PW_P3            0x14    // 接收数据通道3有效数据宽度(1~32字节)
#define REG_RX_PW_P4            0x15    // 接收数据通道4有效数据宽度(1~32字节)
#define REG_RX_PW_P5            0x16    // 接收数据通道5有效数据宽度(1~32字节)
#define REG_FIFO_STATUS         0x17    // FIFO状态寄存器
#define REG_DYNPD               0x1c    // enable dynamic payload length in data pipes
#define REG_FEATURE             0x1d    // global feature reg
#define AD_LEN                  5
#define TX_PAYLOAD_WIDTH        30

static nrf24l01_cfg_t g;

//#define g.readwrite_f(x) NRF24L01_g.readwrite_f(x)

static void WriteReg(unsigned char reg, unsigned char val)
{
    g.cs_f(0);
    g.readwrite_f(reg | CMD_WRITE_REG);
    g.readwrite_f(val);
    g.cs_f(1);
}

static unsigned char ReadReg(unsigned char reg)
{
    unsigned char val;
    g.cs_f(0);
    g.readwrite_f(reg);
    val = g.readwrite_f(0xff);
    g.cs_f(1);
    return val;
}

static unsigned char WriteData(unsigned char reg, unsigned char* buf, int len)
{
    unsigned char status;
    int i;
    g.cs_f(0);
    status = g.readwrite_f(CMD_WRITE_REG | reg);
    for(i = 0; i < len; i++)
        g.readwrite_f(buf[i]);
    g.cs_f(1);
    return status;
}
void NRF24L01_SetupRFCH(unsigned char val)
{
    WriteReg(REG_RF_CH, val);
}

unsigned char ReadData(unsigned char reg, unsigned char* buf, int len)
{
    unsigned char status, i;
    g.cs_f(0);
    status = g.readwrite_f(reg);
    for(i = 0; i < len; i++)
        buf[i] = g.readwrite_f(0xff);
    g.cs_f(1);
    return status;
}

// 读24L01的RX:5Payload
// 参数: 读出长度length, 缓冲区指针rxpl_buf 返回值:    状态
void NRF24L01_ReadRxPayload(void* rxpl_buf, int len)
{
    int i;
    unsigned char* p = rxpl_buf;
    g.cs_f(0);
    g.readwrite_f(CMD_READ_RX_PL);
    for(i = 0; i < len; i++) {
        p[i] = g.readwrite_f(0xff);
    }
    g.cs_f(1);
}
// 写24L01的TX Payload
// 参数:       读出长度length, 缓冲区指针txpl_buf 返回值:    状态
void NRF24L01_WriteTxPayload(void* txpl_buf, int len)
{
    WriteData(CMD_WRITE_TX_PL_NOACK, txpl_buf, len);
}
//刷新24L01的TX FIFO
void NRF24L01_FlushTx(void)
{
    g.cs_f(0);
    g.readwrite_f(CMD_FLUSH_TX);
    g.cs_f(1);
}
//刷新24L01的RX FIFO
void NRF24L01_FlushRx(void)
{
//    unsigned char status;
    g.cs_f(0);
    g.readwrite_f(CMD_FLUSH_RX);
//    status = g.readwrite_f(CMD_FLUSH_RX);
    g.cs_f(1);
//    return status;
}
//重用24L01的TX Payload
unsigned char ReuseTxPayload(void)
{
    unsigned char status;
    g.cs_f(0);
    status = g.readwrite_f(CMD_REUSE_TX_PL);
    g.cs_f(1);
    return status;
}
//读取24L01的RX Payload长度
unsigned char ReadRxPayloadWidth(unsigned char length,
    unsigned char* txpl_width)
{
    unsigned char status;
    g.cs_f(0);
    status = g.readwrite_f(CMD_READ_RX_PL_WIDTH);
    txpl_width[0] = g.readwrite_f(0xff);
    g.cs_f(1);
    return status;
}
//写24L01的ACK Payload(对接收端)
//参数: 管道号pipe(0到5), 读出长度length, 缓冲区指针txpl_buf 返回值:    状态
unsigned char WriteAckTxPayload(unsigned char pipe, unsigned char length,
    unsigned char* txpl_buf)
{
    unsigned char status, i;
    g.cs_f(0);
    status = g.readwrite_f(CMD_WRITE_ACK_PL + pipe);
    for(i = 0; i < length; i++) {
        g.readwrite_f(txpl_buf[i]);
    }
    g.cs_f(1);
    return status;
}
//写24L01的TX Payload, 不需要ACK(对发送端)
//参数:       读出长度length, 缓冲区指针txpl_buf 返回值:    状态
unsigned char WriteNoAckTxPayload(unsigned char length, unsigned char* txpl_buf)
{
    unsigned char status, i;
    g.cs_f(0);
    status = g.readwrite_f(CMD_WRITE_TX_PL_NOACK);
    for(i = 0; i < length; i++) {
        g.readwrite_f(txpl_buf[i]);
    }
    g.cs_f(1);
    return status;
}
//读取24L01的状态. 通过写一个NOP指令实现.
//---------------------底层操作结束----------------------

#define GetStatus() ReadReg(REG_STATUS)
#define GetFifoStatus() ReadReg(REG_FIFO_STATUS)
#define ClearIRQ()    WriteReg(REG_STATUS, 0xff);
#define SetAsTx() WriteReg(REG_CONFIG, 0x0e);
#define SetAsRx() WriteReg(REG_CONFIG, 0x0f);

void NRF24L01_SetMode(int mode)
{
    if(mode == NRF24L01_MODE_TX)
        WriteReg(REG_CONFIG, 0x0e);
    else if(mode == NRF24L01_MODE_RX)
        WriteReg(REG_CONFIG, 0x0f);
}

//todo: 设置channel, data rate等常量的函数

void NRF24L01_SetupRxPw(unsigned char pipe, unsigned char pw)
{
    WriteReg(REG_RX_PW_P0 + pipe, pw);
}
void NRF24L01_SetupRxAddr1(unsigned char rxaddrptr)
{
    WriteReg(REG_RX_ADDR_P1, rxaddrptr);
}
void NRF24L01_SetupRxAddr2345(unsigned char pipe, unsigned char rxaddr)
{
    WriteReg(REG_RX_ADDR_P0 + pipe, rxaddr);	//pipe>=2
}

void NRF24L01_Init(nrf24l01_cfg_t* cfg)
{
    g = *cfg;
    g.ce_f(0);
    WriteData(REG_RX_ADDR_P0, g.rxaddr, 5);
    WriteData(REG_TX_ADDR, g.txaddr, 5);
    WriteReg(REG_EN_AA, g.en_aa);
    WriteReg(REG_EN_RXADDR, g.en_rxaddr);
    WriteReg(REG_SETUP_RETR, g.retry);
    WriteReg(REG_RF_CH, g.rf_ch);
//    while(1) { NRF24L01_ReadAllRegs(); _delay_ms(500); }
}

#include <xprintf.h>
unsigned char NRF24L01_GetStatus(void)
{
    return GetStatus();
}

unsigned char NRF24L01_GetFifoStatus(void)
{
    return GetFifoStatus();
}

void NRF24L01_ClearIRQ(void)
{
    ClearIRQ();
}

void NRF24L01_ReadAllRegs(void)
{
    xprintf("\n00-07: ");
    xprintf("%02x ", 0xff & ReadReg(REG_CONFIG));
    xprintf("%02x ", 0xff & ReadReg(REG_EN_AA));
    xprintf("%02x ", 0xff & ReadReg(REG_EN_RXADDR));
    xprintf("%02x ", 0xff & ReadReg(REG_SETUP_AW));
    xprintf("%02x ", 0xff & ReadReg(REG_SETUP_RETR));
    xprintf("%02x ", 0xff & ReadReg(REG_RF_CH));
    xprintf("%02x ", 0xff & ReadReg(REG_RF_SETUP));
    xprintf("%02x ", 0xff & ReadReg(REG_STATUS));
    xprintf("\n08-0f: ");
    xprintf("%02x ", 0xff & ReadReg(REG_OBSERVE_TX));
    xprintf("%02x ", 0xff & ReadReg(REG_CD));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_ADDR_P0));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_ADDR_P1));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_ADDR_P2));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_ADDR_P3));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_ADDR_P4));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_ADDR_P5));
    xprintf("\n10-17: ");
    xprintf("%02x ", 0xff & ReadReg(REG_TX_ADDR));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_PW_P0));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_PW_P1));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_PW_P2));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_PW_P3));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_PW_P4));
    xprintf("%02x ", 0xff & ReadReg(REG_RX_PW_P5));
    xprintf("%02x", 0xff & ReadReg(REG_FIFO_STATUS));
    xprintf("\n");
}

int NRF24L01_TX(const void* buf, int len)
{
    int n = 25;
    NRF24L01_FlushTx();
    NRF24L01_WriteTxPayload(buf, len);
    SetAsTx();
    g.ce_f(1);
    g.delay_us_f(500);
//    g.ce_f(0);
//    do { n--; g.delay_us_f(50); } while(!(GetStatus() & 0x20) && n);
//    xprintf("### %02x %02x\n", 0xff & GetStatus(), 0xff & GetFifoStatus());
//    ClearIRQ();
    return n;
}

int NRF24L01_RX(void* buf, unsigned char len);

int NRF24L01_RX(void* buf, unsigned char len)
{
    int n = 100;
    g.ce_f(0);
    NRF24L01_FlushRx();
    NRF24L01_SetupRxPw(0, 32);
    SetAsRx();
    g.ce_f(1);
    g.delay_us_f(500);

    do {
        g.delay_us_f(50);
        n--;
    } while(!(GetStatus() & 0x40) && n);
    xprintf("### %02x %02x %d\n", 0xff & GetStatus(),
        0xff & NRF24L01_GetFifoStatus(), n);

    if(n > 0)
        NRF24L01_ReadRxPayload(buf, len);
    ClearIRQ();
    return 0;
}

void NRF24L01_Poll(void)
{
    unsigned char buf[32];
    memset(buf, 0, 32);
    NRF24L01_RX(buf, 32);
    if(buf[0])
        xprintf("### %s ###\n", buf);
}
