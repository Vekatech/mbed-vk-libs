#include "Touch.hpp"



namespace Vekatech {

#if MBED_CONF_LCD_BOARD && ((MBED_CONF_LCD_BOARD == PANEL_VKLCD50RTA) || (MBED_CONF_LCD_BOARD == PANEL_VKLCD70RT))

#define DISABLED	0
#define LAST_SAMPLE	1
#define ALL_SAMPLES	2

//Debug is disabled by default
#define TOUCH_DBG	DISABLED
//#define TOUCH_DBG ALL_SAMPLES

#ifndef FUNCTION_NAME
//#define FUNCTION_NAME ""
//#define FUNCTION_NAME __func__
//#define FUNCTION_NAME __FUNCTION__
#define FUNCTION_NAME __PRETTY_FUNCTION__
#endif

#if (TOUCH_DBG > DISABLED)
#ifdef MORE_DETAILS
	#define DBG(x, ...) printf("[DBG: %s:%s:%d] " x "\r\n", __FILE__, FUNCTION_NAME, __LINE__, ##__VA_ARGS__)
#else
	#define DBG(x, ...) printf("[DBG:] " x "\r\n", ##__VA_ARGS__)
#endif
#else
	#define DBG(x, ...)
#endif

#if (TOUCH_DBG > LAST_SAMPLE)
#ifdef MORE_DETAILS
	#define DBG_MORE(x, ...) printf("[DBG_MORE: %s:%s:%d] " x "\r\n", __FILE__, FUNCTION_NAME, __LINE__,  ##__VA_ARGS__)
#else
	#define DBG_MORE(x, ...) printf("[DBG_MORE:] " x "\r\n", ##__VA_ARGS__)
#endif
#else
	#define DBG_MORE(x, ...)
#endif

#if (TOUCH_DBG > ALL_SAMPLES)
#ifdef MORE_DETAILS
	#define DBG_ALL(x, ...) printf("[DBG_ALL: %s:%s:%d] " x "\r\n", __FILE__, FUNCTION_NAME, __LINE__,  ##__VA_ARGS__)
#else
	#define DBG_ALL(x, ...) printf("[DBG_ALL:] " x "\r\n", ##__VA_ARGS__)
#endif
#else
	#define DBG_ALL(x, ...)
#endif



const touch_config_t  STMPE811_cfg = {
	"STMPE811", MBED_CONF_LCD_BOARD, RESISTIVE,
    {I_2_C, TPIIC_SDA, TPIIC_SCL, NC, NC, NC, NC, 100000},
    {INT_ON_EDGE, FALLING_OR_ACTIVE_LO, TPIRQ_PIN}
};

osThreadId    		 NotifyID = NULL;

/**************************************************************************//**
 * @brief       Constructor of the Touch class
 * @param[in]   pointer to Config structure (touch_config_t)
******************************************************************************/
//SPI(tp_cfg->interface.mosi, tp_cfg->interface.miso, tp_cfg->interface.sclk, tp_cfg->interface.ssel),
Touch::Touch( const touch_config_t * tp_cfg ) : I2C(tp_cfg->interface.sda, tp_cfg->interface.scl), InterruptIn(tp_cfg->activity_irq.pin)
{
	if(tp_cfg == NULL)
		touch_cfg = &STMPE811_cfg;
	else
		touch_cfg = tp_cfg;

	x = y = z = 0;	  adc_x = adc_y = adc_z = 0;
	xyz_data = screen_data; adc_data = raw_data;
	last_xyz_idx = FIFO_DEPTH-1;

	dot_thd = 0;
	fraction = 0;
	calib.data.flag = 0;
} /* End of constructor */


/**************************************************************************//**
 * @brief       Destructor of the Touch class
******************************************************************************/
Touch::~Touch( )
{
	//Thread::State S = thd.get_state();

	disable_irq();

	//if((S != Thread::Inactive) && (S != Thread::Deleted))
	//{
	//	thd.terminate();
	//	thd.join();
	//}

	touch_cfg = NULL;
	xyz_data = NULL;
	adc_data = NULL;
}


/**************************************************************************//**
 * @brief       Touch controller initialization
 * @retval      error code
******************************************************************************/
Touch::init_err_t Touch::Init( void )
{
	init_err_t tp_err;

	if(touch_cfg->type == RESISTIVE)
	{
		tp_err = Clb_Setup();

		if(tp_err != TOUCH_OK)
			return tp_err;
	}

	if(touch_cfg->interface.type == I_2_C)
		I2C::frequency(touch_cfg->interface.freq);
	//else
		//SPI::frequency(touch_cfg->interface.freq);

	tp_err = Drv_Setup();

	if(tp_err != TOUCH_OK)
		return tp_err;
	else
	{
		//thd.start(callback(this, &Touch::Handle_touch));
		//queue = thing;

		if(touch_cfg->activity_irq.polarity == FALLING_OR_ACTIVE_LO)
		{
			rise(NULL);
			fall(Irq_Alert);
		}
		else
		{
			rise(Irq_Alert);
			fall(NULL);
		}

		enable_irq();
	}

	return tp_err;
} /* End of method Init() */


/**************************************************************************//**
 * @brief       Set Calibration data
 * @retval      error code
******************************************************************************/
Touch::init_err_t Touch::Clb_Setup()
{
	if(touch_cfg->name == "STMPE811")
	{
		#ifndef __USE_DEFAULT_CALIBRATION_DATA__
			// extract calibration info from lcdpanel EEPROM
			char adr = 0;

			if( (touch_cfg->interface.sda == EEIIC_SDA) || (touch_cfg->interface.scl == EEIIC_SCL))
			{	// lcdpanel EEPROM is on the same I2C channel no need to initialize a new one !
				if(I2C::write(EE_CALIB_DEVICE_ADDR, (const char *)&adr, 1, true) != 0)
					return TOUCH_INIT_ERR;
				if(I2C::read(EE_CALIB_DEVICE_ADDR, (char*)calib.KX08, sizeof(calib.KX08)) != 0)
					return TOUCH_INIT_ERR;
			}
			else
			{	// lcdpanel EEPROM is on different I2C channel so initialize a new one !
				I2C clb_eeprom(EEIIC_SDA, EEIIC_SCL);

				clb_eeprom.frequency(100000);
				if(clb_eeprom.write(EE_CALIB_DEVICE_ADDR, (const char *)&adr, 1, true) != 0)
					return TOUCH_INIT_ERR;
				if(clb_eeprom.read(EE_CALIB_DEVICE_ADDR, (char*)calib.KX08, sizeof(calib.KX08)) != 0)
					return TOUCH_INIT_ERR;
			}
		#endif

		if(calib.data.flag != 1)
		{	// load default calibration info
			unsigned char clb[] = {TPCALIBRATION_DATA};
			memcpy(calib.KX08, clb, sizeof(clb));
		}

		return TOUCH_OK;
	}
	else
		return TOUCH_UNSUPP_ERR;
} /* End of method Clb_Setup() */


/**************************************************************************//**
 * @brief       Set Touch Controller settings
 * @retval      error code
******************************************************************************/
Touch::init_err_t Touch::Drv_Setup()
{
	if(touch_cfg->name == "STMPE811")
	{
		unsigned char i, initdata[][2] = { INIT_DATA };

		for(i=0; i<(sizeof(initdata)>>1); i++)
		{
			if(initdata[i][0] == INT_CTRL)
			{	// reconfigure interrupt if needed
				initdata[i][1] = 0x01;
				initdata[i][1] |= (touch_cfg->activity_irq.trigger == INT_ON_EDGE)? 0x02 : 0x00;
				initdata[i][1] |= (touch_cfg->activity_irq.polarity == RISING_OR_ACTIVE_HI)? 0x04 : 0x00;
			}

			if(initdata[i][0] == FIFO_TH)
			{	// save threshold
				if(initdata[i][1] < 2)
					initdata[i][1] = 2;

				dot_thd = initdata[i][1]-1;
			}

			if(initdata[i][0] == TSC_FRACT_Z)
				fraction = initdata[i][1];	// save z precision

			if ((I2C::write(STMPE811_DEVICE_ADDR, (const char *)&initdata[i][0], 2)) != 0)
				return TOUCH_INIT_ERR;

			while (I2C::write(STMPE811_DEVICE_ADDR, (const char *)initdata, 0) != 0);  // ACK polling
		}

		return TOUCH_OK;
	}
	else
		return TOUCH_UNSUPP_ERR;
}


/**************************************************************************//**
 * @brief       Get Status of the pen
 * @retval     	true	: StylusDown
 * 				false	: StyluUp
******************************************************************************/
bool Touch::Get_Pen_Status()
{
	if(touch_cfg->name == "STMPE811")
	{
		unsigned char pen = TSC_CTRL;

	  	I2C::write(STMPE811_DEVICE_ADDR, (const char *)&pen, 1, true);
	  	pen = 0;
	  	I2C::read(STMPE811_DEVICE_ADDR, (char *)&pen, 1);

	  	return (pen & TSC_STA)? true : false;
	}
	else
		return false;
} /* End of method Get_Pen_Status() */


/**************************************************************************//**
 * @brief       Get one sample of data
 * @param[in]   * raw	: pointer to ring buffer to store the samples
******************************************************************************/
void Touch::Get_Data( unsigned long long * raw )
{
	if(touch_cfg->name == "STMPE811")
	{
		int idx = last_xyz_idx;
		unsigned char  i, packed_sample[4 * 4];
		unsigned short raw_x, raw_y;
		unsigned char  raw_z;

		i = TSC_DATA_FIFO;
		I2C::write(STMPE811_DEVICE_ADDR, (const char *)&i, 1, true);
		I2C::read(STMPE811_DEVICE_ADDR, (char *)packed_sample, sizeof(packed_sample));
		for(i=0; i<dot_thd; i++)
		{
			raw_x = (unsigned short)((packed_sample[(i*4)+0]<<4) | (packed_sample[(i*4)+1]>>4));
			raw_y = (unsigned short)(((0x0F & packed_sample[(i*4)+1])<<8) | packed_sample[(i*4)+2]);
			raw_z = packed_sample[(i*4)+3];

			idx = ((idx+1) < FIFO_DEPTH)? idx+1 : 0;
		    ((touch_raw_data_t*)raw)[idx].axis.x = raw_x;
		    ((touch_raw_data_t*)raw)[idx].axis.y = raw_y;
		    ((touch_raw_data_t*)raw)[idx].axis.z = raw_z;
		}
	}
} /* End of method Get_Data() */


/**************************************************************************//**
 * @brief       Get all available samples of data
 * @param[in]   * raw	: pointer to ring buffer to store the samples
 * @retval      		  samples count
******************************************************************************/
int Touch::Get_Fifo( unsigned long long * raw )
{
	if(touch_cfg->name == "STMPE811")
	{
	  int idx = last_xyz_idx;
	  unsigned char packets;

	  packets = FIFO_SIZE;
	  I2C::write(STMPE811_DEVICE_ADDR, (const char *)&packets, 1, true);
	  packets = 0;
	  I2C::read(STMPE811_DEVICE_ADDR, (char *)&packets, 1);
	  if(packets)
	  {
		unsigned char  packed_sample[FIFO_DEPTH];
		unsigned short raw_x, raw_y, i;
		unsigned char  raw_z;

		raw_z = TSC_DATA_FIFO;
		I2C::write(STMPE811_DEVICE_ADDR, (const char *)&raw_z, 1, true);
		I2C::read(STMPE811_DEVICE_ADDR, (char *)packed_sample, packets*4);

	    for(i=0; i<packets; i++)
	    {
	      raw_x = (unsigned short)((packed_sample[(i*4)+0]<<4) | (packed_sample[(i*4)+1]>>4));
	      raw_y = (unsigned short)(((0x0F & packed_sample[(i*4)+1])<<8) | packed_sample[(i*4)+2]);
	      raw_z = packed_sample[(i*4)+3];

	      idx = ((idx+1) < FIFO_DEPTH)? idx+1 : 0;
	      ((touch_raw_data_t*)raw)[idx].axis.x = raw_x;
	      ((touch_raw_data_t*)raw)[idx].axis.y = raw_y;
	      ((touch_raw_data_t*)raw)[idx].axis.z = raw_z;
	    }

	    return packets;
	  }
	  return 0;
	}
	else
		return 0;
} /* End of method Get_Fifo() */


/**************************************************************************//**
 * @brief       Coordinates Transfer function
 * @param[in]   points		: number of samples which have to become meaningful
******************************************************************************/
void Touch::Get_XYZ( int points)
{
	if(touch_cfg->name == "STMPE811")
	{
		int i, idx;

		for(i=0; i<points; i++)
		{
			idx = ((last_xyz_idx+1) < FIFO_DEPTH)? last_xyz_idx+1 : 0;
			screen_data[idx].axis.x = (signed short)(calib.data.KX1*((signed short)raw_data[idx].axis.x)+calib.data.KX2*((signed short)raw_data[idx].axis.y)+calib.data.KX3+0.5);
			screen_data[idx].axis.y = (signed short)(calib.data.KY1*((signed short)raw_data[idx].axis.x)+calib.data.KY2*((signed short)raw_data[idx].axis.y)+calib.data.KY3+0.5);
			screen_data[idx].axis.z = ((float)(raw_data[idx].axis.z >> fraction)) + (((float)(raw_data[idx].axis.z & ((0x1<<fraction)-1)))*(1.0f/float(0x1<<fraction)));
			last_xyz_idx = idx;
			#ifdef MORE_DETAILS
				DBG_MORE("TH_No: (%d)   X-> %d (%d), Y-> %d (%d), Z-> %f (%d), dots-> %d/%d", idx, screen_data[idx].axis.x, raw_data[idx].axis.x, screen_data[idx].axis.y, raw_data[idx].axis.y, screen_data[idx].axis.z, raw_data[idx].axis.z, i+1, points);
			#else
				DBG_MORE("TH_No: (%d)   X-> %d, Y-> %d, Z-> %f, dots-> %d/%d", idx, screen_data[idx].axis.x, screen_data[idx].axis.y, screen_data[idx].axis.z, i+1, points);
			#endif
		}

		x = screen_data[last_xyz_idx].axis.x; adc_x = raw_data[last_xyz_idx].axis.x;
		y = screen_data[last_xyz_idx].axis.y; adc_y = raw_data[last_xyz_idx].axis.y;
		z = screen_data[last_xyz_idx].axis.z; adc_z = raw_data[last_xyz_idx].axis.z;
			#ifdef MORE_DETAILS
				DBG("S_No: (%d)   X-> %d (%d), Y-> %d (%d), Z-> %f (%d), dots-> %d", last_xyz_idx, x, adc_x, y, adc_y, z, adc_z, points);
			#else
				DBG("S_No: (%d)   X-> %d, Y-> %d, Z-> %f, dots-> %d", last_xyz_idx, x, y, z, points);
			#endif
	}
} /* End of method Get_XYZ() */


/**************************************************************************//**
 * @brief       IRQ interrupt handler : indicates "New Touch Data available" which activates i2c data transfer in Handle_touch()
******************************************************************************/
void Touch::Irq_Alert()
{
	// Execute the time critical part first

	// Then the rest can execute later in user context (and can contain code that's not interrupt safe).
	//osSignalSet(TouchThreadID, NEW_TOUCH_DATA);

	if(NotifyID)
    	osSignalSet(NotifyID, NEW_TOUCH_DATA);
}

void Touch::notify(bool onoff)
{
	NotifyID = (onoff)? ThisThread::get_id() : NULL;
}

/**************************************************************************//**
 * @brief       Get index of the last sample in the ring buffer
 * @retval      idx
******************************************************************************/
int Touch::Get_Last_Idx() { return last_xyz_idx; }

/**************************************************************************//**
 * @brief       Get dot collection threshold
 * @retval      threshold
******************************************************************************/
int Touch::Get_Dot_thd() { return dot_thd; }

/**************************************************************************//**
 * @brief       Save dots if new data is available
******************************************************************************/
unsigned int Touch::Process_data(draw_data_t * data, size_t size)
{
	static unsigned int processed = 0;
	unsigned char TP_IntStat = 0, rec[2];
	int dots = 0;
	int from = 0;

	if(touch_cfg->name == "STMPE811")
	{
		TP_IntStat = INT_STA;
		I2C::write(STMPE811_DEVICE_ADDR, (const char *)&TP_IntStat, 1, true);
		TP_IntStat = 0;
		I2C::read(STMPE811_DEVICE_ADDR, (char *)&TP_IntStat, 1);

		if(TP_IntStat & INT_FIFO_TH)
		{
			Get_Data(&raw_data[0].dot);
			Get_XYZ(dot_thd);
			from = (last_xyz_idx+1) - dot_thd;
			if((processed + dots) > size)
			{
				processed = 0;
				printf("+%d\r\n", size);
			}
			for(int i = 0; i < dot_thd; i++)
			{
				data[processed+i].dot = screen_data[((from+i) < 0)? FIFO_DEPTH-(from+i) : from+i].xy;
				data[processed+i].axis.x = (data[processed+i].axis.x < 0)? 0 : ((data[processed+i].axis.x >= LCD_PIXEL_WIDTH)? LCD_PIXEL_WIDTH-1 : data[processed+i].axis.x);
				data[processed+i].axis.y = (data[processed+i].axis.y < 0)? 0 : ((data[processed+i].axis.y >= LCD_PIXEL_HEIGHT)? LCD_PIXEL_HEIGHT-1 : data[processed+i].axis.y);
			}
			processed += dot_thd;
		}

		if(TP_IntStat & INT_TOUCH_DET)
		{
			dots = Get_Fifo(&raw_data[0].dot);
			if(dots)
			{
				Get_XYZ(dots);
				from = (last_xyz_idx+1) - dots;
				if((processed + dots) > size)
				{
					processed = 0;
					printf("+%d\r\n", size);
				}
				for(int i = 0; i < dots; i++)
				{
					data[processed+i].dot = screen_data[((from+i) < 0)? FIFO_DEPTH-(from+i) : from+i].xy;
					data[processed+i].axis.x = (data[processed+i].axis.x < 0)? 0 : ((data[processed+i].axis.x >= LCD_PIXEL_WIDTH)? LCD_PIXEL_WIDTH-1 : data[processed+i].axis.x);
					data[processed+i].axis.y = (data[processed+i].axis.y < 0)? 0 : ((data[processed+i].axis.y >= LCD_PIXEL_HEIGHT)? LCD_PIXEL_HEIGHT-1 : data[processed+i].axis.y);
				}
				processed += dots;
			}
		}

		if(TP_IntStat & INT_FIFO_OFLOW)
		{
			printf("Overflow !!!");

			rec[0] = FIFO_STA;
			rec[1] = 0x01;		// Clear FIFO
			I2C::write(STMPE811_DEVICE_ADDR, (const char *)rec, 2);

			rec[1] = 0x00;		// Reset FIFO
			I2C::write(STMPE811_DEVICE_ADDR, (const char *)rec, 2);
		}

		rec[0] = INT_STA;
		rec[1] = TP_IntStat;
		I2C::write(STMPE811_DEVICE_ADDR, (const char *)rec, 2);
	}

	return processed;
}

/**************************************************************************//**
 * @brief       Pull the new samples if new touch data is available
******************************************************************************/
void Touch::Handle_touch()
{
	bool Stylus = false;
	bool Click_send = false;
	unsigned short idx_on = 0;
	unsigned char TP_IntStat = 0, rec[2];
	int dots = 0;
	uint32_t flg;

	notify(true);

	while(true)
	{
		flg = ThisThread::flags_wait_any(NEW_TOUCH_DATA);

		if(flg | NEW_TOUCH_DATA)
		{
			if(touch_cfg->name == "STMPE811")
			{
				TP_IntStat = INT_STA;
				I2C::write(STMPE811_DEVICE_ADDR, (const char *)&TP_IntStat, 1, true);
				TP_IntStat = 0;
				I2C::read(STMPE811_DEVICE_ADDR, (char *)&TP_IntStat, 1);

				if(TP_IntStat & INT_FIFO_TH)
				{
					Get_Data(&raw_data[0].dot);
					Get_XYZ(dot_thd);
					if(Stylus)
					{
						if(!Click_send)
						{
							MSG(EV_STYLUS_DOWN, idx_on, 0);
							Click_send = true;
						}
						MSG(EV_STYLUS_HOLD, idx_on, last_xyz_idx);
					}
				}

				if(TP_IntStat & INT_TOUCH_DET)
				{
					Stylus = Get_Pen_Status();
					DBG("Pen: %s", (Stylus)? "DOWN" : "UP");

					dots = Get_Fifo(&raw_data[0].dot);
					if(dots)
					{
						Get_XYZ(dots);

						if(Stylus)
						{
							int on = (last_xyz_idx+1) - dots;
							idx_on = (on<0)? (FIFO_DEPTH-on) : on;
							MSG(EV_STYLUS_DOWN, idx_on, 0);
							Click_send = true;
						}
						else
						{
							if(!Click_send)
							{
								MSG(EV_STYLUS_DOWN, idx_on, 0);
								Click_send = true;
							}
							MSG(EV_STYLUS_UP, idx_on, last_xyz_idx);
						}
					}
					else
					{
						if(Stylus)
						{
							Click_send = false;
							idx_on = (last_xyz_idx == (FIFO_DEPTH-1))? 0 : last_xyz_idx+1;
						}
						else
						{
							if(Click_send)
								MSG( EV_STYLUS_UP, idx_on, last_xyz_idx);
						}
					}
				}

				if(TP_IntStat & INT_FIFO_OFLOW)
				{
					DBG("Overflow !!!");

					rec[0] = FIFO_STA;
					rec[1] = 0x01;		// Clear FIFO
					I2C::write(STMPE811_DEVICE_ADDR, (const char *)rec, 2);

					rec[1] = 0x00;		// Reset FIFO
					I2C::write(STMPE811_DEVICE_ADDR, (const char *)rec, 2);
				}

				rec[0] = INT_STA;
				rec[1] = TP_IntStat;
				I2C::write(STMPE811_DEVICE_ADDR, (const char *)rec, 2);
			}
		}
	}
}
#endif
} // namespace Vekatech

/* End of file */
