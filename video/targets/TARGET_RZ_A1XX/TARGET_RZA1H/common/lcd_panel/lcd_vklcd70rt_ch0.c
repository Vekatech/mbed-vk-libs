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
* @file         lcd_vklcd70rt_ch0.c
* @version      1.00
* $Rev: 199 $
* $Date:: 2014-05-23 16:33:52 +0900#$
* @brief        LCD panel for vdc5 channel 0 function
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include    <string.h>

#include    "r_typedefs.h"

#include    "r_vdc5.h"

#include    "iodefine.h"
#include    "lcd_panel.h"

#if     (!defined(MBED_CONF_LCD_BOARD) && (LCD_VDC5_CH0_PANEL==PANEL_VKLCD70RT))

/******************************************************************************
Macro definitions
******************************************************************************/
/* Port 3 */
#define LCD_PORT3_1ST (0xFF03u)
/* Port 4 */
#define LCD_PORT4_1ST (0x00FFu)

#define FH_1_2_CYCLE   (2u)

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables (to be accessed by other files)
******************************************************************************/

ioexp_t ioexp;

/******************************************************************************
Private global variables and functions
/**************************************************************************//**
 * @brief       Backlight setting using RIIC
 * @param[in]   void
 * @retval      None
 *****************************************************************************/
static void set_BackLight_using_pwm (uint8_t dc)
{
/*
  IIC_Read(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR, sizeof(ioexp.reg), ioexp.reg);

  if(ioexp.reg[PWM_DC] != dc)
  {
    ioexp.reg[PWM_DC] = dc;
    IIC_Write(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR+PWM_DC, sizeof(ioexp.regs.PWM_dc), &ioexp.reg[PWM_DC]);
  }

  if(!(ioexp.reg[OUTPUT] & (1<<BL_EN)))
  {
    ioexp.reg[OUTPUT] |= (1<<BL_EN);
    IIC_Write(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR+OUTPUT, sizeof(ioexp.regs.output), &ioexp.reg[OUTPUT]);
    //IIC_Write(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR+offsetof(struct __CONTROL, output), sizeof(exp.regs.output), &exp.reg[OUTPUT])
  }
*/
}   /* End of function set_BackLight_using_pwm() */

void GRAPHICS_ChangeBakLight(uint8_t dc)
{
/*
  IIC_Read(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR+PWM_DC, sizeof(ioexp.regs.PWM_dc), &ioexp.reg[PWM_DC]);
  
  if(ioexp.reg[PWM_DC] != dc)
  {
    ioexp.reg[PWM_DC] = dc;
    IIC_Write(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR+PWM_DC, sizeof(ioexp.regs.PWM_dc), &ioexp.reg[PWM_DC]);
  }
*/
}

