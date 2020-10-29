/*
 * Copyright (c) 2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "unity.h"
#include "utest.h"
#include "greentea-client/test_env.h"
#include "EasyAttach_CameraAndLCD.h"
#include "SDHIBlockDevice.hpp"
#include "FATFileSystem.h"

using namespace utest::v1;

#if !(DEVICE_SDHI && MBED_CONF_CAM_BOARD)
#error [NOT_SUPPORTED] No CAM or DEVICE_SDHI support for this target.
#else

#ifndef MBED_TEST_TIMEOUT
#define MBED_TEST_TIMEOUT 120
#endif

#define JPEG_BUFFER_SIZE  (128*1024)

#define PIC_NUM           10 // Take X pics from the CAM
//#define STR(clr)          #clr

static VideoBuffer_t VideoBuffer[2]__attribute((aligned(32)));
static uint8_t JPEGBuffer[JPEG_BUFFER_SIZE]__attribute((aligned(32)));

JPEG_Converter Jcu;
FATFileSystem sd("SD");
SDHIBlockDevice bd(MBED_CONF_SDHI_CH);
#if defined(TARGET_VK_RZ_A1H)
DisplayBase Display(DisplayBase::CH_1);
#else
DisplayBase Display;
#endif

static control_t CAM_test(const size_t call_count)
{
    FILE *fp;
    char fname[50];
    size_t JPEG_size;

    int res = bd.init();
    if(res)
        printf("Can't init SDHIBlockDevice");
    TEST_ASSERT_EQUAL(0, res);

    res = sd.mount(&bd);
    if(res)
        printf("Can't mount SDHI FATFileSystem");
    TEST_ASSERT_EQUAL(0, res);

    CAMnotify(true);

    for(int PIC = 0, IDX = 0; PIC < PIC_NUM; PIC++)
    {
        Swap_CAMFrameBuffer(Display, VideoBuffer);
        ThisThread::flags_wait_any(V_FRAME);
        IDX = GetCAMBUFidx();
        JPEG_size = take_jpeg(Jcu, JPEGBuffer, sizeof(JPEGBuffer), CAM_PIXEL_WIDTH, CAM_PIXEL_HEIGHT, VideoBuffer[IDX].frameBUF);
        snprintf(fname, sizeof(fname), "/SD/PIC_No%d.jpg", PIC);
        fp = fopen(fname, "w+");
        if(fp)
            fwrite(JPEGBuffer, 1, JPEG_size, fp);
        fclose(fp);
        printf("Taking Pic[%d|%lu]\r\n", PIC, JPEG_size);
    }

    CAMnotify(false);

    res = sd.unmount();
    if(res)
        printf("Can't unmount SDHI FATFileSystem");
    TEST_ASSERT_EQUAL(0, res);

    res = bd.deinit();
    if(res)
        printf("Can't deinit SDHIBlockDevice");
    TEST_ASSERT_EQUAL(0, res);

    TEST_ASSERT_EQUAL(0, 0);

    return CaseNext;
}

utest::v1::status_t PANEL_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(MBED_TEST_TIMEOUT, "default_auto");

    EasyAttach_Init(Display);

    EasyAttach_CameraStart(Display, VideoBuffer[0].frameBUF);

    //return greentea_test_setup_handler(number_of_cases);
    return verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("CAM test", CAM_test)
};

Specification specification(PANEL_setup, cases);

int main()
{
    return !Harness::run(specification);
}

#endif //!(DEVICE_SDHI && MBED_CONF_CAM_BOARD)
