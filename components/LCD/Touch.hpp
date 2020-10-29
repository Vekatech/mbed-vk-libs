/**************************************************************************//**
 * @file         Touch.h
 * @brief        Touch driver class for VK-LCD panels
******************************************************************************/

#ifndef MBED_TOUCH_H
#define MBED_TOUCH_H

#include "mbed.h"
#include "stmpe811iic.h"
#include <string>

#define NEW_TOUCH_DATA	(1UL << 2)

namespace Vekatech {

    /*! @enum Touch_type_t
    	@brief Type of the touch controller
    */
    typedef enum {
    	RESISTIVE = 0, /*!< Resistive */
    	CAPACITIVE     /*!< Capacitive */
    } Touch_type_t;

    /*! @enum Touch_interface_type_t
        @brief Communication interface
    */
    typedef enum {
    	I_2_C = 0, 		 /*!< I2C */
    	S_P_I     		 /*!< SPI */
    } Touch_communication_type_t;

    /*! @enum IRQ_trigger_t
    	@brief Type of the interrupt
    */
    typedef enum {
    	INT_ON_EDGE  = 0,   /*!< generate INT on front change */
    	INT_ON_LEVEL        /*!< generate INT on active level */
    } IRQ_trigger_t;

    /*! @enum IRQ_polarity_t
        @brief Edge of a signal
    */
    typedef enum {
    	RISING_OR_ACTIVE_HI  = 0, /*!< Rising edge/HI level     */
    	FALLING_OR_ACTIVE_LO      /*!< Falling edge/LOW level   */
    } IRQ_polarity_t;

    /*! @struct touch_config_t
    	@brief Touch Config structure
    */
    typedef struct {
    	string			name;				/*!< Name of the Touch driver */
        int				screen;				/*!< reserved				  */
        Touch_type_t	type;				/*!< Resistive or Capacitive  */
        struct {							/*!< I2C or SPI & the pins of selected periphery */
        	Touch_communication_type_t type;
        	PinName sda;
        	PinName scl;
        	PinName mosi;
        	PinName miso;
        	PinName sclk;
        	PinName ssel;
        	int 	freq;
        }interface;
        struct {							/*!< IRQ : front and pin of selected periphery */
        	IRQ_trigger_t	trigger;
            IRQ_polarity_t	polarity;
            PinName			pin;
        }activity_irq;
    }touch_config_t;

extern const touch_config_t STMPE811_cfg;



/*! @class Touch
 *  @brief Touch driver class for VK-LCD panels
 *
 *  Example:
 *
 *  @code
 *  #include "mbed.h"
 *  #include "Touch.hpp"
 *
 *  using namespace Vekatech;
 *
 *  Touch   STMPE811;
 *
 *  // main() runs in its own thread in the OS
 *  int main()
 *  {
 *     osEvent evt;
 *
 *     STMPE811.Init();
 *
 *     while (true)
 *     {
 *          evt = STMPE811.Wait_MSG();
 *          if (evt.status == osEventMail)
 *          {
 *              Touch::MSGQ_t* msg = (Touch::MSGQ_t*)evt.value.p;
 *              printf("[EV (%d)], start_idx: %d end_idx: %d\r\n", msg->event, msg->start_idx, msg->stop_idx);
 *
 *              switch(msg->event)
 *              {
 *                  case Touch::EV_STYLUS_UP:
 *                      printf("[UP (%d)] X: %d Y: %d Z: %f\r\n", msg->stop_idx, STMPE811.xyz_data[msg->stop_idx].axis.x, STMPE811.xyz_data[msg->stop_idx].axis.y, STMPE811.xyz_data[msg->stop_idx].axis.z );
 *                  break;
 *
 *                  case Touch::EV_STYLUS_DOWN:
 *                      printf("[DOWN (%d)] X: %d Y: %d Z: %f\r\n", msg->start_idx, STMPE811.xyz_data[msg->start_idx].axis.x, STMPE811.xyz_data[msg->start_idx].axis.y, STMPE811.xyz_data[msg->start_idx].axis.z );
 *                  break;
 *
 *                  case Touch::EV_STYLUS_HOLD:
 *                      if(msg->stop_idx >= (STMPE811.Get_Dot_thd()-1) )
 *                          for(int i = (msg->stop_idx+1 - STMPE811.Get_Dot_thd()); i<=msg->stop_idx; i++)
 *                              printf("[HOLD (%d)] X: %d Y: %d Z: %f\r\n", i, STMPE811.xyz_data[i].axis.x, STMPE811.xyz_data[i].axis.y, STMPE811.xyz_data[i].axis.z );
 *                  break;
 *
 *                  default: break;
 *              }
 *
 *              STMPE811.Del_MSG((Touch::MSGQ_t*)evt.value.p);
 *          }
 *      }
 *   }
 *  @endcode
 */
class Touch : public I2C, public InterruptIn
{
public:
    /*! @enum Init_err_t
        @brief Error codes
     */
    typedef enum {
        TOUCH_OK = 0,                   /*!< Initialization successful      */
        TOUCH_INIT_ERR = -1,           	/*!< Communication interface err while configuring driver */
		TOUCH_UNSUPP_ERR = -2,			/*!< Unsupported driver */
        TOUCH_ERR = -3,         		/*!< unknown error */
    } init_err_t;

