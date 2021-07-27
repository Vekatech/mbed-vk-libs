/* Copyright (c) 2017 dkato
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "rtos.h"
#include "EasyAttach_CameraAndLCD.h"
#include "dcache-control.h"

#if MBED_CONF_LCD_BOARD

static volatile uint8_t         vIDX = 0;
static volatile int32_t         vsync_count = 0;
static volatile osThreadId      LCDNotifyID = NULL;
static volatile uint16_t        everyXsync = 0;
DisplayBase::graphics_layer_t   used_LCDlayer;

#if defined(TARGET_VK_RZ_A1H)
#if (MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA)
#define LCD_CNTRST_PIN    P8_15
//#define CTRL_SCL
//#define CTRL_SDA

static PwmOut lcd_cntrst(LCD_CNTRST_PIN);
#elif (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT)
#define CTRL_SCL           LVDS_IIC_SCL
#define CTRL_SDA           LVDS_IIC_SDA

static I2C Xtender(CTRL_SDA, CTRL_SCL);
ioexp_t IOEXP;
#elif(MBED_CONF_LCD_BOARD == PANEL_VKLVDS_HDMI)
#define CTRL_SCL           HDMI_IIC_SCL
#define CTRL_SDA           HDMI_IIC_SDA

static I2C hdmi_Xmitter(CTRL_SDA, CTRL_SCL);
const char HDMI_Init[2] = {TFP410_REG_CTL_1_MODE, LCD_CH0_TFP410_EN};
#else
//#define LCD_PWON_PIN      P8_15
//static DigitalOut lcd_cntrst(LCD_CNTRST_PIN);
#endif
#endif

static float voltage_adjust = (TYPICAL_BACKLIGHT_VOL / MAX_BACKLIGHT_VOL);

static void Vsync_IRQ(DisplayBase::int_type_t int_type)
{
    if (vsync_count > 0)
    {
    	vsync_count--;
    }

    if(LCDNotifyID && !vsync_count)
    {
    	osSignalSet(LCDNotifyID, V_SYNC);
        if(everyXsync)
        	vsync_count = everyXsync;
    }
}

static const DisplayBase::lcd_config_t * lcd_port_init(DisplayBase& Display) {
#if ((MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT) || (MBED_CONF_LCD_BOARD == PANEL_VKLVDS_HDMI))
  #if defined(TARGET_VK_RZ_A1H)
    PinName lvds_pin[] = {
        P5_7, P5_6, P5_5, P5_4, P5_3, P5_2, P5_1, P5_0
    };
  #else
    #error "MBED_CONF_LCD_BOARD is not supported in this target."
  #endif
    Display.Graphics_Lvds_Port_Init(lvds_pin, sizeof(lvds_pin)/sizeof(lvds_pin[0]));
#else
  #if defined(TARGET_VK_RZ_A1H)
                       /* D15,  D14,  D13,  D12,  D11,  D10,   D9,   D8,   D7,    D6,    D5,    D4,    D3,    D2,    D1,   D0,   DE,  CLK */
    PinName lcd_pin[] = {P4_7, P4_6, P4_5, P4_4, P4_3, P4_2, P4_1, P4_0, P3_15, P3_14, P3_13, P3_12, P3_11, P3_10, P3_9, P3_8, P3_1, P3_0};
  #else
    #error "MBED_CONF_LCD_BOARD is not supported in this target."
  #endif
    Display.Graphics_Lcd_Port_Init(lcd_pin, sizeof(lcd_pin)/sizeof(lcd_pin[0]));
#endif

#if (MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA)
    ThisThread::sleep_for(200);
    lcd_cntrst.period_us(100);
#elif (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT)
    char addr = IOEXP_REG_IO_DATA;

	Xtender.frequency(100000);
	if(!Xtender.write(LCD_CH0_IOEXP_ADDR, (const char*)&addr, 1, true))
		Xtender.read(LCD_CH0_IOEXP_ADDR, (char*)IOEXP.reg, sizeof(IOEXP.reg));
#elif (MBED_CONF_LCD_BOARD == PANEL_VKLVDS_HDMI)
	hdmi_Xmitter.frequency(100000);
	hdmi_Xmitter.write(LCD_CH0_TFP410_ADDR, HDMI_Init, sizeof(HDMI_Init));
#endif
    return &LcdCfgTbl_LCD_panel;
}

