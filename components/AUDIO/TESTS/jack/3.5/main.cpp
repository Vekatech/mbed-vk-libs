/*
 * Copyright (c) 2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "unity.h"
#include "utest.h"
#include "greentea-client/test_env.h"
#include "EasyPlayback.h"
#include "EasyDec_WavCnv2ch.h"
#include "HeapBlockDevice.h"

using namespace utest::v1;

#if ((!defined(TARGET_VK_RZ_A1LU) && !defined(TARGET_VK_RZ_A1H)) || (defined(TARGET_VK_RZ_A1H) && (MBED_CONF_LCD_BOARD != PANEL_VKLCD50RTA)))
#error [NOT_SUPPORTED] No MAX9867 CODEC / MAX9850 DAC support for this target.
#else

#ifndef MBED_TEST_TIMEOUT
#define MBED_TEST_TIMEOUT 120
#endif

#define BLOCK_SIZE 512

static EasyPlayback AudioPlayer;

extern const long Smpl_size;
extern const unsigned char Smpl[1921102];

FATFileSystem fs("FS");
HeapBlockDevice bd((2*1024*1024), BLOCK_SIZE);

static control_t WAV_test(const size_t call_count)
{
    FILE *fp;
    char fname[] = "/FS/Few_sec.wav";

    int res;
    res = fs.format(&bd);
    if(res)
        printf("Can't init FATFileSystem");
    TEST_ASSERT_EQUAL(0, res);

    res = fs.mount(&bd);
    if(res)
        printf("Can't mount FATFileSystem");
    TEST_ASSERT_EQUAL(0, res);

    fp = fopen(fname, "w");
    if(fp)
    	fwrite(Smpl, 1, Smpl_size, fp);
    fclose(fp);

	AudioPlayer.play(fname);

	res = fs.unmount();
	if(res)
	    printf("Can't unmount FATFileSystem");
	TEST_ASSERT_EQUAL(0, res);

	res = bd.deinit();
	if(res)
	    printf("Can't deinit FATFileSystem");
	TEST_ASSERT_EQUAL(0, res);

    TEST_ASSERT_EQUAL(0, 0);

    return CaseNext;
}

utest::v1::status_t CODEC_setup(const size_t number_of_cases)
{
    GREENTEA_SETUP(MBED_TEST_TIMEOUT, "default_auto");

    AudioPlayer.add_decoder<EasyDec_WavCnv2ch>(".wav");  // decoder setting
    AudioPlayer.add_decoder<EasyDec_WavCnv2ch>(".WAV");  // decoder setting
    AudioPlayer.outputVolume(0.5);  //(min:0.0 max:1.0)

    //return greentea_test_setup_handler(number_of_cases);
    return verbose_test_setup_handler(number_of_cases);
}

Case cases[] = {
    Case("WAV test", WAV_test)
};

Specification specification(CODEC_setup, cases);

int main()
{
    return !Harness::run(specification);
}

#endif // (!defined(TARGET_VK_RZ_A1LU) && !defined(TARGET_VK_RZ_A1H)) || (defined(TARGET_VK_RZ_A1H) && (MBED_CONF_LCD_BOARD != PANEL_VKLCD50RTA))
