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
* http://www.renesas.com/disclaimer*
* Copyright (C) 2017 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#ifndef MBED_AUDIO_CODEC_H
#define MBED_AUDIO_CODEC_H

#if defined(TARGET_VK_RZ_A1H)

#include "mbed.h"
#include "MAX9850_RBSP.h"

/** AUDIO_codec class
*
*/
class AUDIO_codec : public MAX9850_RBSP {
public:

    /** Create a audio codec class
     * 
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    AUDIO_codec(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16) :
      MAX9850_RBSP(I2C_SDA, I2C_SCL, P8_12, P8_13, P8_14, NC, int_level, max_write_num, max_read_num) {
    }
};

#elif defined(TARGET_VK_RZ_A1LU)

#include "mbed.h"
#include "MAX9867_RBSP.h"

/** AUDIO_codec class
*
*/
class AUDIO_codec : public MAX9867_RBSP {
public:

    /** Create a audio codec class
     * 
     * @param int_level     Interupt priority (SSIF)
     * @param max_write_num The upper limit of write buffer (SSIF)
     * @param max_read_num  The upper limit of read buffer (SSIF)
     */
    AUDIO_codec(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16) :
      MAX9867_RBSP(I2C_SDA, I2C_SCL, P6_4, P6_5, P6_6, P6_7, int_level, max_write_num, max_read_num) {
    }
};

#else

#include "mbed.h"
#include "AUDIO_RBSP.h"

class AUDIO_codec : public AUDIO_RBSP {
public:
    AUDIO_codec(uint8_t int_level = 0x80, int32_t max_write_num = 16, int32_t max_read_num = 16){}

    virtual void power(bool type = true) {}
    virtual bool format(char length) { return false; }
    virtual bool frequency(int hz) { return false; }
    virtual int write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) { return -1; }
    virtual int read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) { return -1; }
    virtual bool outputVolume(float leftVolumeOut, float rightVolumeOut) { return false; }
    virtual bool micVolume(float VolumeIn) { return false; }
};

#endif

#endif