void GRAPHICS_TurnLcd(uint8_t on_off)
{
/*
  //ioexp_t ioexp;

  IIC_Read(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR+OUTPUT, sizeof(ioexp.regs.output), &ioexp.reg[OUTPUT]);
  
  if( ((ioexp.reg[OUTPUT]>>BL_EN) & 0x1) != on_off)
  {
    ioexp.reg[OUTPUT] = (on_off)? (ioexp.reg[OUTPUT] | (1<<BL_EN)) : (ioexp.reg[OUTPUT] & (~(1<<BL_EN)));
    IIC_Write(EEIIC_CH, EE_CHIP_ADDR, E_CALIB_DEVICE_ADDR, LCD_CH0_EXP_ADDR+OUTPUT, sizeof(ioexp.regs.output), &ioexp.reg[OUTPUT]);
  }
*/
}
/**************************************************************************//**
 * @brief       LCD panel I/O port setup (VDC5 channel 0)
 * @param[in]   void
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdPanel_Ch0 (void)
{
    volatile uint32_t   dummy_read;
    uint32_t            reg_data;  
    
    /*  VK-LCD70RT (RGB565)
    
      LVDS_UPDATE     //LVDS register update control register                   R/W H'00000000 H'FCFF7A30 32
      LVDSFCL         //LVDS format conversion register L                       R/W H'00000321 H'FCFF7A34 32
      LCLKSELR        //LVDS clock select register                              R/W H'00000000 H'FCFF7A50 32
      LPLLSETR        //LVDSPLL setting register                                R/W H'00000001 H'FCFF7A54 32
      LPHYACC         //LVDS PHY characteristics switching register LPHYACC     R/W H'00000000 H'FCFF7A5C 32
    
      LVDS_interface____________________________________________________________
        Out format 666
            R[5:0]      TXOUT0P - TXOUT0M       ... P5_6 - P5_7, 1st alternative function
            G[5:0]      TXOUT1P - TXOUT1M       ... P5_4 - P5_5, 1st alternative function
            B[5:0]      TXOUT2P - TXOUT2M       ... P5_2 - P5_3, 1st alternative function

            CLK         TXCLKOUTP - TXCLKOUTM   ... P5_0 - P5_1, 1st alternative function

        LCD0_POWER
            BL_EN       : P9_0                  ... P9_0, Port output
            BL_PWM      : TIOC2B                ... P8_15,4th alternative function
    
      Paralel_interface___________________________________________________________
        out format 565
            CLK         : LCD0_CLK              ... P3_0, 1st alternative function
            DE          : LCD0_TCON0            ... P3_1, 1st alternative function
            Hsync       : LCD0_TCON1            ... P3_2, 1st alternative function
            Vsync       : LCD0_TCON6            ... P3_7, 1st alternative function

        LCD0_POWER
            DISPLAY_ON  : P9_0                  ... P9_0, Port output
            BL_PWM      : TIOC2B                ... P8_15,4th alternative function

        LCD0_DATA
            R[4:0]      : LCD0_DATA[15:12]      ... P4_3 ~ P4_7, 1st alternative function
            G[5:0]      : LCD0_DATA[11:9]       ... P4_0 ~ P4_2, 1st alternative function
                        : LCD0_DATA[8:6]        ... P3_13 ~ P3_15, 1st alternative function
            B[4:0]      : LCD0_DATA[5:0]        ... P3_8 ~ P3_12, 1st alternative function
    */
    
  #if ((LCD_CH0_PANEL_CLK == VDC5_PANEL_ICKSEL_LVDS) || (LCD_CH0_PANEL_CLK == VDC5_PANEL_ICKSEL_LVDS_DIV7))
    /* Port 5 */
    reg_data        = (uint32_t)GPIO.PMC5 & (uint32_t)~0xFFu;
    GPIO.PMC5       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC5;
    dummy_read      = reg_data;
    /* PFCAE5, PFCE5, PFC5 ... 1st alternative function
       PIPC5, PMC5
       b7:b0    : P5_7 ~ P5_0 */
    reg_data        = (uint32_t)GPIO.PFCAE5 & (uint32_t)~0xFFu;
    GPIO.PFCAE5     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE5 & (uint32_t)~0xFF;
    GPIO.PFCE5      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC5 & (uint32_t)~0xFF;
    GPIO.PFC5       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PIPC5 & (uint32_t)~0xFFu;
    GPIO.PIPC5      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC5 | (uint32_t)0xFFu;
    GPIO.PMC5       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PM5 | (uint32_t)0xFFu;
    GPIO.PM5        = (uint16_t)reg_data;
  #else
    /* Port 3 */
    reg_data        = (uint32_t)GPIO.PMC3 & (uint32_t)~LCD_PORT3_1ST;
    GPIO.PMC3       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC3;
    dummy_read      = reg_data;
    /* PFCAE3, PFCE3, PFC3 ... 1st alternative function
       PIPC3, PMC3
       b15:b8   : P3_15 ~ P3_8
       b1:b0    : P3_1 ~ P3_0 */
    reg_data        = (uint32_t)GPIO.PFCAE3 & (uint32_t)~LCD_PORT3_1ST;
    GPIO.PFCAE3     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE3 & (uint32_t)~LCD_PORT3_1ST;
    GPIO.PFCE3      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC3 & (uint32_t)~LCD_PORT3_1ST;
    GPIO.PFC3       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PIPC3 | (uint32_t)LCD_PORT3_1ST;
    GPIO.PIPC3      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC3 | (uint32_t)LCD_PORT3_1ST;
    GPIO.PMC3       = (uint16_t)reg_data;
    
    /* Port 4 */
    reg_data        = (uint32_t)GPIO.PMC4 & (uint32_t)~LCD_PORT4_1ST;
    GPIO.PMC4       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC4;
    dummy_read      = reg_data;
    /* PFCAE4, PFCE4, PFC4 ... 1st alternative function
       PIPC4,  PMC4
       b7:b0   : P4_7 ~ P4_0 */
    reg_data        = (uint32_t)GPIO.PFCAE4 & (uint32_t)~LCD_PORT4_1ST;
    GPIO.PFCAE4     = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFCE4 & (uint32_t)~LCD_PORT4_1ST;
    GPIO.PFCE4      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PFC4 & (uint32_t)~LCD_PORT4_1ST;
    GPIO.PFC4       = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PIPC4 | (uint32_t)LCD_PORT4_1ST;
    GPIO.PIPC4      = (uint16_t)reg_data;
    reg_data        = (uint32_t)GPIO.PMC4 | (uint32_t)LCD_PORT4_1ST;
    GPIO.PMC4       = (uint16_t)reg_data;
  #endif

    /* Backlight LED
        PWM channel 0
    */
    set_BackLight_using_pwm(LCD_CH0_INITIAL_DC);

}   /* End of function GRAPHICS_SetLcdPanel_Ch0() */

