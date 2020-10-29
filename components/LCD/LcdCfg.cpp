#include "LcdCfg.h"

#if MBED_CONF_LCD_BOARD

const DisplayBase::lcd_config_t LcdCfgTbl_Display_panel = {
(MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA)? DisplayBase::LCD_TYPE_PARALLEL_RGB : DisplayBase::LCD_TYPE_LVDS           /* lcd_type             */
    , LCD_INPUT_CLOCK                                                               /* intputClock          */
    , LCD_OUTPUT_CLOCK                                                              /* outputClock          */
    , (DisplayBase::lcd_outformat_t)LCD_CH0_OUT_FORMAT                              /* lcd_outformat        */
    , (DisplayBase::edge_t)LCD_CH0_OUT_EDGE                                         /* lcd_edge             */
    , LCD_CH0_SIG_FH                                                                /* h_toatal_period      */
    , LCD_CH0_SIG_FV                                                                /* v_toatal_period      */
    , LCD_PIXEL_WIDTH                                                               /* h_disp_widht         */
    , LCD_PIXEL_HEIGHT                                                              /* v_disp_widht         */
    , LCD_H_BACK_PORCH                                                              /* h_back_porch         */
    , LCD_V_BACK_PORCH                                                              /* v_back_porch         */
    , DisplayBase::LCD_TCON_PIN_NON                                                 /* h_sync_port          */
    , DisplayBase::SIG_POL_NOT_INVERTED                                             /* h_sync_port_polarity */
    , LCD_H_SYNC_WIDTH                                                              /* h_sync_width         */
    , DisplayBase::LCD_TCON_PIN_NON                                                 /* v_sync_port          */
    , DisplayBase::SIG_POL_NOT_INVERTED                                             /* v_sync_port_polarity */
    , LCD_V_SYNC_WIDTH                                                              /* v_sync_width         */
    , DisplayBase::LCD_TCON_PIN_0                                                   /* de_port              */
    , DisplayBase::SIG_POL_NOT_INVERTED                                             /* de_port_polarity     */
};

#endif // MBED_CONF_LCD_BOARD
