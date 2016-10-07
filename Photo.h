
#include "Arduino.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SD.h>
#include <SPI.h>
#include <util/twi.h>

/******* ARDUINO ATMEGA DEFINES PHOTOS *******/
#define LED         13
#define FLASH       13 ///// TODO
#define SD_MOSI     51
#define SD_MISO     50
#define SD_CLK      52
#define SD_CS       53
#define CAM_SCL     21
#define CAM_SDATA   20
#define CAM_VSYNC   30  // PORTC 7
#define CAM_HREF    31  // PORTC 6
#define CAM_DOUT0   22  // PORTA 0
#define CAM_DOUT1   23  // PORTA 1
#define CAM_DOUT2   24  // PORTA 2
#define CAM_DOUT3   25  // PORTA 3
#define CAM_DOUT4   26  // PORTA 4
#define CAM_DOUT5   27  // PORTA 5
#define CAM_DOUT6   28  // PORTA 6
#define CAM_DOUT7   29  // PORTA 7
#define FIFO_RCLK   32  // PORTC 5
#define FIFO_WREN   33  // PORTC 4
#define FIFO_WRST   34  // PORTC 3
#define FIFO_RRST   35  // PORTC 2


/******* OV7670 REGISTERS *******/
 
#define OV7670_I2C_ADDRESS      0x21
 
