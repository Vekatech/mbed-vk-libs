
#ifndef OV7740_CONFIG_H
#define OV7740_CONFIG_H

#include "mbed.h"
#include "camera_config.h"

#define CAM_PIXEL_WIDTH     640
#define CAM_PIXEL_HEIGHT    480

#define OV7740_I2C_SLAVE_ADDR	(0x42)

#define OV7740REG_GAIN		(0x00)
#define OV7740REG_BGAIN		(0x01)
#define OV7740REG_RGAIN		(0x02)
#define OV7740REG_GGAIN		(0x03)
#define OV7740REG_BAVG		(0x05)
#define OV7740REG_GAVG		(0x06)
#define OV7740REG_RAVG		(0x07)
#define OV7740REG_PIDH		(0x0A)
#define OV7740REG_PIDL		(0x0B)
#define OV7740REG_REG0C		(0x0C)
#define OV7740REG_REG0E		(0x0E)
#define OV7740REG_HAEC		(0x0F)
#define OV7740REG_AEC		(0x10)
#define OV7740REG_CLK		(0x11)
#define OV7740REG_REG12		(0x12)
#define OV7740REG_REG13		(0x13)
#define OV7740REG_REG14		(0x14)
#define OV7740REG_REG15		(0x15)
#define OV7740REG_REG16		(0x16)
#define OV7740REG_AHSTART	(0x17)
#define OV7740REG_AHSIZE	(0x18)
#define OV7740REG_AVSTART	(0x19)
#define OV7740REG_AVSIZE	(0x1A)
#define OV7740REG_PSHFT 	(0x1B)
#define OV7740REG_MIDH  	(0x1C)
#define OV7740REG_MIDL  	(0x1D)
#define OV7740REG_REG1E		(0x1E)
#define OV7740REG_REG1F		(0x1F)
#define OV7740REG_WPT   	(0x24)
#define OV7740REG_BPT   	(0x25)
#define OV7740REG_VPT		(0x26)
#define OV7740REG_REG27		(0x27)
#define OV7740REG_REG28		(0x28)
#define OV7740REG_REG29		(0x29)
#define OV7740REG_REG2A		(0x2A)
#define OV7740REG_REG2B		(0x2B)
#define OV7740REG_REG2C		(0x2C)
#define OV7740REG_REG2D		(0x2D)
#define OV7740REG_REG2E		(0x2E)
#define OV7740REG_YAVG		(0x2F)
#define OV7740REG_REG30		(0x30)
#define OV7740REG_HOUTSIZE	(0x31)
#define OV7740REG_VOUTSIZE	(0x32)
#define OV7740REG_REG34		(0x34)
#define OV7740REG_REG38		(0x38)
#define OV7740REG_REG50		(0x50)
#define OV7740REG_REG51		(0x51)
#define OV7740REG_REG52		(0x52)
#define OV7740REG_REG56		(0x56)

class OV7740_config : public camera_config {
	uint16_t product_id;
public:

	uint16_t ID(void) { return product_id; }

