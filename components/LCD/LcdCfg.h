#ifndef LCD_CFG_DISPLAY_PANEL_H
#define LCD_CFG_DISPLAY_PANEL_H

#if MBED_CONF_LCD_BOARD
#include "lcd_panel.h"
#endif
#include "DisplayBace.h"

/* LCD Parameter */
#define LCD_INPUT_CLOCK                     (66.67)  /* not use */
#define LCD_OUTPUT_CLOCK                    ((float)(LCD_CH0_SIG_FH*LCD_CH0_SIG_FV*60)/1000000.0f)
#define LCD_PIXEL_WIDTH                     LCD_CH0_DISP_HW
#define LCD_PIXEL_HEIGHT                    LCD_CH0_DISP_VW
#define LCD_H_BACK_PORCH                    LCD_CH0_DISP_HS
#define LCD_H_FRONT_PORCH                   (LCD_CH0_SIG_FH - (LCD_CH0_DISP_HSYNC + LCD_CH0_DISP_HS + LCD_CH0_DISP_HW))
#define LCD_H_SYNC_WIDTH                    LCD_CH0_DISP_HSYNC
#define LCD_V_BACK_PORCH                    LCD_CH0_DISP_VS
#define LCD_V_FRONT_PORCH                   (LCD_CH0_SIG_FV - (LCD_CH0_DISP_VSYNC + LCD_CH0_DISP_VS + LCD_CH0_DISP_VW))
#define LCD_V_SYNC_WIDTH                    LCD_CH0_DISP_VSYNC

extern const DisplayBase::lcd_config_t LcdCfgTbl_Display_panel;

#endif