#define OV_REG_GAIN             0x00
#define OV_REG_BLUE             0x01
#define OV_REG_RED              0x02
#define OV_REG_VREF             0x03
#define OV_REG_COM1             0x04
#define OV_REG_BAVE             0x05
#define OV_REG_GbAVE            0x06
#define OV_REG_AECHH            0x07
#define OV_REG_RAVE             0x08
#define OV_REG_COM2             0x09
#define OV_REG_PID              0x0A
#define OV_REG_VER              0x0B
#define OV_REG_COM3             0x0C
#define OV_REG_COM4             0x0D
#define OV_REG_COM5             0x0E
#define OV_REG_COM6             0x0F
#define OV_REG_AECH             0x10
#define OV_REG_CLKRC            0x11
#define OV_REG_COM7             0x12
#define OV_REG_COM8             0x13
#define OV_REG_COM9             0x14
#define OV_REG_COM10            0x15
#define OV_REG_HSTART           0x17
#define OV_REG_HSTOP            0x18
#define OV_REG_VSTART           0x19
#define OV_REG_VSTOP            0x1A
#define OV_REG_PSHFT            0x1B
#define OV_REG_MIDH             0x1C
#define OV_REG_MIDL             0x1D
#define OV_REG_MVFP             0x1E
#define OV_REG_LAEC             0x1F
#define OV_REG_ADCCTR0          0x20
#define OV_REG_ADCCTR1          0x21
#define OV_REG_ADCCTR2          0x22
#define OV_REG_ADCCTR3          0x23
#define OV_REG_AEW              0x24
#define OV_REG_AEB              0x25
#define OV_REG_VPT              0x26
#define OV_REG_BBIAS            0x27
#define OV_REG_GbBIAS           0x28
#define OV_REG_EXHCH            0x2A
#define OV_REG_EXHCL            0x2B
#define OV_REG_RBIAS            0x2C
#define OV_REG_ADVFL            0x2D
#define OV_REG_ADVLH            0x2E
#define OV_REG_YAVE             0x2F
#define OV_REG_HSYST            0x30
#define OV_REG_HSYEN            0x31
#define OV_REG_HREF             0x32
#define OV_REG_CHLF             0x33
#define OV_REG_ARBLM            0x34
#define OV_REG_ADC              0x37
#define OV_REG_ACOM             0x38
#define OV_REG_OFON             0x39
#define OV_REG_TSLB             0x3A
#define OV_REG_COM11            0x3B
#define OV_REG_COM12            0x3C
#define OV_REG_COM13            0x3D
#define OV_REG_COM14            0x3E
#define OV_REG_EDGE             0x3F
#define OV_REG_COM15            0x40
#define OV_REG_COM16            0x41
#define OV_REG_COM17            0x42
#define OV_REG_AWBC1            0x43
#define OV_REG_AWBC2            0x44
#define OV_REG_AWBC3            0x45
#define OV_REG_AWBC4            0x46
#define OV_REG_AWBC5            0x47
#define OV_REG_AWBC6            0x48
#define OV_REG_REG4B            0x4B
#define OV_REG_DNSTH            0x4C
#define OV_REG_MTX1             0x4F
#define OV_REG_MTX2             0x50
#define OV_REG_MTX3             0x51
#define OV_REG_MTX4             0x52
#define OV_REG_MTX5             0x53
#define OV_REG_MTX6             0x54
#define OV_REG_BRIGHT           0x55
#define OV_REG_CONTRAS          0x56
#define OV_REG_CONTRAS_CENTER   0x57
#define OV_REG_MTXS             0x58
#define OV_REG_LCC1             0x62
#define OV_REG_LCC2             0x63
#define OV_REG_LCC3             0x64
#define OV_REG_LCC4             0x65
#define OV_REG_LCC5             0x66
#define OV_REG_MANU             0x67
#define OV_REG_MANV             0x68
#define OV_REG_GFIX             0x69
#define OV_REG_GGAIN            0x6A
#define OV_REG_DBLV             0x6B
#define OV_REG_AWBCTR3          0x6C
#define OV_REG_AWBCTR2          0x6D
#define OV_REG_AWBCTR1          0x6E
#define OV_REG_AWBCTR0          0x6F
#define OV_REG_SCALING_XSC      0x70
#define OV_REG_SCALING_YSC      0x71
#define OV_REG_SCALING_DCWCTR   0x72
#define OV_REG_SCALING_PCLK_DIV 0x73
#define OV_REG_REG74            0x74
#define OV_REG_REG75            0x75
#define OV_REG_REG76            0x76
#define OV_REG_REG77            0x77
#define OV_REG_SLOP             0x7A
#define OV_REG_GAM1             0x7B
#define OV_REG_GAM2             0x7C
#define OV_REG_GAM3             0x7D
#define OV_REG_GAM4             0x7E
#define OV_REG_GAM5             0x7F
#define OV_REG_GAM6             0x80
#define OV_REG_GAM7             0x81
#define OV_REG_GAM8             0x82
#define OV_REG_GAM9             0x83
#define OV_REG_GAM10            0x84
#define OV_REG_GAM11            0x85
#define OV_REG_GAM12            0x86
#define OV_REG_GAM13            0x87
#define OV_REG_GAM14            0x88
#define OV_REG_GAM15            0x89
#define OV_REG_RGB444           0x8C
#define OV_REG_DM_LNL           0x92
#define OV_REG_DM_LNH           0x93
#define OV_REG_LCC6             0x94
#define OV_REG_LCC7             0x95
#define OV_REG_BD50ST           0x9D
#define OV_REG_BD60ST           0x9E
#define OV_REG_HAECC1           0x9F
#define OV_REG_HAECC2           0xA0
#define OV_REG_SCALING_PCLK_DEL 0xA2
#define OV_REG_NT_CTRL          0xA4
#define OV_REG_BD50MAX          0xA5
#define OV_REG_HAECC3           0xA6
#define OV_REG_HAECC4           0xA7
#define OV_REG_HAECC5           0xA8
#define OV_REG_HAECC6           0xA9
#define OV_REG_HAECC7           0xAA
#define OV_REG_BD60MAX          0xAB
#define OV_REG_STR_OPT          0xAC
#define OV_REG_STR_R            0xAD
#define OV_REG_STR_G            0xAE
#define OV_REG_STR_B            0xAF
#define OV_REG_ABLC             0xB1
#define OV_REG_THL_ST           0xB3
#define OV_REG_THL_DLT          0xB5
#define OV_REG_AD_CHB           0xBE
#define OV_REG_AD_CHR           0xBF
#define OV_REG_AD_CHGb          0xC0
#define OV_REG_AD_CHGr          0xC1
#define OV_REG_SATCTR           0xC9
 
 
 
struct config{
  uint8_t reg;
  uint16_t data;
};
 
