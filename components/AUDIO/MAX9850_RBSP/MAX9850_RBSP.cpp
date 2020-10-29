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

#include "mbed.h"
#include "MAX9850_RBSP.h"
#include "pinmap.h"

MAX9850_RBSP::MAX9850_RBSP(PinName sda, PinName scl, PinName sck, PinName ws, PinName tx, PinName rx,
    uint8_t int_level, int32_t max_write_num, int32_t max_read_num) 
       : mI2c_(sda, scl), mI2s_(sck, ws, tx, rx) {
    mAddr     = 0x20;

    // I2S Mode
    ssif_cfg.enabled                = true;
    ssif_cfg.int_level              = int_level;
    ssif_cfg.slave_mode             = false;
    ssif_cfg.sample_freq            = 44100u;
    ssif_cfg.clk_select             = SSIF_CFG_CKS_AUDIO_X1;
    ssif_cfg.multi_ch               = SSIF_CFG_MULTI_CH_1;
    ssif_cfg.data_word              = SSIF_CFG_DATA_WORD_16;
    ssif_cfg.system_word            = SSIF_CFG_SYSTEM_WORD_32;
    ssif_cfg.bclk_pol               = SSIF_CFG_FALLING;
    ssif_cfg.ws_pol                 = SSIF_CFG_WS_LOW;
    ssif_cfg.padding_pol            = SSIF_CFG_PADDING_LOW;
    ssif_cfg.serial_alignment       = SSIF_CFG_DATA_FIRST;
    ssif_cfg.parallel_alignment     = SSIF_CFG_LEFT;
    ssif_cfg.ws_delay               = SSIF_CFG_DELAY;
    ssif_cfg.noise_cancel           = SSIF_CFG_ENABLE_NOISE_CANCEL;
    ssif_cfg.tdm_mode               = SSIF_CFG_DISABLE_TDM;
    ssif_cfg.romdec_direct.mode     = SSIF_CFG_DISABLE_ROMDEC_DIRECT;
    ssif_cfg.romdec_direct.p_cbfunc = NULL;
    mI2s_.init(&ssif_cfg, max_write_num, max_read_num);

    mI2c_.frequency(150000);
    power(false);                  // Power off
    frequency(44100);              // Default sample frequency is 44.1kHz
    activateDigitalInterface_();   // The digital part of the chip is active
}

// Public Functions
bool MAX9850_RBSP::outputVolume(float leftVolumeOut, float rightVolumeOut) {
    // check values are in range
    if ((leftVolumeOut  < 0.0) || (leftVolumeOut  > 1.0)
     || (rightVolumeOut < 0.0) || (rightVolumeOut > 1.0)) {
        return false;
    }

    // convert float to encoded char
    char left  = (char)(40 - (40 * leftVolumeOut));
    char right = (char)(40 - (40 * rightVolumeOut));

    cmd[0] = 0x02;  // Playback Volume Registers
    cmd[1] = 0x40 | left;
    mI2c_.write(mAddr, cmd, 2);

    return true;
}

bool MAX9850_RBSP::micVolume(float VolumeIn) {
    // check values are in range
    if ((VolumeIn < 0.0) || (VolumeIn > 1.0)) {
        return false;
    }

    return true;
}

void MAX9850_RBSP::power(bool type) {
    cmd[0] = 0x05; // Power-Management Register
    if (type) {
        cmd[1] = 0xFD;  //SHDN=1,MCLKEN=1,CPEN[1:0]=11,HPEN=1,LNOEN=1,LNIEN=0,DACEN=1
    } else {
        cmd[1] = 0xFC;  //SHDN=1,MCLKEN=1,CPEN[1:0]=11,HPEN=1,LNOEN=1,LNIEN=0,DACEN=0
    }
    mI2c_.write(mAddr, cmd, 2);
}

bool MAX9850_RBSP::format(char length) {
    if (length != 16) {
        return false;
    }

    /*
    switch(length)
    {
    	case 18: cmd[1] = 0x1; break; // Word Length Select: 18bit Audio
    	case 20: cmd[1] = 0x2; break; // Word Length Select: 20bit Audio
    	case 24: cmd[1] = 0x3; break; // Word Length Select: 24bit Audio
    	default: cmd[1] = 0x0; break; // Word Length Select: 16bit Audio
    }

    if(cmd[1])
    {
    	cmd[0] = 0x0A; // Digqital Audio Register
    	mI2c_.write(mAddr, cmd, 2);
    }
    */

    return true;
}

bool MAX9850_RBSP::frequency(int hz) {
	/*
    uint16_t lrclk;

    switch (hz) {
        case 48000:
            lrclk = 0x624E;
            break;
        case 44100:
            lrclk = 0x5A51;
            break;
        case 32000:
            lrclk = 0x4189;
            break;
        case 24000:
            lrclk = 0x3127;
            break;
        case 16000:
            lrclk = 0x20C5;
            break;
        case 8000:
            lrclk = 0x1062;
            break;
        default:
            return false;
    }

    cmd[0] = 0x05;  // Clock Control Registers
    cmd[1] = 0x10;  // PSCLK = 00  FREQ = 0x0
    cmd[2] = (uint8_t)(lrclk >> 8);
    cmd[3] = (uint8_t)(lrclk);
    mI2c_.write(mAddr, cmd, 4);
    */
    return true;
}

// Private Functions
void MAX9850_RBSP::activateDigitalInterface_(void) {
    cmd[0] = 0x06;  // Clock
    cmd[1] = 0x04;  // IC=1 (Iclk = 1/2 Mclk; MCLK=24Mhz)
    mI2c_.write(mAddr, cmd, 2);

    cmd[0] = 0x07;  // Charge Pump
    cmd[1] = 0x49;  // SR=1,CP=9 (Slew-Rate 125ms, fCP=666.7kHz)
    mI2c_.write(mAddr, cmd, 2);

    cmd[0] = 0x0A;  // Digital Audio
    cmd[1] = 0x08;  // DLY=1 (Audio data is latched on the second rising BCLK edge after LRCLK transitions)
    mI2c_.write(mAddr, cmd, 2);

    /*
    I2C_W2(I2SDAC_B, I2SDAC_DAUDIO, 0b00001011);	// set to Slave mode, DLY, 24 bit
    I2C_W2(I2SDAC_B, I2SDAC_LRCLKL, 0b00010000);	// divide iclk by 16 * 16 (typ 44.1 or 48 kHz)
    I2C_W2(I2SDAC_B, I2SDAC_LRCLKH, 0b10000000);	// Integer mode,
    I2C_W2(I2SDAC_B, I2SDAC_VOLUME, 0b00001111);	// volume to mid-range
    I2C_W2(I2SDAC_B, I2SDAC_GENPUR, 0b00100001);	// ZDEN, GPD out, GPO=0
    I2C_W2(I2SDAC_B, I2SDAC_CHGPMP, 0b01000000);	// Slew 125 ms, CP internal
    ...
    I2C_W2(I2SDAC_B, I2SDAC_ENABLE, 0b11111101);
    */
}
