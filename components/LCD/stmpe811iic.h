/*******************************************************************************

*******************************************************************************/
/******************************************************************************
* File Name    : tpiic.h
* Description  : TPIIC header
******************************************************************************/
#ifndef STMPE811IIC_H
#define STMPE811IIC_H

#include "EasyAttach_CameraAndLCD.h"

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/

#define __USE_DEFAULT_CALIBRATION_DATA__

#define STMPE811_DEVICE_ADDR   (0x82)
//#define STMPE811_DEVICE_ADDR	(0x88)
//#define EE_LPC11U35_DEVICE_ADDR (0xA0)

#define TPIIC_CH      	3
#define TPIIC_SCL		P1_6
#define TPIIC_SDA		P1_7

#if  (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT)
  #define TPIRQ_CH      1
  #define TPIRQ_PIN     P7_8

  #define EEIIC_CH	3
  #define EEIIC_SCL	P1_6
  #define EEIIC_SDA	P1_7
  #define EE_CALIB_DEVICE_ADDR  0xA8

  #define TPCALIBRATION_DATA				0x38,0x00,0x00,0x00,0x01,0x00,0x00,0x00,\
    0x50,0xb4,0x52,0x38,0xec,0x97,0xd0,0x3f,0xd7,0xfe,0x58,0xc5,0x10,0xb2,0x52,0x3f,\
	0xde,0x09,0xc6,0x87,0xaa,0x7d,0xbb,0x3e,0x65,0xaa,0xb4,0xd2,0x56,0x04,0xc5,0xbf,\
	0x02,0xb3,0x33,0x61,0x9b,0x1e,0x34,0xc0,0x88,0x87,0xc3,0x39,0x38,0x7d,0x83,0x40
#else
  #define TPIRQ_CH      2
  #define TPIRQ_PIN     P1_2

  #define EEIIC_CH		0
  #define EEIIC_SCL		P1_0
  #define EEIIC_SDA		P1_1
  #define EE_CALIB_DEVICE_ADDR  0xA8

  #define TPCALIBRATION_DATA				0x38,0x00,0x00,0x00,0x01,0x00,0x00,0x00,\
	0x66,0x44,0x6c,0xe9,0x11,0xa1,0xbf,0xbf,0x70,0x8f,0x61,0x1c,0x3f,0x5e,0x20,0x3f,\
	0xad,0x80,0xca,0x76,0x62,0xb9,0x8a,0x3e,0xae,0x95,0xef,0x7e,0xa7,0x8d,0xb2,0xbf,\
	0x24,0x37,0x82,0x68,0xe6,0xc1,0x7e,0x40,0xf4,0xb6,0xa8,0xe8,0x88,0xa3,0x71,0x40
#endif

#define FIFO_DEPTH     128      // packets, which means 12bit x + 12bit y +8bit = 32bit for 1 data packet

//sys regs
#define CHIP_ID        0x00     // 16 R 0x0811 Device identification
#define ID_VER         0x02     // 8 R 0x01 Revision number 0x01 for engineering sample for final silicon
#define SYS_CTRL1      0x03     // 8 R/W 0x00 Reset control
#define SYS_CTRL2      0x04     // 8 R/W 0x0F Clock control
#define SPI_CFG        0x08     // 8 R/W 0x01 SPI interface configuration
#define INT_CTRL       0x09     // 8 R/W 0x00 Interrupt control register
#define INT_EN         0x0A     // 8 R/W 0x00 Interrupt enable register
#define INT_STA        0x0B     // 8 R 0x10 interrupt status register
#define GPIO_EN        0x0C     // 8 R/W 0x00 GPIO interrupt enable register
#define GPIO_INT_STA   0x0D     // 8 R 0x00 GPIO interrupt status register
#define ADC_INT_EN     0x0E     // 8 R/W 0x00 ADC interrupt enable register
#define ADC_INT_STA    0x0F     // 8 R 0x00 ADC interrupt status register
#define GPIO_SET_PIN   0x10     // 8 R/W 0x00 GPIO set pin register
#define GPIO_CLR_PIN   0x11     // 8 R/W 0x00 GPIO clear pin register
#define GPIO_MP_STA    0x12     // 8 R/W 0x00 GPIO monitor pin state register
#define GPIO_DIR       0x13     // 8 R/W 0x00 GPIO direction register
#define GPIO_ED        0x14     // 8 R/W 0x00 GPIO edge detect register
#define GPIO_RE        0x15     // 8 R/W 0x00 GPIO rising edge register
#define GPIO_FE        0x16     // 8 R/W 0x00 GPIO falling edge register
#define GPIO_AF        0x17     // 8 R/W 0x00 Alternate function register

