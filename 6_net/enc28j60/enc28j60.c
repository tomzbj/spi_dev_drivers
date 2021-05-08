#include "enc28j60.h"
#include <stdio.h>

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
#define ADDR_MASK        0x1F   // - Register address        (bits 0-4)
#define BANK_MASK        0x60   // - Bank number        (bits 5-6)
//#define SPRD_MASK        0x80 // - MAC/PHY indicator        (bit 7)
#define RXSTART_INIT     0x0    // start with recbuf at 0/
#define RXSTOP_INIT      (0x1FFF-0x0600-1)  // receive buffer end
#define TXSTART_INIT     (0x1FFF-0x0600)    // start TX buffer at 0x1FFF-0x0600, pace for one full ethernet frame (~1500 bytes)
#define TXSTOP_INIT      0x1FFF // stp TX buffer at end of mem
// max frame length which the conroller will accept:
#define        MAX_FRAMELEN        1500 // (note: maximum ethernet frame length would be 1518)
//#define MAX_FRAMELEN     600

// SPI operation codes
enum {
    OP_READ_CTRL_REG = 0x00, OP_READ_BUF_MEM = 0x3A, OP_WRITE_CTRL_REG = 0x40,
    OP_WRITE_BUF_MEM = 0x7A, OP_BIT_FIELD_SET = 0x80, OP_BIT_FIELD_CLR = 0xA0,
    OP_SOFT_RESET = 0xFF
};

// All-bank registers
enum {
    EIE = 0x1B, EIR = 0x1C, ESTAT = 0x1D, ECON2 = 0x1E, ECON1 = 0x1F
};

// Bank 0 registers
enum {
    ERDPTL = 0x00, ERDPTH = 0x01, EWRPTL = 0x02, EWRPTH = 0x03, ETXSTL = 0x04,
    ETXSTH = 0x05, ETXNDL = 0x06, ETXNDH = 0x07, ERXSTL = 0x08, ERXSTH = 0x09,
    ERXNDL = 0x0A, ERXNDH = 0x0B, ERXRDPTL = 0x0C, ERXRDPTH = 0x0D,
    ERXWRPTL = 0x0E, ERXWRPTH = 0x0F, EDMASTL = 0x10, EDMASTH = 0x11,
    EDMANDL = 0x12, EDMANDH = 0x13, EDMADSTL = 0x14, EDMADSTH = 0x15,
    EDMACSL = 0x16, EDMACSH = 0x17
};
// Bank 1 registers
enum {
    EHT0 = 0x20, EHT1 = 0x21, EHT2 = 0x22, EHT3 = 0x23, EHT4 = 0x24,
    EHT5 = 0x25, EHT6 = 0x26, EHT7 = 0x27, EPMM0 = 0x28, EPMM1 = 0x29,
    EPMM2 = 0x2A, EPMM3 = 0x2B, EPMM4 = 0x2C, EPMM5 = 0x2D, EPMM6 = 0x2E,
    EPMM7 = 0x2F, EPMCSL = 0x30, EPMCSH = 0x31, EPMOL = 0x34, EPMOH = 0x35,
    EWOLIE = 0x36, EWOLIR = 0x37, ERXFCON = 0x38, EPKTCNT = 0x39
};
// Bank 2 registers
enum {
    MACON1 = 0xc0, MACON2 = 0xc1, MACON3 = 0xc2, MACON4 = 0xc3, MABBIPG = 0xc4,
    MAIPGL = 0xc6, MAIPGH = 0xc7, MACLCON1 = 0xc8, MACLCON2 = 0xc9,
    MAMXFLL = 0xcA, MAMXFLH = 0xcB, MAPHSUP = 0xcD, MICON = 0xd1, MICMD = 0xd2,
    MIREGADR = 0xd4, MIWRL = 0xd6, MIWRH = 0xd7, MIRDL = 0xd8, MIRDH = 0xd9
};
// Bank 3 registers
enum {
    MAADR1 = 0xe0, MAADR0 = 0xe1, MAADR3 = 0xe2, MAADR2 = 0xe3, MAADR5 = 0xe4,
    MAADR4 = 0xe5, EBSTSD = 0x66, EBSTCON = 0x67, EBSTCSL = 0x68,
    EBSTCSH = 0x69, MISTAT = 0xeA, EREVID = 0x62, ECOCON = 0x65, EFLOCON = 0x67,
    EPAUSL = 0x68, EPAUSH = 0x69
};