    /** Initialise
     *
     * @return true = success, false = failure
     */
    virtual bool Initialise() {
        /* OV7740 camera input config */
        const char OV7740_InitRegTable[][2] = {
            {0x04, 0x60},                        // "reserved"
            {OV7740REG_REG0C,	    0x12},       // REG0C  Y<->UV swap
            {0x0d, 0x34},                        // "reserved"
            {OV7740REG_CLK,         0x01},       // CLK
            {OV7740REG_REG12,       0x00},       // REG12 CC656 & RGB mode OFF
       	    {OV7740REG_REG13,       0xff},       // REG13
            {OV7740REG_REG14,       0x38},       // REG14
            {OV7740REG_AHSTART,     0x24},       // AHSTART
            {OV7740REG_AHSIZE,      0xa0},       // AHSIZE
            {OV7740REG_AVSTART,     0x03},       // AVSTART
            {OV7740REG_AVSIZE,      0xf0},       // AVSIZE
            {OV7740REG_PSHFT,       0x85},       // PSHFT
            {OV7740REG_REG1E,       0x13},       // REG1E
            {0x20, 0x00},                        // "reserved"
            {0x21, 0x23},                        // "reserved"
            {0x22, 0x03},                        // "reserved"
            {OV7740REG_WPT,         0x3c},       // WPT
            {OV7740REG_BPT,         0x30},       // BPT
            {OV7740REG_VPT,         0x72},       // VPT
            {OV7740REG_REG27,       0x80},       // REG27
            {OV7740REG_REG28,       0x48},       // REG28
            {OV7740REG_REG29,       0x17},       // REG29
            {OV7740REG_REG2B,       0xf8},       // REG2B
            {OV7740REG_REG2C,       0x01},       // REG2C
            {OV7740REG_HOUTSIZE,    0xA0},       // HOUTSIZE
            {OV7740REG_VOUTSIZE,    0xF0},       // VOUTSIZE
            {0x33, 0xc4},                        // "reserved"
            {0x36, 0x3f},                        // "reserved"
            {OV7740REG_REG38,       0x11},       // REG38 - see OV7740 data sheet
            {0x3a, 0xb4},                        // "reserved"
            {0x3d, 0x0f},                        // "reserved"
            {0x3e, 0x82},                        // "reserved"
            {0x3f, 0x40},                        // "reserved"
            {0x40, 0x7f},                        // "reserved"
            {0x41, 0x6a},                        // "reserved"
            {0x42, 0x29},                        // "reserved"
            {0x44, 0xe5},                        // "reserved"
            {0x45, 0x41},                        // "reserved"
            {0x47, 0x42},                        // "reserved"
            {0x48, 0x00},                        // "reserved"
            {0x49, 0x61},                        // "reserved"
            {0x4a, 0xa1},                        // "reserved"
            {0x4b, 0x46},                        // "reserved"
            {0x4c, 0x18},                        // "reserved"
            {0x4d, 0x50},                        // "reserved"
            {0x4e, 0x13},                        // "reserved"
            {OV7740REG_REG50,       0x97},       // REG50
            {OV7740REG_REG51,       0x7e},       // REG51
            {OV7740REG_REG52,       0x00},       // REG52
            {0x53, 0x00},                        // "reserved"
            {0x55, 0x42},                        // "reserved"
            {OV7740REG_REG56,       0x55},       // REG56
            {0x57,                  0xff},       // REG57
            {0x58,                  0xff},       // REG58
            {0x59,                  0xff},       // REG59
            {0x5a, 0x24},                        // "reserved"
            {0x5b, 0x1f},                        // "reserved"
            {0x5c, 0x88},                        // "reserved"
            {0x5d, 0x60},                        // "reserved"
            {0x5f, 0x04},                        // "reserved"
            {0x64, 0x00},                        // "reserved"
            {0x67,                  0x88},       // REG67
            {0x68, 0x1a},                        // "reserved"
            {0x69,                  0x08},       // REG69
            {0x70, 0x00},                        // "reserved"
            {0x71, 0x34},                        // "reserved"
            {0x74, 0x28},                        // "reserved"
            {0x75, 0x98},                        // "reserved"
            {0x76, 0x00},                        // "reserved"
            {0x77, 0x08},                        // "reserved"
            {0x78, 0x01},                        // "reserved"
            {0x79, 0xc2},                        // "reserved"
            {0x7a, 0x9c},                        // "reserved"
            {0x7b, 0x40},                        // "reserved"
            {0x7c, 0x0c},                        // "reserved"
            {0x7d, 0x02},                        // "reserved"
            {0x80,                  0x7f},       // ISP CTRL00
            {0x81,                  0x3f},       // ISP CTRL01
            {0x82,                  0x32},       // ISP CTRL02
            {0x83,                  0x03},       // ISP CTRL03
            {0x84,                  0x70},       // REG84 - see OV7740 data sheet
            {0x85,                  0x00},       // AGC OFFSET
            {0x86,                  0x03},       // AGC BASE1
            {0x87,                  0x01},       // AGC BASE2
            {0x88,                  0x05},       // AGC CTRL
            {0x89,                  0x30},       // LENC CTRL
            {0x8d,                  0x40},       // LENC RED A1
            {0x8e,                  0x00},       // LENC RED B1
            {0x8f,                  0x33},       // LENC RED AB2
            {0x93,                  0x28},       // LENC GREEN A1
            {0x94,                  0x20},       // LENC GREEN B1
            {0x95,                  0x33},       // LENC GREEN AB2
            {0x99,                  0x30},       // LENC BLUE A1
            {0x9a,                  0x14},       // LENC BLUE B1
            {0x9b,                  0x33},       // LENC BLUE AB2
            {0x9c,                  0x08},       // GMA YST01
            {0x9d,                  0x12},       // GMA YST02
            {0x9e,                  0x23},       // GMA YST03
            {0x9f,                  0x45},       // GMA YST04
            {0xa0,                  0x55},       // GMA YST05
            {0xa1,                  0x64},       // GMA YST06
            {0xa2,                  0x72},       // GMA YST07
            {0xa3,                  0x7f},       // GMA YST08
            {0xa4,                  0x8b},       // GMA YST09
            {0xa5,                  0x95},       // GMA YST10
            {0xa6,                  0xa7},       // GMA YST11
            {0xa7,                  0xb5},       // GMA YST12
            {0xa8,                  0xcb},       // GMA YST13
            {0xa9,                  0xdd},       // GMA YST14
            {0xaa,                  0xec},       // GMA YST15
            {0xab,                  0x1a},       // GMA YSLP
            {0xac, 0x6e},                        // "reserved"
            {0xbe, 0xff},                        // "reserved"
            {0xbf, 0x00},                        // "reserved"
            {0xce,                  0x78},       // CMX M1
            {0xcf,                  0x6e},       // CMX M2
            {0xd0,                  0x0a},       // CMX M3
            {0xd1,                  0x0c},       // CMX M4
            {0xd2,                  0x84},       // CMX M5
            {0xd3,                  0x90},       // CMX M6
            {0xd4,                  0x1e},       // CMX CTRL
            {0xd5,                  0x10},       // SCALE SMTH CTRL
            {0xda,                  0x04},       // SDE CTRL
            {0xe9,                  0x00},       // YAVG BLK THRE
            {0xec,                  0x02},       // REGEC
            {0xda,                  0x00},       // SDE CTRL
        };
        const char sw_reset_cmd[2] = {OV7740REG_REG12, 0x80};
        int ret;
        I2C mI2c_(CAM_SDA, CAM_SCL);
        mI2c_.frequency(150000);

        /*
        sw_reset_cmd[0] = OV7740REG_PIDH;

        if((ret = mI2c_.write(OV7725_I2C_SLAVE_ADDR, sw_reset_cmd, 1, true)) != 0)
        	printf("CAM ADDR ERR [%d]\r\n", ret);
        else
        	if((ret = mI2c_.read(OV7725_I2C_SLAVE_ADDR, sw_reset_cmd, 1)) != 0) //sizeof(sw_reset_cmd)
        	    printf("READ CAM ID ERR [%d] [%#x|%#x]\r\n", ret, sw_reset_cmd[0], sw_reset_cmd[1]);
        	else
        	    product_id = (sw_reset_cmd[0]<<8) | sw_reset_cmd[1];

        sw_reset_cmd[0] = 0x12;
		sw_reset_cmd[1] = 0x80;
        */

        if (mI2c_.write(OV7740_I2C_SLAVE_ADDR, sw_reset_cmd, sizeof(sw_reset_cmd)) != 0) {
            return false;
        }
        ThisThread::sleep_for(1);

        for (uint32_t i = 0; i < (sizeof(OV7740_InitRegTable)/sizeof(OV7740_InitRegTable[0])) ; i++) {
            ret = mI2c_.write(OV7740_I2C_SLAVE_ADDR, OV7740_InitRegTable[i], sizeof(OV7740_InitRegTable[i])/sizeof(OV7740_InitRegTable[i][0]));
            if (ret != 0) {
                return false;
            }
        }

        return true;
    }