    /*! @enum EVENTLIST_t
        @brief Type of the Event:
     */
    typedef enum {
    	EV_STYLUS_UP =0,
		EV_STYLUS_DOWN,
		EV_STYLUS_HOLD,
		//EV_STYLUS_MOVE,
		NEVENTS
    }EVENTLIST_t;

    /*! @struct MSGQ_t
        @brief Touch Message structure
     */
    typedef	struct {
    	EVENTLIST_t			event;
    	unsigned short 		start_idx;
    	unsigned short 		stop_idx;
    }MSGQ_t;

    /** Constructor method of Touch object
    */
    Touch( const touch_config_t * tp_cfg = &STMPE811_cfg );

    /** Destructor method of Touch object
    */
    virtual ~Touch( void );

    /** Touch controller initialization
     *  @retval       Error code
     */
    init_err_t Init();

    /** Set Calibration data (raw data interpretation)
     *  @retval       Error code
     */
    virtual init_err_t Clb_Setup();

    /** Set Touch Controller settings
    *  @retval       Error code
    */
    virtual init_err_t Drv_Setup();

    /** Get Status of the pen
     * @retval     	true	: StylusDown
     * 				false	: StyluUp
     */
    virtual bool Get_Pen_Status();

    /** Get one sample of data
     *  @param[in]   raw: pointer to ring buffer to store the samples
     */
    virtual void Get_Data( unsigned long long * raw );

    /** Get all available samples of data
     *  @param[in]   raw: pointer to ring buffer to store the samples
     *  @retval           samples count
     */
    virtual int Get_Fifo( unsigned long long * raw );

    /** Transfer function (from raw coordinates to screen coordinates)
     *  @param[in]	 points : number of samples to transfer
     */
    virtual void Get_XYZ( int points );

    /** New Data available
     */
    static void Irq_Alert();

    /** Pull the new samples if new data is available
     */
    virtual void Handle_touch();

    /** Save dots if new data is available
     */
    virtual unsigned int Process_data(draw_data_t * data, size_t size);

    /** Get index of the last sample in the ring buffer
     */
    virtual int Get_Last_Idx();

    /** Get the minimum amounts of dots when pen is hold down
     */
    virtual int Get_Dot_thd();

    virtual void notify(bool onoff);

    /** Wait message from the touch thread
     */
    virtual osEvent Wait_MSG( uint32_t millisec=osWaitForever ) { return mail.get( millisec ); };

    /** Delete message from the mail queue
    */
    virtual osStatus Del_MSG( MSGQ_t *ptr ) { return mail.free( ptr ); };

    /** raw coordinates of the last received dot
     */
	unsigned short adc_x;
	unsigned short adc_y;
	unsigned short adc_z;

    /** Coordinates of the last received dot
     */
    short x;
	short y;
	float z;

	/** Raw History of the received dots
	*/
	const touch_raw_data_t *adc_data;

	/** History of the received dots
	*/
	const touch_screen_data_t *xyz_data;

protected:
	/** Send message to the main application
	 */
	osStatus MSG(EVENTLIST_t touch_msg, unsigned short touch_on, unsigned short touch_off)
	{ /*
		MSGQ_t * msg = mail.alloc();
	    msg->event = touch_msg;
	    msg->start_idx = touch_on;
	   	msg->stop_idx = touch_off;
	    return mail.put(msg); */
		return osOK;
	};

    int					 last_xyz_idx;
    int					 dot_thd;
    unsigned char		 fraction;
    touch_raw_data_t	 raw_data[FIFO_DEPTH];
    touch_screen_data_t  screen_data[FIFO_DEPTH];
    const touch_config_t *touch_cfg;
    touch_calib_data_t	 calib;
    //Thread		 		 thd;
    //EventQueue 			 queue;
    //EventQueue queue(32 * EVENTS_EVENT_SIZE);
    Mail<MSGQ_t, 32> 	 mail;
};

} // namespace Vekatech

#endif /* MBED_TOUCH_H */