// PHY registers
enum {
    PHCON1 = 0x00, PHSTAT1 = 0x01, PHHID1 = 0x02, PHHID2 = 0x03, PHCON2 = 0x10,
    PHSTAT2 = 0x11, PHIE = 0x12, PHIR = 0x13, PHLCON = 0x14
};

// ENC28J60 ERXFCON Register Bit Definitions
enum {
    ERXFCON_UCEN = 0x80, ERXFCON_ANDOR = 0x40, ERXFCON_CRCEN = 0x20,
    ERXFCON_PMEN = 0x10, ERXFCON_MPEN = 0x08, ERXFCON_HTEN = 0x04,
    ERXFCON_MCEN = 0x02, ERXFCON_BCEN = 0x01
};
// ENC28J60 EIE Register Bit Definitions
enum {
    EIE_INTIE = 0x80, EIE_PKTIE = 0x40, EIE_DMAIE = 0x20, EIE_LINKIE = 0x10,
    EIE_TXIE = 0x08, EIE_WOLIE = 0x04, EIE_TXERIE = 0x02, EIE_RXERIE = 0x01
};
// ENC28J60 EIR Register Bit Definitions
enum {
    EIR_PKTIF = 0x40, EIR_DMAIF = 0x20, EIR_LINKIF = 0x10, EIR_TXIF = 0x08,
    EIR_WOLIF = 0x04, EIR_TXERIF = 0x02, EIR_RXERIF = 0x01
};
// ENC28J60 ESTAT Register Bit Definitions
enum {
    ESTAT_INT = 0x80, ESTAT_LATECOL = 0x10, ESTAT_RXBUSY = 0x04,
    ESTAT_TXABRT = 0x02, ESTAT_CLKRDY = 0x01
};
// ENC28J60 ECON2 Register Bit Definitions
enum {
    ECON2_AUTOINC = 0x80, ECON2_PKTDEC = 0x40, ECON2_PWRSV = 0x20,
    ECON2_VRPS = 0x08
};
// ENC28J60 ECON1 Register Bit Definitions
enum {
    ECON1_TXRST = 0x80, ECON1_RXRST = 0x40, ECON1_DMAST = 0x20,
    ECON1_CSUMEN = 0x10, ECON1_TXRTS = 0x08, ECON1_RXEN = 0x04,
    ECON1_BSEL1 = 0x02, ECON1_BSEL0 = 0x01
};
// ENC28J60 MACON1 Register Bit Definitions
enum {
    MACON1_LOOPBK = 0x10, MACON1_TXPAUS = 0x08, MACON1_RXPAUS = 0x04,
    MACON1_PASSALL = 0x02, MACON1_MARXEN = 0x01
};
// ENC28J60 MACON2 Register Bit Definitions
enum {
    MACON2_MARST = 0x80, MACON2_RNDRST = 0x40, MACON2_MARXRST = 0x08,
    MACON2_RFUNRST = 0x04, MACON2_MATXRST = 0x02, MACON2_TFUNRST = 0x01
};
// ENC28J60 MACON3 Register Bit Definitions
enum {
    MACON3_PADCFG2 = 0x80, MACON3_PADCFG1 = 0x40, MACON3_PADCFG0 = 0x20,
    MACON3_TXCRCEN = 0x10, MACON3_PHDRLEN = 0x08, MACON3_HFRMLEN = 0x04,
    MACON3_FRMLNEN = 0x02, MACON3_FULDPX = 0x01
};
// ENC28J60 MICMD Register Bit Definitions
enum {
    MICMD_MIISCAN = 0x02, MICMD_MIIRD = 0x01
};
// ENC28J60 MISTAT Register Bit Definitions
enum {
    MISTAT_NVALID = 0x04, MISTAT_SCAN = 0x02, MISTAT_BUSY = 0x01
};
// ENC28J60 PHY PHCON1 Register Bit Definitions
enum {
    PHCON1_PRST = 0x8000, PHCON1_PLOOPBK = 0x4000, PHCON1_PPWRSV = 0x0800,
    PHCON1_PDPXMD = 0x0100
};
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
enum {
    PHSTAT1_PFDPX = 0x1000, PHSTAT1_PHDPX = 0x0800, PHSTAT1_LLSTAT = 0x0004,
    PHSTAT1_JBSTAT = 0x0002
};
// ENC28J60 PHY PHCON2 Register Bit Definitions
enum {
    PHCON2_FRCLINK = 0x4000, PHCON2_TXDIS = 0x2000, PHCON2_JABBER = 0x0400,
    PHCON2_HDLDIS = 0x0100
};

