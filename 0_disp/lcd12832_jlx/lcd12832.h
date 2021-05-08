#ifndef _LCD12832_H
#define _LCD12832_H

#include "misc.h"

typedef struct {
	void (*cs_f)(int);
	void (*rs_f)(int);
	void (*reset_f)(int);
	void (*fastwrite_f)(void *buf, int nbytes);
} lcd12832_cfg_t;

void LCD12832_Clear(unsigned char byte);
void LCD12832_Init(lcd12832_cfg_t *pcfg);
void LCD12832_Write(unsigned char *buf, int size);

#endif