void Swap_LCDFrameBuffer(DisplayBase& Display, void * frm)
{
	DisplayBase::graphics_error_t error;
	/* Change write buffer -> The new buffer will be active after 1 Vfield (if input is progressive) or 2 Vfields (if input is Interlaced) */
	//core_util_critical_section_enter();
	if(frm)
	{
		dcache_clean(((DisplayBuffer_t*)frm)[vIDX? 0 : 1].frameBUF, sizeof(((DisplayBuffer_t*)frm)[vIDX? 0 : 1].frameBUF));
		if ((error = Display.Graphics_Read_Change(used_LCDlayer, ((DisplayBuffer_t*)frm)[(vIDX = vIDX? 0 : 1)].frameBUF)) != DisplayBase::GRAPHICS_OK)
			printf("Line %d, error %d\n", __LINE__, error);
	}
	//else
	//{
	//	if(!everyX)
	//	    vfield_count = skip_frames;
    //}
	//core_util_critical_section_exit();
}

uint8_t GetLCDBUFidx(void) { return vIDX? 0 : 1; }
void LCDnotify(bool onoff, uint16_t e) { LCDNotifyID = (onoff)? ThisThread::get_id() : 0; everyXsync = e; }
#endif /* MBED_CONF_LCD_BOARD */

#if MBED_CONF_CAM_BOARD
static volatile uint8_t             fIDX = 0;
static volatile int32_t             vframe_count = 0;
static volatile osThreadId          CAMNotifyID = NULL;
static volatile uint8_t             everyXframe = 0;
DisplayBase::video_input_channel_t  used_CAMchannel;

static void Vfield_IRQ(DisplayBase::int_type_t int_type)
{
    if (vframe_count > 0)
    {
    	vframe_count--;
    }

    if(CAMNotifyID && !vframe_count)
    {
    	osSignalSet(CAMNotifyID, V_FRAME);
        if(everyXframe)
        	vframe_count = everyXframe;
    }
}