const struct config Config_OV7670[] PROGMEM = {
 
  { OV_REG_COM7, 0x80 },
  { OV_REG_TSLB, 0x04 },
  { OV_REG_COM7, 0x00 },
 
  { OV_REG_HSTART, 0x13 },
  { OV_REG_HSTOP, 0x01 },
  { OV_REG_HREF, 0xB6 },
  { OV_REG_VSTART, 0x02 },
  { OV_REG_VSTOP, 0x7A },
  { OV_REG_VREF, 0x0A },
 
  { OV_REG_COM3, 0x00 },
  { OV_REG_COM14, 0x00 },
  { OV_REG_SCALING_XSC, 0x3A },
  { OV_REG_SCALING_YSC, 0x35 },
  { OV_REG_SCALING_DCWCTR, 0x11 },
  { OV_REG_SCALING_PCLK_DIV, 0xF0 },
  { OV_REG_SCALING_PCLK_DEL, 0x01 },
  { OV_REG_COM10, 0x0 },
 
  { OV_REG_SLOP, 0x20 }, { OV_REG_GAM1, 0x10 },
  { OV_REG_GAM2, 0x1E }, { OV_REG_GAM3, 0x35 },
  { OV_REG_GAM4, 0x5A }, { OV_REG_GAM5, 0x69 },
  { OV_REG_GAM6, 0x76 }, { OV_REG_GAM7, 0x80 },
  { OV_REG_GAM8, 0x88 }, { OV_REG_GAM9, 0x8F },
  { OV_REG_GAM10, 0x96 }, { OV_REG_GAM11, 0xA3 },
  { OV_REG_GAM12, 0xAF }, { OV_REG_GAM13, 0xC4 },
  { OV_REG_GAM14, 0xD7 }, { OV_REG_GAM15, 0xE8 },
 
  { OV_REG_COM8, 0x80 | 0x40 },
  { OV_REG_GAIN, 0x00 },
  { OV_REG_AECH, 0x00 },
  { OV_REG_COM4, 0x40 },
  { OV_REG_COM9, 0x18 },
  { OV_REG_BD50MAX, 0x05 },
  { OV_REG_BD60MAX, 0x07 },
  { OV_REG_AEW, 0x95 },
  { OV_REG_AEB, 0x33 },
  { OV_REG_VPT, 0xE3 },
  { OV_REG_HAECC1, 0x78 },
  { OV_REG_HAECC2, 0x68 }, { 0xA1, 0x03 },
  { OV_REG_HAECC3, 0xD8 }, { OV_REG_HAECC4, 0xD8 },
  { OV_REG_HAECC5, 0xF0 }, { OV_REG_HAECC6, 0x90 },
  { OV_REG_HAECC7, 0x94 },
  { OV_REG_COM8, 0x80 | 0x40 | 0x40 | 0x01 },
  { OV_REG_HSYST, 0 }, { OV_REG_HSYEN, 0 },
   
  { OV_REG_COM5, 0x61 },
  { OV_REG_COM6, 0x4B },
  { 0x16, 0x02 },
  { OV_REG_MVFP, 0x07 },
  { OV_REG_ADCCTR1, 0x02 },
  { OV_REG_ADCCTR2, 0x91 },
  { 0x29, 0x07 },
  { OV_REG_CHLF, 0x0B },
  { 0x35, 0x0B },
  { OV_REG_ADC, 0x1D },
  { OV_REG_ACOM, 0x71 },
  { OV_REG_OFON, 0x2A },
  { OV_REG_COM12, 0x78 },
  { 0x4D, 0x40 },
  { 0x4e, 0x20 },
  { OV_REG_GFIX, 0x00 },
  { OV_REG_REG74, 0x10 },
  { 0x8D, 0x4F }, { 0x8E, 0x00 },
  { 0x8F, 0x00 }, { 0x90, 0x00 },
  { 0x91, 0x00 }, { 0x96, 0x00 },
  { 0x9A, 0x00 }, { 0xB0, 0x84 },
  { OV_REG_ABLC, 0x0C }, { 0xB2, 0x0E },
  { OV_REG_THL_ST, 0x82 }, { 0xB8, 0x0A },
 
  { OV_REG_AWBC1, 0x0A }, { OV_REG_AWBC2, 0xF0 },
  { OV_REG_AWBC3, 0x34 }, { OV_REG_AWBC4, 0x58 },
  { OV_REG_AWBC5, 0x28 }, { OV_REG_AWBC6, 0x3A },
  { 0x59, 0x88 }, { 0x5A, 0x88 },
  { 0x5B, 0x44 }, { 0x5C, 0x67 },
  { 0x5D, 0x49 }, { 0x5E, 0x0E },
  { OV_REG_AWBCTR3, 0x0A }, { OV_REG_AWBCTR2, 0x55 },
  { OV_REG_AWBCTR1, 0x11 }, { OV_REG_AWBCTR0, 0x9E },
  { OV_REG_GGAIN, 0x40 }, { OV_REG_BLUE, 0x40 },
  { OV_REG_RED, 0x60 },
  { OV_REG_COM8, 0x80 | 0x40 | 0x04 | 0x01 | 0x02 },
 
  { OV_REG_MTX1, 0x80 }, { OV_REG_MTX2, 0x80 },
  { OV_REG_MTX3, 0x00 }, { OV_REG_MTX4, 0x22 },
  { OV_REG_MTX5, 0x5E }, { OV_REG_MTX6, 0x80 },
  { OV_REG_MTXS, 0x9E },
 
  { OV_REG_COM16, 0x08 }, { OV_REG_EDGE, 0x00 },
  { OV_REG_REG75, 0x05 }, { OV_REG_REG76, 0xE1 },
  { OV_REG_DNSTH, 0x00 }, { OV_REG_REG77, 0x01 },
  { OV_REG_COM13, 0x48 }, { OV_REG_REG4B, 0x09 },
  { OV_REG_SATCTR, 0x60 },
  { OV_REG_CONTRAS, 0x40 },
 
  { OV_REG_ARBLM, 0x11 }, { OV_REG_COM11, 0x02 | 0x10 },
  { OV_REG_NT_CTRL, 0x82 }, { 0x96, 0x00 },
  { 0x97, 0x30 }, { 0x98, 0x20 },
  { 0x99, 0x30 }, { 0x9a, 0x84 },
  { 0x9b, 0x29 }, { 0x9c, 0x03 },
  { OV_REG_BD50ST, 0x4C }, { OV_REG_BD60ST, 0x3F },
  { 0x78, 0x04 },
 
  { 0x79, 0x01 }, { 0xc8, 0xF0 },
  { 0x79, 0x0F }, { 0xc8, 0x00 },
  { 0x79, 0x10 }, { 0xc8, 0x7E },
  { 0x79, 0x0A }, { 0xc8, 0x80 },
  { 0x79, 0x0B }, { 0xc8, 0x01 },
  { 0x79, 0x0C }, { 0xc8, 0x0F },
  { 0x79, 0x0D }, { 0xc8, 0x20 },
  { 0x79, 0x09 }, { 0xc8, 0x80 },
  { 0x79, 0x02 }, { 0xc8, 0xC0 },
  { 0x79, 0x03 }, { 0xc8, 0x40 },
  { 0x79, 0x05 }, { 0xc8, 0x30 },
  { 0x79, 0x26 },
   
  { 0xFF, 0xFF }, /* END MARKER */
};
 
 
const struct config VGA_OV7670[] PROGMEM = {
  {OV_REG_HREF,0xF6},  // was B6  
  {OV_REG_HSTART,0x13},    // HSTART
  {OV_REG_HSTOP,0x01},    // HSTOP
  {OV_REG_VSTART,0x02},    // VSTART
  {OV_REG_VSTOP,0x7A},    // VSTOP
  {OV_REG_VREF,0x0A},  // VREF
   
  {0xFF, 0xFF},   /* END MARKER */
};
 
 
const struct config BayerRGB_OV7670[] PROGMEM = {
  {OV_REG_COM7, 0x01},
  {OV_REG_COM13, 0x08},
  {OV_REG_COM15, 0xC0},
  {OV_REG_COM16, 0x3D},
  {OV_REG_REG76, 0xE1},
   
  {0xFF, 0xFF}, /* END MARKER */
};
 
 
const byte header[]  = {0x42,0x4D,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x36,0x00,0x00,0x00, // Offset Image :  (offset palette 0x36) + 4*(2**NumColorBits) = 54+4*(2**8) = 1078 = 0x436
                        0x28,0x00,0x00,0x00, // Header Length
                        0x80,0x02,0x00,0x00,0xE0,0x01,0x00,0x00,0x01,0x00, // Width 640 , Height 480, Num Plan 1
                        0x18,0x00, // Numb Color Bits yuv : 24 bits
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
                        };
 
 
/******* PHOTO FUNCTIONS *******/
 
// Prototypes
int Error(void);
void NoError(void);
void Flash(int state);
void twiStart(void);
void twiWriteByte(uint8_t DATA, uint8_t type);
void wrReg(uint8_t reg, uint8_t dat);
static uint8_t twiRd(uint8_t nack);
uint8_t rdReg(uint8_t reg);
void wrSensorRegs8_8(const struct config reglist[]);
void Reset_FIFO(void);
void Enable_Write_FIFO(void);
void Disable_Write_FIFO(void);
void Init_Sensor(void);
void Reset_INT(int* cpt_INT,int pin_INT);
int captureImg(File myFile);
int Take_Photo(void);
void Go_Photo(int* flag_INT,int pin_INT);
