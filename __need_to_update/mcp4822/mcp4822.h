#ifndef _MCP4822_H
#define _MCP4822_H

#include "misc.h"

#define MCP4822_PORT GPIOB
#define MCP4822_CS   GPIO_Pin_2
#define MCP4822_SCK  GPIO_Pin_10
#define MCP4822_SDI  GPIO_Pin_11

//extern void MCP4822_Write(u16 data);
extern void MCP4822_WriteDacA(u16 data);
extern void MCP4822_WriteDacB(u16 data);

#endif