    virtual void SetExtInConfig(DisplayBase::video_ext_in_config_t * p_cfg) {
        p_cfg->inp_format     = DisplayBase::VIDEO_EXTIN_FORMAT_BT601; /* BT601 8bit YCbCr format */
        p_cfg->inp_pxd_edge   = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing data          */
        p_cfg->inp_vs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Vsync signals */
        p_cfg->inp_hs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Hsync signals */
        p_cfg->inp_endian_on  = DisplayBase::OFF;                      /* External input bit endian change on/off       */
        p_cfg->inp_swap_on    = DisplayBase::OFF;                      /* External input B/R signal swap on/off         */
        p_cfg->inp_vs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_VSYNC inversion control     */
        p_cfg->inp_hs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_HSYNC inversion control     */
        p_cfg->inp_f525_625   = DisplayBase::EXTIN_LINE_525;           /* Number of lines for BT.656 external input */
        p_cfg->inp_h_pos      = DisplayBase::EXTIN_H_POS_YCBYCR;       /* Y/Cb/Y/Cr data string start timing to Hsync reference */
        p_cfg->cap_vs_pos     = 4+15;                                  /* Capture start position from Vsync */
        p_cfg->cap_hs_pos     = 140;                                   /* Capture start position form Hsync */
        p_cfg->cap_width      = CAM_PIXEL_WIDTH;                       /* Capture width Max */
        p_cfg->cap_height     = CAM_PIXEL_HEIGHT;                      /* Capture height Max */
    }

