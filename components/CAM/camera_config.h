
#ifndef CAMERA_CONFIG_H
#define CAMERA_CONFIG_H

#include "mbed.h"
#include "DisplayBace.h"

#if	defined(TARGET_VK_RZ_A1H)
#define CAM_SCL 	P1_2
#define CAM_SDA		P1_3
#elif   defined(TARGET_VK_RZ_A1LU)
#define CAM_SCL 	P1_6
#define CAM_SDA		P1_7
#else
#define CAM_SCL 	I2C_SCL
#define CAM_SDA		I2C_SDA
#endif

class camera_config {

public:

    /** Initialise
     *
     * @return true = success, false = failure
     */
    virtual bool Initialise() = 0;

    virtual void SetExtInConfig(DisplayBase::video_ext_in_config_t * p_cfg) = 0;

};

#endif