// ENC28J60 Packet Control Byte Bit Definitions
enum {
    PKTCTRL_PHUGEEN = 0x08, PKTCTRL_PPADEN = 0x04, PKTCTRL_PCRCEN = 0x02,
    PKTCTRL_POVERRIDE = 0x01
};

static enc28j60_cfg_t g;

static unsigned char Enc28j60Bank;
static unsigned int NextPacketPtr;

#define WriteByte(x) g.writebyte_f(x)
#define ReadByte() g.writebyte_f(0xff)

static unsigned char ReadOp(unsigned char op, unsigned char addr)
{
    unsigned char data = 0;

    g.setcs_f(0);
    data = op | (addr & ADDR_MASK);
    WriteByte(data);
    data = ReadByte();
    if (addr & 0x80) {
        data = ReadByte();      // do dummy read if needed (for mac and mii, see datasheet page 29)
    }
    g.setcs_f(1);
    return data;
}

static void WriteOp(unsigned char op, unsigned char addr, unsigned char data)
{
    g.setcs_f(0);
    WriteByte(op | (addr & ADDR_MASK)); // issue write command
    WriteByte(data);            // write data
    g.setcs_f(1);
}

static void ENC28J60_Read(int len, void *data)
{
    g.setcs_f(0);
    WriteByte(OP_READ_BUF_MEM); // issue read command
    while (len) {
        len--;
        // read data
//        *data = (unsigned char)WriteByte(0);
        *(unsigned char *)data = ReadByte();
        data++;
    }
    *(unsigned char *)data = '\0';
    g.setcs_f(1);
}

static void ENC28J60_Write(int len, void *data)
{
    g.setcs_f(0);
    WriteByte(OP_WRITE_BUF_MEM);    // issue write command

    while (len) {
        len--;
        WriteByte(*(unsigned char *)data);
        data++;
    }
    g.setcs_f(1);
}

static void SetBank(unsigned char addr)
{
    if ((addr & BANK_MASK) != Enc28j60Bank) {   // set the bank (if needed)
        WriteOp(OP_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1 | ECON1_BSEL0));
        WriteOp(OP_BIT_FIELD_SET, ECON1, (addr & BANK_MASK) >> 5);
        Enc28j60Bank = (addr & BANK_MASK);
    }
}

static unsigned char ReadReg(unsigned char addr)
{
    SetBank(addr);              // set the bank
    return ReadOp(OP_READ_CTRL_REG, addr);  // do the read
}

static void WriteReg(unsigned char addr, unsigned char data)
{
    SetBank(addr);              // set the bank
    WriteOp(OP_WRITE_CTRL_REG, addr, data); // do the write
}

static void WritePhy(unsigned char addr, unsigned int data)
{
    WriteReg(MIREGADR, addr);   // set the PHY register addr
    WriteReg(MIWRL, data);      // write the PHY data
    WriteReg(MIWRH, data >> 8);
    while (ReadReg(MISTAT) & MISTAT_BUSY) { // wait until the PHY write completes
        //Del_10us(1);
        //_nop_();
    }
}