    /** Exposure and Gain Setting
     *
     * @param[in]      bAuto             : Automatic adjustment ON/OFF(AEC/AGC)
     * @param[in]      usManualExposure  : Exposure time at automatic adjustment OFF  (number of lines)
     * @param[in]      usManualGain      : Gain at automatic adjustment OFF �i0x00-0xFF)
     * @return true = success, false = failure
     */
    static bool SetExposure(bool bAuto, uint16_t usManualExposure, uint8_t usManualGain) {
        int ret;
        char cmd[2];
        I2C mI2c_(CAM_SDA, CAM_SCL);
        mI2c_.frequency(150000);

        /* REG13(AGC Enable/AEC Enable) */
        cmd[0] = OV7740REG_REG13;
        ret = mI2c_.write(OV7740_I2C_SLAVE_ADDR, &cmd[0], 1);
        if (ret != 0) {
            return false;
        }
        ret = mI2c_.read(OV7740_I2C_SLAVE_ADDR, &cmd[1], 1);
        if (ret != 0) {
            return false;
        }

        cmd[0] = OV7740REG_REG13;
        if (bAuto) {
            cmd[1] |= (uint8_t)0x05;
        } else {
            cmd[1] &= (uint8_t)~0x05;
        }
        ret = mI2c_.write(OV7740_I2C_SLAVE_ADDR, &cmd[0], 2);
        if (ret != 0) {
            return false;
        }

        if (!bAuto) {
            /* HAEC/AEC(exposure) */
            cmd[0] = OV7740REG_HAEC;
            cmd[1] = (uint8_t)((usManualExposure & 0xFF00) >> 8);
            ret = mI2c_.write(OV7740_I2C_SLAVE_ADDR, &cmd[0], 2);
            if (ret != 0) {
                return false;
            }

            cmd[0] = OV7740REG_AEC;
            cmd[1] = (uint8_t)(usManualExposure & 0x00FF);
            ret = mI2c_.write(OV7740_I2C_SLAVE_ADDR, &cmd[0], 2);
            if (ret != 0) {
                return false;
            }

            /* GAIN */
            cmd[0] = OV7740REG_GAIN;
            cmd[1] = usManualGain;
            ret = mI2c_.write(OV7740_I2C_SLAVE_ADDR, &cmd[0], 2);
            if (ret != 0) {
                return false;
            }
        }

        return true;
    }
};

#endif

