#ifndef __ENC28J60_H
#define __ENC28J60_H

typedef unsigned char (*enc28j60_writebyte_func_t) (unsigned char);
typedef void (*enc28j60_setcs_func_t) (int cs);

typedef struct {
    enc28j60_writebyte_func_t writebyte_f;
    enc28j60_setcs_func_t setcs_f;
} enc28j60_cfg_t;

void ENC28J60_Init(enc28j60_cfg_t * cfg, unsigned char *macaddr);
void ENC28J60_SetCLKOUT(unsigned char clk);
void ENC28J60_TX(unsigned int len, void *packet);
unsigned char ENC28J60_GetRev(void);
unsigned int ENC28J60_RX(unsigned int maxlen, void *packet);

#endif