//ADC regs
#define ADC_CTRL1      0x20     // 8 R/W 0x9C ADC control
#define ADC_CTRL2      0x21     // 8 R/W 0x01 ADC control
#define ADC_CAPT       0x22     // 8 R/W 0xFF To initiate ADC data acquisition
#define ADC_DATA_CH0   0x30     // 16 R 0x0000 ADC channel 0
#define ADC_DATA_CH1   0x32     // 16 R 0x0000 ADC channel 1
#define ADC_DATA_CH2   0x34     // 16 R 0x0000 ADC channel 2
#define ADC_DATA_CH3   0x36     // 16 R 0x0000 ADC channel 3
#define ADC_DATA_CH4   0x38     // 16 R 0x0000 ADC channel 4
#define ADC_DATA_CH5   0x3A     // 16 R 0x0000 ADC channel 5
#define ADC_DATA_CH6   0x3C     // 16 R 0x0000 ADC channel 6
#define ADC_DATA_CH7   0x3E     // 16 R 0x0000 ADC channel 7

//touch regs
#define TSC_CTRL       0x40     // 8 R/W 0x90 4-wire touchscreen controller setup
#define TSC_CFG        0x41     // 8 R/W 0x00 Touchscreen controller configuration
#define WDW_TR_X       0x42     // 16 R/W 0x0FFF Window setup for top right X
#define WDW_TR_Y       0x44     // 16 R/W 0x0FFF Window setup for top right Y
#define WDW_BL_X       0x46     // 16 R/W 0x0000 Window setup for bottom left X
#define WDW_BL_Y       0x48     // 16 R/W 0x0000 Window setup for bottom left Y
#define FIFO_TH        0x4A     // 8 R/W 0x00 FIFO level to generate interrupt
#define FIFO_STA       0x4B     // 8 R/W 0x20 Current status of FIFO
#define FIFO_SIZE      0x4C     // 8 R 0x00 Current filled level of FIFO
#define TSC_DATA_X     0x4D     // 16 R 0x0000 Data port for touchscreen controller data access
#define TSC_DATA_Y     0x4F     // 16 R 0x0000 Data port for touchscreen controller data access
#define TSC_DATA_Z     0x51     // 8 R 0x0000 Data port for touchscreen controller data access
#define TSC_DATA_XYZ   0x52     // 32 R 0x00000000 Data port for touchscreen controller data access
#define TSC_FRACT_Z    0x56     // 8 0x00
#define TSC_DATA_AI    0x57     // 8 R 0x00 Data port for touchscreen controller data access AUTO_INCREMENT
#define TSC_DATA_NAI   0x07     // 8 R 0x00 Data port for touchscreen controller data access NONE_AUTO_INCREMENT
#define TSC_DATA_FIFO  0xD7     //
#define TSC_I_DRIVE    0x58     // 8 R/W 0x00
#define TSC_SHIELD     0x59     // 8 R/W 0x00

//temperature regs
#define TEMP_CTRL      0x60     // 8 R/W 0x00 Temperature sensor setup
#define TEMP_DATA      0x61     // 8 R 0x00 Temperature data access port
#define TEMP_TH        0x62     // 8 R/W 0x00 Threshold for temperature controlled interrupt

