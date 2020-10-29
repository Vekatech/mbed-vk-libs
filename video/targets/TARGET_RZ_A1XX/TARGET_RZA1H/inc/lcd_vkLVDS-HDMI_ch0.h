/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2012 - 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/**************************************************************************//**
* @file         lcd_vklcd70rt_ch0.h
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        LCD panel for vdc5 channel 0 definition header
******************************************************************************/

#ifndef LCD_VKLVDS_HDMI_CH0_H
#define LCD_VKLVDS_HDMI_CH0_H

#ifndef LCD_PANEL_H
#error  Do not include this file directly!
#else
/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <stdlib.h>

#include    "r_typedefs.h"

#include    "r_vdc5.h"


/******************************************************************************
Macro definitions
******************************************************************************/
#define     LCD_CH0_DISP_HS         (48u)                       /* LCD display area size, horizontal start position */
#define     LCD_CH0_DISP_HW         (1280u)                     /* LCD display area size, horizontal width */
#define     LCD_CH0_DISP_HSYNC      (32u)                       /* Hsync width */
#define     LCD_CH0_DISP_VS         (18u) //(3u)                /* LCD display area size, vertical start position */
#define     LCD_CH0_DISP_VW         (720u)                      /* LCD display area size, height (vertical width) */
#define     LCD_CH0_DISP_VSYNC      (6u)                        /* Vsync width */
#define     LCD_CH0_SIG_FV          (755u)                      /* Free-running Vsync period */
#define     LCD_CH0_SIG_FH          (1440u)                     /* Hsync period */
/* Pixel data is latched in the falling edge of pixel clock.
   Therefore, pixel data should be output from VDC5 at the rising edge of the clock. */
#define     LCD_CH0_OUT_EDGE        VDC5_EDGE_RISING            /* Output phase control of LCD_DATA[23:0] signal */
#define     LCD_CH0_OUT_FORMAT      VDC5_LCD_OUTFORMAT_RGB888   /* LCD output format select */

#define     LCD_CH0_PANEL_CLK       VDC5_PANEL_ICKSEL_LVDS_DIV7 /* Panel clock select */
#define     LCD_CH0_PANEL_CLK_DIV   VDC5_PANEL_CLKDIV_1_2       /* Panel clock frequency division ratio */

#define     LCD_CH0_TCON_HALF       (LCD_CH0_SIG_FH / 2u)       /* TCON reference timing, 1/2fH timing */
#define     LCD_CH0_TCON_OFFSET     (0u)                        /* TCON reference timing, offset Hsync signal timing */

#define     LCD_CH0_INITIAL_DC      30

#define     LCD_CH0_TFP410_ADDR     0x78
#define     LCD_CH0_TFP410_EN       0xBF

/******************************************************************************
Typedef definitions
******************************************************************************/

#define HDMI_IIC_SCL            P1_6
#define HDMI_IIC_SDA            P1_7

#define TFP410_REG_VEN_ID       (0x00)
#define TFP410_REG_DEV_ID       (0x02)
#define TFP410_REG_REV_ID       (0x04)
#define TFP410_REG_CTL_1_MODE   (0x08)
#define TFP410_REG_CTL_2_MODE   (0x09)

//#define HDMI_IIC_CH           3
//#define TI_VEN_ID             0x014C
//#define TFP410_DEV_ID         0x0410

/******************************************************************************
Exported global functions (to be accessed by other files)
******************************************************************************/
void GRAPHICS_SetLcdPanel_Ch0(void);
void GRAPHICS_SetLcdTconSettings_Ch0(const vdc5_lcd_tcon_timing_t * * const outctrl);


#endif  /* LCD_PANEL_H not defined */
#endif  /* LCD_VKLVDS_HDMI_CH0_H */