static DisplayBase::graphics_error_t camera_init(DisplayBase& Display, uint16_t cap_width, uint16_t cap_height) {
#if 0 //MBED_CONF_CAM_TYPE == CAMERA_CVBS
    DisplayBase::graphics_error_t error;
    error = Display.Graphics_Video_init(DisplayBase::INPUT_SEL_VDEC, NULL);
    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    return DisplayBase::GRAPHICS_OK;
#else
    DisplayBase::graphics_error_t error;
    DisplayBase::video_input_sel_t video_input_sel;

  #if defined(TARGET_VK_RZ_A1H)
    PinName cmos_camera_pin[] = {
        /* data pin */
    	P5_0, P5_1, P5_2, P5_3, P5_4, P5_5, P5_6, P5_7,
        /* control pin */
	    P1_5,       /* DV1_CLK   */
	    P1_6,       /* DV1_Vsync */
	    P1_7        /* DV1_Hsync */
    };
    DigitalOut pwdn(P3_7);
    DigitalOut rstb(P3_2);

    pwdn = 0;
    rstb = 0;
    ThisThread::sleep_for(10 + 1);
    rstb = 1;
    ThisThread::sleep_for(1 + 1);
  #elif defined(TARGET_VK_RZ_A1LU)
    PinName cmos_camera_pin[] = {
        /* data pin */
        P1_0, P1_1, P1_2, P1_3, P1_8, P1_9, P1_10, P1_11,
        /* control pin */
        P7_8,       /* DV0_CLK   */
        P7_9,       /* DV0_Vsync */
        P7_10       /* DV0_Hsync */
    };
    DigitalOut pwdn(P6_11);
    DigitalOut rstb(P7_11);

    pwdn = 0;
    rstb = 0;
    ThisThread::sleep_for(10 + 1);
    rstb = 1;
    ThisThread::sleep_for(1 + 1);
#elif defined(TARGET_VK_RZ_A1R3)
    PinName cmos_camera_pin[] = {
        /* data pin */
        P6_12, P6_13, P6_14, P6_15, P1_8, P1_9, P1_10, P1_11,
        /* control pin */
        P7_8,       /* DV0_CLK   */
        P7_9,       /* DV0_Vsync */
        P7_10       /* DV0_Hsync */
    };
    DigitalOut pwdn(P6_11);
    DigitalOut rstb(P7_11);

    pwdn = 0;
    rstb = 0;
    ThisThread::sleep_for(10 + 1);
    rstb = 1;
    ThisThread::sleep_for(1 + 1);
  #endif

    /* camera input port setting */
  #if CAMERA_MODULE == MODULE_VDC
    video_input_sel = DisplayBase::INPUT_SEL_EXT;
    error = Display.Graphics_Dvinput_Port_Init(cmos_camera_pin, sizeof(cmos_camera_pin)/sizeof(cmos_camera_pin[0]));
  #elif CAMERA_MODULE == MODULE_CEU
    video_input_sel = DisplayBase::INPUT_SEL_CEU;
    error = Display.Graphics_Ceu_Port_Init(cmos_camera_pin, sizeof(cmos_camera_pin)/sizeof(cmos_camera_pin[0]));
  #endif
    if( error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

  #if MBED_CONF_CAM_BOARD == CAMERA_OV7725
    OV7725_config CAM;
  #elif MBED_CONF_CAM_BOARD == CAMERA_OV7740
    OV7740_config CAM;
  #else
    #error "No camera chosen. Please add appropriate value for 'config.cam.board' in your mbed-vk-libs/components/CAM/mbed_lib.json."
  #endif

    DisplayBase::video_ext_in_config_t ext_in_config;

    CAM.Initialise();
    CAM.SetExtInConfig(&ext_in_config);
    if (cap_width != 0) {
        ext_in_config.cap_width  = cap_width;                             /* Capture width */
    }
    if (cap_height != 0) {
        ext_in_config.cap_height = cap_height;                            /* Capture heigh */
    }

    error = Display.Graphics_Video_init(video_input_sel, &ext_in_config);

    if( error != DisplayBase::GRAPHICS_OK ) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    return DisplayBase::GRAPHICS_OK;
#endif
}

void Swap_CAMFrameBuffer(DisplayBase& Display, void * frm)
{
	DisplayBase::graphics_error_t error;
	/* Change write buffer -> The new buffer will be active after 1 Vfield (if input is progressive) or 2 Vfields (if input is Interlaced) */
	//core_util_critical_section_enter();
	if(frm)
	{
		dcache_invalid(((VideoBuffer_t*)frm)[fIDX? 0 : 1].frameBUF, sizeof(((VideoBuffer_t*)frm)[fIDX? 0 : 1].frameBUF));
		if ((error = Display.Video_Write_Change(used_CAMchannel, ((VideoBuffer_t*)frm)[(fIDX = fIDX? 0 : 1)].frameBUF, CAMFRAME_BUFFER_STRIDE)) != DisplayBase::GRAPHICS_OK)
			printf("Line %d, error %d\n", __LINE__, error);
	}
	//else
	//{
	//	if(!everyXframe)
	//	    vframe_count = skip_frames;
    //}
	//core_util_critical_section_exit();
}

size_t take_jpeg(JPEG_Converter& Jcu, uint8_t* buf, int len, int width, int height, void* inbuf)
{
    size_t encode_size;
    JPEG_Converter::bitmap_buff_info_t bitmap_buff_info;
    JPEG_Converter::encode_options_t encode_options;
    bitmap_buff_info.width = width;
    bitmap_buff_info.height = height;
    bitmap_buff_info.format = JPEG_Converter::WR_RD_YCbCr422;
    bitmap_buff_info.buffer_address = inbuf;
    encode_options.encode_buff_size = len;
    encode_options.p_EncodeCallBackFunc = NULL;
    encode_options.input_swapsetting = JPEG_Converter::WR_RD_WRSWA_32_16_8BIT;

    encode_size = 0;
    dcache_invalid(buf, len);
    if (Jcu.encode(&bitmap_buff_info, buf, &encode_size, &encode_options)
            != JPEG_Converter::JPEG_CONV_OK) {
        encode_size = 0;
    }

    return encode_size;
}

uint8_t GetCAMBUFidx(void) { return fIDX? 0 : 1; }
void CAMnotify(bool onoff, uint16_t e) { CAMNotifyID = (onoff)? ThisThread::get_id() : 0; everyXframe = e; }
#endif /* MBED_CONF_CAM_BOARD */

DisplayBase::graphics_error_t EasyAttach_Init(DisplayBase& Display, uint16_t cap_width, uint16_t cap_height) {
    DisplayBase::graphics_error_t error;

#if MBED_CONF_LCD_BOARD
    const DisplayBase::lcd_config_t * lcd_config = lcd_port_init(Display);

    error = Display.Graphics_init(lcd_config);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    error = Display.Graphics_Irq_Handler_Set(DisplayBase::INT_TYPE_S0_LO_VSYNC, 0, Vsync_IRQ);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }
#endif

#if MBED_CONF_CAM_BOARD

#if (!defined(MBED_CONF_LCD_BOARD) || (MBED_CONF_LCD_BOARD == 0))
    error = Display.Graphics_init(NULL);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }
