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
#ifndef EASY_ATTACH_CAMERA_AND_LCD_H
#define EASY_ATTACH_CAMERA_AND_LCD_H

#include "DisplayBace.h"
#include "JPEG_Converter.h"
#if MBED_CONF_LCD_BOARD
#include "PanelCfg.h"
#endif

//Video buffer components
typedef union
{
	struct
	{
		uint16_t B : 5;
		uint16_t G : 6;
		uint16_t R : 5;
	} CLR;
	uint16_t 	data;
} RGB_pixel_t;

typedef union
{
	struct
	{
		uint8_t Y0;	//Y
		uint8_t U;	//Cb
		uint8_t Y1;	//Y
		uint8_t V;	//Cr
	} CMP;
	uint8_t 	dat[4];
	uint16_t	luma[2];
	uint32_t 	chroma;
	uint32_t    data;
} YUY2_macropixel_t;

#define VIDEO_FRAME(name, w, h, p)				\
union											\
{												\
	uint8_t 			frameBUF[w*h*p];		\
	RGB_pixel_t			RGB[w*h];               \
	RGB_pixel_t         RGB_2D[h][w];           \
	YUY2_macropixel_t	YUY2[(w*h)/2];		    \
	YUY2_macropixel_t	YUY2_2D[h][w/2];	    \
	uint16_t			lumaPLANE[w*h];			\
	uint16_t			lumaPLANE_2D[h][w];		\
	uint32_t			chromaPLANE[(w*h)/2];	\
	uint32_t			chromaPLANE_2D[h][w/2];	\
} name

// camera-type
#define CAMERA_NONE                 (0)
#define CAMERA_OV7725               (1)
#define CAMERA_OV7740               (2)

// camera module
#define MODULE_VDC                  0
#define MODULE_CEU                  1

// camera
#if MBED_CONF_CAM_BOARD
//pixel size in bytes
  #define CAMPIX_DATA_SIZE_BYTES    2u
//cam frame buffer line size
  #define CAMFRAME_BUFFER_STRIDE	(((CAM_PIXEL_WIDTH * CAMPIX_DATA_SIZE_BYTES) + 31u) & ~31u)
//video frame sync event flag
  #define V_FRAME			   		(1UL << 1)

  #define CAMERA_MODULE             MODULE_VDC

  #if (defined(TARGET_VK_RZ_A1H) || defined(TARGET_VK_RZ_A1LU) || defined(TARGET_VK_RZ_A1R3))
    #if MBED_CONF_CAM_BOARD == CAMERA_OV7725
      #include "OV7725_config.h"
    #elif MBED_CONF_CAM_BOARD == CAMERA_OV7740
      #include "OV7740_config.h"
    #endif
  #else
    #error "MBED_CONF_CAM_BOARD is not supported in this target."
  #endif

  typedef  VIDEO_FRAME(VideoBuffer_t, CAM_PIXEL_WIDTH, CAM_PIXEL_HEIGHT, CAMPIX_DATA_SIZE_BYTES);

  extern uint8_t GetCAMBUFidx(void);
  extern void CAMnotify(bool onoff, uint16_t e = 0);
  extern void Swap_CAMFrameBuffer(DisplayBase& Display, void * frm);
  extern size_t take_jpeg(JPEG_Converter& Jcu, uint8_t* buf, int len, int width, int height, void* inbuf);
#endif

// lcd
#if MBED_CONF_LCD_BOARD
//pixel size in bytes
  #define LCDPIX_DATA_SIZE_BYTES    2u
//lcd frame buffer line size
  #define LCDFRAME_BUFFER_STRIDE	(((LCD_PIXEL_WIDTH * LCDPIX_DATA_SIZE_BYTES) + 31u) & ~31u)
//vertical sync event flag
  #define V_SYNC			   		(1UL << 0)

  #if ((MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA) || (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT) || (MBED_CONF_LCD_BOARD == PANEL_VKLVDS_HDMI))

    #define MAX_BACKLIGHT_VOL       (100.0f)
    #ifndef TYPICAL_BACKLIGHT_VOL
      #define TYPICAL_BACKLIGHT_VOL ((float)LCD_CH0_INITIAL_DC)
    #endif

    typedef  VIDEO_FRAME(DisplayBuffer_t, LCD_PIXEL_WIDTH, LCD_PIXEL_HEIGHT, LCDPIX_DATA_SIZE_BYTES);

    extern uint8_t GetLCDBUFidx(void);
    extern void LCDnotify(bool onoff, uint16_t e = 0);
    extern void Swap_LCDFrameBuffer(DisplayBase& Display, void * frm);
  #else
    #error "No lcd chosen. Please add appropriate value for 'config.lcd.board' in your mbed-vk-libs/components/LCD/mbed_lib.json."
  #endif
#endif

extern DisplayBase::graphics_error_t EasyAttach_Init(
    DisplayBase& Display,
    uint16_t cap_width = 0,
    uint16_t cap_height = 0
);

extern DisplayBase::graphics_error_t EasyAttach_LcdStart(
    DisplayBase& Display, void * p_frm,
	DisplayBase::graphics_layer_t layer = DisplayBase::GRAPHICS_LAYER_0
);

extern DisplayBase::graphics_error_t EasyAttach_CameraStart(
    DisplayBase& Display, void * p_frm,
    DisplayBase::video_input_channel_t channel = DisplayBase::VIDEO_INPUT_CHANNEL_0
);

extern void EasyAttach_SetTypicalBacklightVol(float typ_vol);

extern void EasyAttach_LcdBacklight(bool type = true);

extern void EasyAttach_LcdBacklight(float value);

void DataLOG(void * data, uint32_t l);

#endif
