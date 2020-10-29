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
* @file         lcd_vklcd50rta_ch0.c
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
#include    "rza_io_regrw.h"

#if     (!defined(MBED_CONF_LCD_BOARD) && (LCD_VDC5_CH0_PANEL==PANEL_VKLCD50RTA))

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

/******************************************************************************
Private global variables and functions
******************************************************************************/

/**************************************************************************//**
 * @brief       Backlight setting using RIIC
 * @param[in]   void
 * @retval      None
 *****************************************************************************/
static void set_BackLight_using_pwm (uint8_t dc)
{
  
#define PERIOD 3330     // 10kHz output @ 33,3 MHz input

      /* === Initialization of PORT function ==== */
    /* ---- P8_15 : TIOC2B ---- */
    /* Port initialize */
    RZA_IO_RegWrite_16(&GPIO.PIBC8,  0, GPIO_PIBC8_PIBC815_SHIFT,   GPIO_PIBC8_PIBC815);
    RZA_IO_RegWrite_16(&GPIO.PBDC8,  0, GPIO_PBDC8_PBDC815_SHIFT,   GPIO_PBDC8_PBDC815);
    RZA_IO_RegWrite_16(  &GPIO.PM8,  1,     GPIO_PM8_PM815_SHIFT,       GPIO_PM8_PM815);
    RZA_IO_RegWrite_16( &GPIO.PMC8,  0,   GPIO_PMC8_PMC815_SHIFT,     GPIO_PMC8_PMC815);
    RZA_IO_RegWrite_16(&GPIO.PIPC8,  0, GPIO_PIPC8_PIPC815_SHIFT,   GPIO_PIPC8_PIPC815);
    /* Port mode : Multiplex mode                     */
    /* Port function setting : 4th multiplex function */
    /* I/O control mode : Peripheral function         */
    /* Bidirectional mode : Disable                   */
    RZA_IO_RegWrite_16( &GPIO.PBDC8, 0,   GPIO_PBDC8_PBDC815_SHIFT,   GPIO_PBDC8_PBDC815);
    RZA_IO_RegWrite_16(  &GPIO.PM8,  0,       GPIO_PM8_PM815_SHIFT,       GPIO_PM8_PM815);
    RZA_IO_RegWrite_16(&GPIO.PFCAE8, 0, GPIO_PFCAE8_PFCAE815_SHIFT, GPIO_PFCAE8_PFCAE815);
    RZA_IO_RegWrite_16( &GPIO.PFCE8, 1,   GPIO_PFCE8_PFCE815_SHIFT,   GPIO_PFCE8_PFCE815);
    RZA_IO_RegWrite_16(  &GPIO.PFC8, 1,     GPIO_PFC8_PFC815_SHIFT,     GPIO_PFC8_PFC815);
    RZA_IO_RegWrite_16( &GPIO.PIPC8, 0,   GPIO_PIPC8_PIPC815_SHIFT,   GPIO_PIPC8_PIPC815);
    RZA_IO_RegWrite_16(  &GPIO.PMC8, 1,     GPIO_PMC8_PMC815_SHIFT,     GPIO_PMC8_PMC815);

    /* ==== Processing when MTU2 module standby mode is not cancelled ==== */
    //if (1 == RZA_IO_RegRead_8(&CPG.STBCR3, CPG_STBCR3_MSTP33_SHIFT, CPG_STBCR3_MSTP33))
    //{
    //    volatile uint8_t dummy_buf;
    //    /* ==== Module standby clear ==== */
    //    /* ---- Supply clock to the MTU2 ---- */
    //    RZA_IO_RegWrite_8(&CPG.STBCR3, 0, CPG_STBCR3_MSTP33_SHIFT, CPG_STBCR3_MSTP33);
    //    dummy_buf = CPG.STBCR3;     /* Dummy read */
    //}

    /*[1] Select the counter clock with bits TPSC2 to TPSC0 in TCR. At the same time, select the
    input clock edge with bits CKEG1 and CKEG0 in TCR.
    [2] Use bits CCLR2 to CCLR0 in TCR to select the TGR to be used as the TCNT clearing source.
    [3] Use TIOR to designate the TGR as an output compare register, and select the initial value and output value.
    [4] Set the cycle in the TGR selected in [2], and set the duty in the other TGR.
    [5] Select the PWM mode with bits MD3 to MD0 in TMDR.
    [6] Set the CST bit in TSTR to 1 to start the count operation.

    TCR_2    - CCLR[1] CCLR[0] CKEG[1] CKEG[0] TPSC[2] TPSC[1] TPSC[0]
    TGRB_2   16 bit value
    TIOR_2     IOB[3] IOB[2] IOB[1] IOB[0] IOA[3] IOA[2] IOA[1] IOA[0]
    TMDR_2   - - - - MD[3] MD[2] MD[1] MD[0]
    */
    if(dc>100)
      dc = 100;

    MTU2.TCR_2 =  0x20; //TRGB_1 clears TCNT_2 on match;
    MTU2.TIOR_2 = 0x21; //set otuput to 1 on match (init val 0)
    MTU2.TGRA_2 = PERIOD-1; //match value;
    MTU2.TGRB_2 = (dc*PERIOD)/100;
    MTU2.TMDR_2 = 0x03; //PWM2 mode;*/
    RZA_IO_RegWrite_8(&MTU2.TSTR, 1, MTU2_TSTR_CST2_SHIFT, MTU2_TSTR_CST2);

}   /* End of function set_BackLight_using_pwm() */

void GRAPHICS_ChangeBakLight(uint8_t dc)
{
  if(dc > 100)
    MTU2.TGRB_2 = PERIOD-1;
  else
    MTU2.TGRB_2 = (dc*PERIOD)/100;
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

    /*  VK-LCD50RTA (RGB565)
                        : LCD0_CLK              ... P3_0, 1st alternative function
            DE          : LCD0_TCON0            ... P3_1, 1st alternative function
        LCD0_DATA
            R[4:0]      : LCD0_DATA[15:12]      ... P4_3 ~ P4_7, 1st alternative function
            G[5:0]      : LCD0_DATA[11:9]       ... P4_0 ~ P4_2, 1st alternative function
                        : LCD0_DATA[8:6]        ... P3_13 ~ P3_15, 1st alternative function
            B[4:0]      : LCD0_DATA[5:0]        ... P3_8 ~ P3_12, 1st alternative function
    */
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
    /* VK-LCD50RTA (RGB565), WVGA 480x272 */
    /* TCON timing setting, VE */
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

#endif  /* LCD_VDC5_CH0_PANEL==PANEL_VKLCD50RTA */