//interrupt status & enable bits
#define INT_GPIO	   0x80
#define INT_ADC        0x40
#define INT_TEMP_SENS  0x20
#define INT_FIFO_EMPTY 0x10
#define INT_FIFO_FULL  0x08
#define INT_FIFO_OFLOW 0x04
#define INT_FIFO_TH    0x02
#define INT_TOUCH_DET  0x01

//other bits
#define TSC_STA		   0x80

//Z axis accuracy
#define _8_0		   0x00
#define _7_1		   0x01
#define _6_2		   0x02
#define _5_3		   0x03
#define _4_4		   0x04
#define _3_5		   0x05
#define _2_6		   0x06
#define _1_7		   0x07

#define INIT_DATA											/* {register, value}, ...	*/																				\
{SYS_CTRL1, 0x02},											/* 0. Reset touchscreen controller	*/																		\
{SYS_CTRL2, 0x0C},											/* 1. Disable the clock gating for the touchscreen controller and ADC in the SYS_CFG2 register*/			\
{INT_EN, (INT_FIFO_OFLOW | INT_FIFO_TH | INT_TOUCH_DET)},	/* 2. Configure which interrupts to be outputed */															\
{ADC_CTRL1, 0x49},											/* 3. Write: register Add = 0x20 ADC_CTRL1, data = 0x49 delay, 2mS */										\
{ADC_CTRL2, 0x01},											/* 4. Write: register Add = 0x21 ADC_CTRL2, data = 0x01 */													\
{GPIO_AF,   0x00},											/* 5. Write: register Add = 0x17 GPIO_AF, data = 0x00 */													\
{TSC_CFG,   0x6C},											/* 6. 5ms panel voltage setting time, 1ms touch detection delay, averaging on every 2-nd sample */			\
{FIFO_TH,   0x05},											/* 7. Group points reading (if more than 4 points collected generate interrupt) */							\
{FIFO_STA,  0x01},											/* 8a. Clear FIFO */																						\
{FIFO_STA,  0x00},											/* 8b. Reset FIFO */																						\
{TSC_FRACT_Z, _8_0},										/* 9. Choose the appropriate accurency of the measured pressure of the touch 7 fractional bits & 1 int */ 	\
{TSC_I_DRIVE, 0x00},										/* 10. Write: register Add = 0x58 TSC_I_DRIVE, data = 0x01 */												\
{TSC_CTRL,  0x01},											/* 11. Configure the operating mode and the window tracking index, start touch detection & data acquisition*/\
{INT_STA,   0xFF},											/* 12. Clear Interrupt register*/ 																			\
{INT_CTRL,  0x03},											/* 13. Configure the interrupt signal and enable the interrupts //int-> Edge interrupt -> falling edge*/

/******************************************************************************
Variable Externs
******************************************************************************/

typedef union __CALIB {
	struct __DATA
	{
	 unsigned long len;
	 unsigned long flag;
		   double KX1, KY1, KX2, KY2, KX3, KY3;
	}data;
	unsigned char KX08[sizeof(struct __DATA)];
}touch_calib_data_t;

typedef union RAW_SAMPLE__ {
	struct AXIS__
    {
		unsigned short x;
        unsigned short y;
        unsigned long  z;
    }axis;
    unsigned long xy;
    unsigned long long dot;
}touch_raw_data_t;

typedef union SCREEN_SAMPLE__ {
	struct AXIS_
    {
		short x;
        short y;
        float z;
    }axis;
    unsigned long xy;
    unsigned long long dot;
}touch_screen_data_t;

typedef union DRAW_SAMPLE__ {
	struct POINT_
    {
		short x;
        short y;
    }axis;
    unsigned long dot;
}draw_data_t;

#endif  /* STMPE811IIC_H */
