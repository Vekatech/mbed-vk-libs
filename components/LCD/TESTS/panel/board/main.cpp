/*
 * Copyright (c) 2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "unity.h"
#include "utest.h"
#include "greentea-client/test_env.h"
#include "EasyAttach_CameraAndLCD.h"
#include "Touch.hpp"

using namespace utest::v1;
using namespace Vekatech;

#ifndef MBED_CONF_LCD_BOARD
#error [NOT_SUPPORTED] No LCD support for this target.
#elif (MBED_CONF_LCD_BOARD == MBED_CONF_LCD_PANEL_NONE)
#error [NOT_SUPPORTED] No LCD support for this target.
#else


#ifndef MBED_TEST_TIMEOUT
#define MBED_TEST_TIMEOUT 120
#endif

#define RED               0xF800
#define GREEN             0x07E0
#define BLUE              0x001F
#define CYAN              0x07FF
#define MAGENTA           0xF81F
#define YELLOW            0xFFE0
#define BLACK             0x0000
#define WHITE             0xFFFF

#define INSPECT_CLR_TIME  5 // Wait X sec to inspect for abnormal pixels
#define STR(clr)          #clr

#define WAIT_DRAW_TIME    30 // Wait X sec for drawing

static uint16_t TestCOLORS[] = {RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, BLACK, WHITE};
static draw_data_t Drawing[1000];

#if defined(__ICCARM__)
#pragma data_alignment=32
static DisplayBuffer_t DisplayBuffer[2];
#else
static DisplayBuffer_t DisplayBuffer[2]__attribute((aligned(32)));
#endif

DisplayBase Display;

static control_t CLR_test(const size_t call_count)
{
    LCDnotify(true, 60*INSPECT_CLR_TIME);

    for(uint32_t CLR = 0, IDX = 0; CLR < (sizeof(TestCOLORS)/sizeof(TestCOLORS[0])); CLR++)
    {
        ThisThread::flags_wait_any(V_SYNC);
        IDX = GetLCDBUFidx();
        for(uint32_t PIX = 0; PIX < (sizeof(DisplayBuffer[IDX].RGB)/sizeof(DisplayBuffer[IDX].RGB[0])); PIX++)
            DisplayBuffer[IDX].RGB[PIX].data = TestCOLORS[CLR];
        Swap_LCDFrameBuffer(Display, DisplayBuffer);
        switch(TestCOLORS[CLR])
        {
            case RED:       printf("Show: %s\r\n", STR(RED));       break;
            case GREEN:     printf("Show: %s\r\n", STR(GREEN));     break;
            case BLUE:      printf("Show: %s\r\n", STR(BLUE));      break;
            case CYAN:      printf("Show: %s\r\n", STR(CYAN));      break;
            case MAGENTA:   printf("Show: %s\r\n", STR(MAGENTA));   break;
            case YELLOW:    printf("Show: %s\r\n", STR(YELLOW));    break;
            case BLACK:     printf("Show: %s\r\n", STR(BLACK));     break;
            case WHITE:     printf("Show: %s\r\n", STR(WHITE));     break;
            default:                                                break;
        }
    }

    LCDnotify(false);

    TEST_ASSERT_EQUAL(0, 0);

    return CaseNext;
}

#if ((MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA) || (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT))

static control_t BRIGHT_test(const size_t call_count)
{
    float bright_lvl = TYPICAL_BACKLIGHT_VOL;

    EasyAttach_SetTypicalBacklightVol(MAX_BACKLIGHT_VOL);

    LCDnotify(true, 6);

    while(bright_lvl > 0.0f)
    {
        bright_lvl--;
        printf("%c: %d %%\r\n",0xF, (int)bright_lvl);
        ThisThread::flags_wait_any(V_SYNC);
        EasyAttach_LcdBacklight(bright_lvl/MAX_BACKLIGHT_VOL);
    }

    while(bright_lvl < TYPICAL_BACKLIGHT_VOL)
    {
        bright_lvl++;
        printf("%c: %d %%\r\n",0xF, (int)bright_lvl);
        ThisThread::flags_wait_any(V_SYNC);
        EasyAttach_LcdBacklight(bright_lvl/MAX_BACKLIGHT_VOL);
    }

    LCDnotify(false);

    EasyAttach_SetTypicalBacklightVol(TYPICAL_BACKLIGHT_VOL);

    TEST_ASSERT_EQUAL(0, 0);

    return CaseNext;
}

static control_t TOUCH_test(const size_t call_count)
{
	uint32_t flg, IDX = 0, drw = 0, redrw = 0, ms = WAIT_DRAW_TIME*1000;
	uint64_t stamp = Kernel::get_ms_count(), wait_time_passed = 0;

	Touch STMPE811;

	STMPE811.Init();
	STMPE811.notify(true);
	LCDnotify(true);

	while(ms)
	{
		flg = ThisThread::flags_wait_any_for(V_SYNC | NEW_TOUCH_DATA, ms);

		switch(flg)
		{
			case NEW_TOUCH_DATA:
				drw = STMPE811.Process_data(Drawing, sizeof(Drawing)/sizeof(Drawing[0]));
			break;

			case V_SYNC:
				if(drw != redrw)
				{
					IDX = GetLCDBUFidx();
					for(uint32_t PIX = 0; PIX < (sizeof(DisplayBuffer[IDX].RGB)/sizeof(DisplayBuffer[IDX].RGB[0])); PIX++)
						DisplayBuffer[IDX].RGB[PIX].data = WHITE;
					for(uint32_t i = 0; i < drw; i++)
						DisplayBuffer[IDX].RGB_2D[Drawing[i].axis.y][Drawing[i].axis.x].data = BLACK;
					Swap_LCDFrameBuffer(Display, DisplayBuffer);
					redrw = drw;
				}
			break;

			case (V_SYNC | NEW_TOUCH_DATA):
				printf("Multiple EVENTs\r\n");
			break;

			default: break;
		}

		if(ms != osWaitForever)
		{
			wait_time_passed = Kernel::get_ms_count() - stamp;
			stamp += wait_time_passed;
			ms = (wait_time_passed < ms)? ms - wait_time_passed : 0;
		}
	}

	printf("Total collected TOUCH points: %u\r\n", (unsigned int)drw);

	LCDnotify(false);
	STMPE811.notify(false);

    TEST_ASSERT_EQUAL(0, 0);

    return CaseNext;
}
#endif

utest::v1::status_t PANEL_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(MBED_TEST_TIMEOUT, "default_auto");

    EasyAttach_Init(Display);
    EasyAttach_LcdStart(Display, DisplayBuffer[0].frameBUF);
    EasyAttach_LcdBacklight(true);

    //return greentea_test_setup_handler(number_of_cases);
    return verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("COLOR test", CLR_test)
#if ((MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA) || (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT))
    ,Case("BRIGHT test", BRIGHT_test)
    ,Case("TOUCH test", TOUCH_test)
#endif
};

Specification specification(PANEL_setup, cases);

int main()
{
    return !Harness::run(specification);
}

#endif // MBED_CONF_LCD_BOARD