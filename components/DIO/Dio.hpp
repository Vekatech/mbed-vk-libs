#ifndef DIO_PANEL_HPP
#define DIO_PANEL_HPP

#include "mbed.h"

class Dio
{
public:
    /** Constructor method of Dio object
    */
    Dio( void );

    /** Destructor method of Tio object
    */
    ~Dio( void );

    DigitalIn  In[8] =  {P4_0,  P4_1,  P4_2,  P4_3,  P4_4,  P4_5,  P4_6,  P4_7};
    DigitalOut Out[6] = {P3_8,  P3_9,  P3_10, P3_11, P3_12, P3_13};
    DigitalOut Rly[2] = {P3_14, P3_15};
};

#endif /* DIO_PANEL_HPP */