/**************************************************************************//**
 * @brief       LCD TCON setup parameter acquisition processing (VDC5 channel 0)
 * @param[out]  outctrl     : Address of the area for storing the LCD TCON timing setup data table
 * @retval      None
******************************************************************************/
void GRAPHICS_SetLcdTconSettings_Ch0 (const vdc5_lcd_tcon_timing_t * * const outctrl)
{
    /* VK-LCD70RT (RGB565), WVGA 1024x600 */

    static const vdc5_lcd_tcon_timing_t lcd_tcon_timing_VE =
    {
        (uint16_t)LCD_CH0_DISP_VS,          /* Signal pulse start position */
        (uint16_t)LCD_CH0_DISP_VW,          /* Pulse width */
        VDC5_LCD_TCON_POLMD_NORMAL,
        VDC5_LCD_TCON_REFSEL_HSYNC,
        VDC5_SIG_POL_NOT_INVERTED,          /* Polarity inversion control of signal */
        VDC5_LCD_TCON_PIN_NON,              /* Output pin for LCD driving signal */
        LCD_CH0_OUT_EDGE                    /* Output phase control of signal */
    };
    /* TCON timing setting, HE */
    static const vdc5_lcd_tcon_timing_t lcd_tcon_timing_HE =
    {
        (uint16_t)LCD_CH0_DISP_HS,          /* Signal pulse start position */
        (uint16_t)LCD_CH0_DISP_HW,          /* Pulse width */
        VDC5_LCD_TCON_POLMD_NORMAL,
        VDC5_LCD_TCON_REFSEL_HSYNC,         /* Signal operating reference select */
        VDC5_SIG_POL_NOT_INVERTED,          /* Polarity inversion control of signal */
        VDC5_LCD_TCON_PIN_NON,              /* Output pin for LCD driving signal */
        LCD_CH0_OUT_EDGE                    /* Output phase control of signal */
    };

    /* TCON timing setting, DE */
    static const vdc5_lcd_tcon_timing_t lcd_tcon_timing_DE =
    {
        (uint16_t)0,
        (uint16_t)0,
        VDC5_LCD_TCON_POLMD_NORMAL,
        VDC5_LCD_TCON_REFSEL_HSYNC,
        VDC5_SIG_POL_NOT_INVERTED,          /* Polarity inversion control of signal */
        VDC5_LCD_TCON_PIN_0,                /* Output pin for LCD driving signal */
        LCD_CH0_OUT_EDGE                    /* Output phase control of signal */
    };

    if (outctrl != NULL)
    {
        outctrl[VDC5_LCD_TCONSIG_STVA_VS]   = NULL;                 /* STVA/VS: Vsync */
        outctrl[VDC5_LCD_TCONSIG_STVB_VE]   = &lcd_tcon_timing_VE;  /* STVB/VE: Not used */
        outctrl[VDC5_LCD_TCONSIG_STH_SP_HS] = NULL;                 /* STH/SP/HS: Hsync */
        outctrl[VDC5_LCD_TCONSIG_STB_LP_HE] = &lcd_tcon_timing_HE;  /* STB/LP/HE: Not used */
        outctrl[VDC5_LCD_TCONSIG_CPV_GCK]   = NULL;                 /* CPV/GCK: Not used */
        outctrl[VDC5_LCD_TCONSIG_POLA]      = NULL;                 /* POLA: Not used */
        outctrl[VDC5_LCD_TCONSIG_POLB]      = NULL;                 /* POLB: Not used */
        outctrl[VDC5_LCD_TCONSIG_DE]        = &lcd_tcon_timing_DE;  /* DE: */
    }
}   /* End of function GRAPHICS_SetLcdTconSettings_Ch0() */

#endif  /* LCD_VDC5_CH0_PANEL==PANEL_VKLCD70RT */

