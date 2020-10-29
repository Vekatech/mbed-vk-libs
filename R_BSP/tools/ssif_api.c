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
* Copyright (C) 2015 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/

#include "ssif_api.h"

#if defined(TARGET_RZ_A1XX)
#if defined(TARGET_VK_RZ_A1H) /* mbed */
static const PinMap PinMap_SSIF_SCK[] = {
//   pin      ch     func
    {P4_4   , 0    , 5},
    {P8_12  , 4    , 8},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_WS[] = {
//   pin      ch     func
    {P4_5   , 0    , 5},
    {P8_13  , 4    , 8},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_TxD[] = {
//   pin      ch     func
    {P4_7   , 0    , 5},
    {P8_14  , 4    , 8},  /* SSIDATA4 */
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_RxD[] = {
//   pin      ch     func
    {P4_6   , 0    , 5},
    {P8_14  , 4    , 8},  /* SSIDATA4 */
    {NC     , NC   , 0}
};

static const PinMap PinMap_SPDIF_AUDIO_CLK[] = {
    {NC,    NC,     0}
};

#elif defined(TARGET_VK_RZ_A1LU)
static const PinMap PinMap_SSIF_SCK[] = {
//   pin      ch     func
    {P6_4   , 3    , 3},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_WS[] = {
//   pin      ch     func
    {P6_5   , 3    , 3},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_TxD[] = {
//   pin      ch     func
    {P6_6   , 3    , 3},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_RxD[] = {
//   pin      ch     func
    {P6_7   , 3    , 3},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SPDIF_AUDIO_CLK[] = {
    {NC,    NC,     0}
};
#else
static const PinMap PinMap_SSIF_SCK[] = {
//   pin      ch     func
    {P2_7   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_WS[] = {
//   pin      ch     func
    {P2_9   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_TxD[] = {
//   pin      ch     func
    {P2_8   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SSIF_RxD[] = {
//   pin      ch     func
    {P2_6   , 3    , 2},
    {NC     , NC   , 0}
};

static const PinMap PinMap_SPDIF_AUDIO_CLK[] = {
    {NC,    NC,     0}
};

#endif

static void ssif_power_enable(uint32_t ssif_ch) {
    volatile uint8_t dummy;

    switch (ssif_ch) {
        case 0:
            CPGSTBCR11 &= ~(0x20);
            break;
        case 1:
            CPGSTBCR11 &= ~(0x10);
            break;
        case 2:
            CPGSTBCR11 &= ~(0x08);
            break;
        case 3:
            CPGSTBCR11 &= ~(0x04);
            break;
#if (defined(TARGET_RZA1H) || defined(TARGET_VK_RZ_A1H)) /* mbed */
        case 4:
            CPGSTBCR11 &= ~(0x02);
            break;
        case 5:
            CPGSTBCR11 &= ~(0x01);
            break;
#endif
    }
    dummy = CPGSTBCR11;
    (void)dummy;
}

#endif

int32_t ssif_init(PinName sck, PinName ws, PinName tx, PinName rx, PinName audio_clk) {
    /* determine the ssif to use */
    uint32_t ssif_sck = pinmap_peripheral(sck, PinMap_SSIF_SCK);
    uint32_t ssif_ws  = pinmap_peripheral(ws,  PinMap_SSIF_WS);
    uint32_t ssif_tx  = pinmap_peripheral(tx,  PinMap_SSIF_TxD);
    uint32_t ssif_rx  = pinmap_peripheral(rx,  PinMap_SSIF_RxD);
    uint32_t ssif_ch  = pinmap_merge(ssif_tx, ssif_rx);

    if ((ssif_ch == ssif_sck) && (ssif_ch == ssif_ws)) {
        ssif_power_enable(ssif_ch);
        pinmap_pinout(sck, PinMap_SSIF_SCK);
        pinmap_pinout(ws,  PinMap_SSIF_WS);
        pinmap_pinout(tx,  PinMap_SSIF_TxD);
        pinmap_pinout(rx,  PinMap_SSIF_RxD);
        if ((int32_t)audio_clk != NC) {
            pinmap_pinout(audio_clk, PinMap_SPDIF_AUDIO_CLK);
        }
    } else {
        ssif_ch = (uint32_t)NC;
    }

    return (int32_t)ssif_ch;
}

