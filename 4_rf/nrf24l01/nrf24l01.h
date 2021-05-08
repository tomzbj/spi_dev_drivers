#ifndef _NRF24L01_H
#define _NRF24L01_H

enum {
    NRF24L01_MODE_TX, NRF24L01_MODE_RX
};

typedef struct {
    void (*cs_f)(int);
    void (*ce_f)(int);
    void (*delay_us_f)(volatile int);
    unsigned char (*readwrite_f)(unsigned char);
    unsigned char* txaddr;
    unsigned char* rxaddr;
    unsigned char en_aa;
    unsigned char en_rxaddr;
    unsigned char rf_ch;
    unsigned char retry;
} nrf24l01_cfg_t;

void NRF24L01_Init(nrf24l01_cfg_t* cfg);
void NRF24L01_WriteTxPayload(void* buf, int len);
void NRF24L01_ReadAllRegs(void);

void NRF24L01_SetupRxPw(unsigned char pipe, unsigned char pw);
void NRF24L01_FlushTx(void);
void NRF24L01_FlushRx(void);

void NRF24L01_ReadRxPayload(void* rxpl_buf, int len);

unsigned char NRF24L01_GetFifoStatus(void);
void NRF24L01_SetMode(int mode);

int NRF24L01_TX(const void* buf, int len);
unsigned char NRF24L01_GetStatus(void);
void NRF24L01_ClearIRQ(void);
int NRF24L01_RX(void* buf, unsigned char len);

#endif