#endif

    error = Display.Graphics_Irq_Handler_Set(DisplayBase::INT_TYPE_S0_VFIELD, 0, Vfield_IRQ);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }
    else
    	return camera_init(Display, cap_width, cap_height);
#else
    return DisplayBase::GRAPHICS_OK;
#endif
}

void EasyAttach_SetTypicalBacklightVol(float typ_vol) {
#if MBED_CONF_LCD_BOARD
    if (typ_vol > MAX_BACKLIGHT_VOL) {
        voltage_adjust = 1.0f;
    } else {
        voltage_adjust = (typ_vol / MAX_BACKLIGHT_VOL);
    }
#endif
}

void EasyAttach_LcdBacklight(bool type) {
#if MBED_CONF_LCD_BOARD
    if (type) {
        EasyAttach_LcdBacklight(1.0f);
    } else {
        EasyAttach_LcdBacklight(0.0f);
    }
#endif
}

void EasyAttach_LcdBacklight(float value) {
#if MBED_CONF_LCD_BOARD
#if (MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA)
    lcd_cntrst = 1.0f - (value * voltage_adjust);
#elif (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT)
    const char DC[2] = {IOEXP_REG_IO_DATA+(offsetof( ioexp_t, regs.PWM_dc)), (char)(value*voltage_adjust*MAX_BACKLIGHT_VOL)};

    Xtender.write(LCD_CH0_IOEXP_ADDR, DC, sizeof(DC));
#endif
#endif
}

DisplayBase::graphics_error_t EasyAttach_LcdStart(DisplayBase& Display, void * p_frm, DisplayBase::graphics_layer_t layer)
{
#if MBED_CONF_LCD_BOARD
	DisplayBase::graphics_error_t error;
    DisplayBase::rect_t rect = {0,LCD_PIXEL_HEIGHT,0,LCD_PIXEL_WIDTH};
    used_LCDlayer = layer;

    error = Display.Graphics_Read_Setting(layer, p_frm, LCDFRAME_BUFFER_STRIDE, DisplayBase::GRAPHICS_FORMAT_RGB565, DisplayBase::WR_RD_WRSWA_32_16BIT, &rect);
    return (error == DisplayBase::GRAPHICS_OK)? Display.Graphics_Start(layer) : error;
#endif
    return DisplayBase::GRAPHICS_OK;
}

DisplayBase::graphics_error_t EasyAttach_CameraStart(DisplayBase& Display, void * p_frm, DisplayBase::video_input_channel_t channel) {
#if MBED_CONF_CAM_BOARD
    DisplayBase::graphics_error_t error;
    used_CAMchannel = channel;

    error = Display.Video_Write_Setting(channel, DisplayBase::COL_SYS_NTSC_358, p_frm, CAMFRAME_BUFFER_STRIDE, DisplayBase::VIDEO_FORMAT_YCBCR422, DisplayBase::WR_RD_WRSWA_32_16BIT, CAM_PIXEL_HEIGHT, CAM_PIXEL_WIDTH);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    /* Video write process start */
    error = Display.Video_Start(channel);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

  #if CAMERA_MODULE == MODULE_VDC
    /* Video write process stop */
    error = Display.Video_Stop(channel);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }

    /* Video write process start */
    error = Display.Video_Start(channel);
    if (error != DisplayBase::GRAPHICS_OK) {
        printf("Line %d, error %d\n", __LINE__, error);
        return error;
    }
  #endif
#endif
    return DisplayBase::GRAPHICS_OK;
}

void DataLOG(void * data, uint32_t l)
{
    uint8_t *p = (uint8_t *)data;

    printf(">>> DataLEN: %d <<<\r\n", l);

    for (int i = 0; i < l; i += 16)
    {
	printf("%04X:\t",i);
        for (int j = 0; ((j < 16) && ((i + j) < l)); j++)
            printf("%02X ", p[i + j]);
        printf("\t");
        for (int j = 0; ((j < 16) && ((i + j) < l)); j++)
    	    printf("%c", ((p[i + j] > 0x20) && (p[i + j] < 0x80))? p[i + j] : '.');
	printf("\r\n");
    }
}