void ENC28J60_SetCLKOUT(unsigned char clk)
{
    WriteReg(ECOCON, clk & 0x7);    //setup clkout: 2 is 12.5MHz:
}

void ENC28J60_Init(enc28j60_cfg_t * cfg, unsigned char *macaddr)
{
    g.setcs_f = cfg->setcs_f;
    g.writebyte_f = cfg->writebyte_f;
    g.setcs_f(1);               // initialize I/O

//enc28j60SetSCK();
//enc28j60HWreset();
    WriteOp(OP_SOFT_RESET, 0, OP_SOFT_RESET);   // perform system reset
//  Del_1ms(250);
// check CLKRDY bit to see if reset is complete
// The CLKRDY does not work. See Rev. B4 Silicon Errata point. Just wait.
//while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
// do bank 0 stuff
// initialize receive buffer
// 16-bit transfers, must write low byte first
// set receive buffer start addr
    NextPacketPtr = RXSTART_INIT;
    WriteReg(ERXSTL, RXSTART_INIT & 0xff);  // Rx start
    WriteReg(ERXSTH, RXSTART_INIT >> 8);
    WriteReg(ERXRDPTL, RXSTART_INIT & 0xff);    // set receive pointer addr
    WriteReg(ERXRDPTH, RXSTART_INIT >> 8);
    WriteReg(ERXNDL, RXSTOP_INIT & 0xff);   // RX end
    WriteReg(ERXNDH, RXSTOP_INIT >> 8);
    WriteReg(ETXSTL, TXSTART_INIT & 0xff);  // TX start
    WriteReg(ETXSTH, TXSTART_INIT >> 8);
    WriteReg(ETXNDL, TXSTOP_INIT & 0xff);   // TX end
    WriteReg(ETXNDH, TXSTOP_INIT >> 8);
// do bank 1 stuff, packet filter:
// For broadcast packets we allow only ARP packtets
// All other packets should be unicast only for our mac (MAADR)
//
// The pattern to match on is therefore
// Type     ETH.DST
// ARP      BROADCAST
// 06 08 -- ff ff ff ff ff ff -> ip checksum for theses bytes=f7f9
// in binary these poitions are:11 0000 0011 1111
// This is hex 303F->EPMM0=0x3f,EPMM1=0x30
    WriteReg(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_PMEN);
    WriteReg(EPMM0, 0x3f);
    WriteReg(EPMM1, 0x30);
    WriteReg(EPMCSL, 0xf9);
    WriteReg(EPMCSH, 0xf7);
// do bank 2 stuff
// enable MAC receive
    WriteReg(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
// bring MAC out of reset
    WriteReg(MACON2, 0x00);
// enable automatic padding to 60bytes and CRC operations
    WriteOp(OP_BIT_FIELD_SET, MACON3,
            MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN | MACON3_FULDPX);
    WriteReg(MAIPGL, 0x12);     // set inter-frame gap (non-back-to-back)
    WriteReg(MAIPGH, 0x0C);
    WriteReg(MABBIPG, 0x12);    // set inter-frame gap (back-to-back)
    WriteReg(MAMXFLL, MAX_FRAMELEN & 0xff); // Set the maximum packet size which the controller will accept
    WriteReg(MAMXFLH, MAX_FRAMELEN >> 8);   // Do not send packets longer than MAX_FRAMELEN:
// do bank 3 stuff
    WriteReg(MAADR5, macaddr[0]);   // write MAC addr NOTE: MAC addr in ENC28J60 is byte-backward
    WriteReg(MAADR4, macaddr[1]);
    WriteReg(MAADR3, macaddr[2]);
    WriteReg(MAADR2, macaddr[3]);
    WriteReg(MAADR1, macaddr[4]);
    WriteReg(MAADR0, macaddr[5]);

    printf("MAADR5 = 0x%x\r\n", ReadReg(MAADR5));
    printf("MAADR4 = 0x%x\r\n", ReadReg(MAADR4));
    printf("MAADR3 = 0x%x\r\n", ReadReg(MAADR3));
    printf("MAADR2 = 0x%x\r\n", ReadReg(MAADR2));
    printf("MAADR1 = 0x%x\r\n", ReadReg(MAADR1));
    printf("MAADR0 = 0x%x\r\n", ReadReg(MAADR0));

    WritePhy(PHCON1, PHCON1_PDPXMD);
    WritePhy(PHCON2, PHCON2_HDLDIS);    // no loopback of transmitted frames
    SetBank(ECON1);             // switch to bank 0
    WriteOp(OP_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);  // enable interrutps
    WriteOp(OP_BIT_FIELD_SET, ECON1, ECON1_RXEN);   // enable packet reception
}

// read the revision of the chip:
unsigned char ENC28J60_GetRev(void)
{
    return (ReadReg(EREVID));
}

void ENC28J60_TX(unsigned int len, void *packet)
{
    WriteReg(EWRPTL, TXSTART_INIT & 0xff);  // Set the write pointer to start of transmit buffer area
    WriteReg(EWRPTH, TXSTART_INIT >> 8);
    WriteReg(ETXNDL, (TXSTART_INIT + len) & 0xff);  // Set the TXND pointer to correspond to the packet size given
    WriteReg(ETXNDH, (TXSTART_INIT + len) >> 8);
    WriteOp(OP_WRITE_BUF_MEM, 0, 0x00); // write per-packet control byte (0x00 means use macon3 settings)
    ENC28J60_Write(len, packet);    // copy the packet into the transmit buffer
    WriteOp(OP_BIT_FIELD_SET, ECON1, ECON1_TXRTS);  // send the contents of the transmit buffer onto the network
    if ((ReadReg(EIR) & EIR_TXERIF)) {  // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
        WriteOp(OP_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }
}

// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
unsigned int ENC28J60_RX(unsigned int maxlen, void *packet)
{
    unsigned int rxstat;
    unsigned int len;

// check if a packet has been received and buffered
//if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
// The above does not work. See Rev. B4 Silicon Errata point 6.
    if (ReadReg(EPKTCNT) == 0) {
        return (0);
    }
    WriteReg(ERDPTL, (NextPacketPtr));  // Set the read pointer to the start of the received packet
    WriteReg(ERDPTH, (NextPacketPtr) >> 8);
    NextPacketPtr = ReadOp(OP_READ_BUF_MEM, 0); // read the next packet pointer
    NextPacketPtr |= ReadOp(OP_READ_BUF_MEM, 0) << 8;

    len = ReadOp(OP_READ_BUF_MEM, 0);   // read the packet length (see datasheet page 43)
    len |= ReadOp(OP_READ_BUF_MEM, 0) << 8;

    len -= 4;                   //remove the CRC count
    rxstat = ReadOp(OP_READ_BUF_MEM, 0);    // read the receive status (see datasheet page 43)
    rxstat |= ReadOp(OP_READ_BUF_MEM, 0) << 8;
    if (len > maxlen - 1) {     // limit retrieve length
        len = maxlen - 1;
    }
// check CRC and symbol errors (see datasheet page 44, table 7-3):
// The ERXFCON.CRCEN is set by default. Normally we should not
// need to check this.
    if ((rxstat & 0x80) == 0) {
        // invalid
        len = 0;
    } else {
        ENC28J60_Read(len, packet); // copy the packet from the receive buffer
    }
// Move the RX read pointer to the start of the next received packet
    WriteReg(ERXRDPTL, (NextPacketPtr));    // This frees the memory we just read out
    WriteReg(ERXRDPTH, (NextPacketPtr) >> 8);

// decrement the packet counter indicate we are done with this packet
    WriteOp(OP_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);

    WritePhy(PHLCON, 0x0476);
//    WritePhy(PHLCON, 0x7a4);
    return (len);
}
