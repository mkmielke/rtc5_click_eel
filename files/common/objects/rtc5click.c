/******************************************************************************/
/*                                Includes                                    */
/******************************************************************************/
#include "rtc5click.h"
#include <atmel_start_pins.h>


/******************************************************************************/
/*                                 Defines                                    */
/******************************************************************************/
#define _ATMO_MAX_RTC5_CLICKS	2

#define _INSTR_READ		0x13
#define _INSTR_WRITE	0x12

#define _REG_RTCHSEC	0x00
#define _REG_RTCSEC		0x01
#define _REG_RTCMIN		0x02
#define _REG_RTCHOUR	0x03
#define _REG_RTCWKDAY	0x04
#define _REG_RTCDATE	0x05
#define _REG_RTCMTH		0x06
#define _REG_RTCYEAR	0x07
#define _REG_CONTROL	0x08
#define _REG_OSCTRIM	0x09


/******************************************************************************/
/*                       Structs, Typedefs, and Enums                         */
/******************************************************************************/
typedef union
{
	uint8_t memory[8];
	struct {
		uint8_t RTCHSEC;
		uint8_t RTCSEC;
		uint8_t RTCMIN;
		uint8_t RTCHOUR;
		uint8_t RTCWKDAY;
		uint8_t RTCDATE;
		uint8_t RTCMTH;
		uint8_t RTCYEAR;
	} reg;
	struct {
		// RTCHSEC
		uint8_t HSECONE : 4;
		uint8_t HSECTEN : 4;
		// RTCSEC
		uint8_t SECONE : 4;
		uint8_t SECTEN : 3;
		uint8_t ST : 1;
		// RTCMIN
		uint8_t MINONE : 4;
		uint8_t MINTEN : 3;
		uint8_t :1;
		// RTCHOUR
		uint8_t HRONE : 4;
		uint8_t HRTEN : 1;
		uint8_t AM_PM : 1;
		uint8_t FORMAT: 1;
		uint8_t TRIMSIGN: 1;
		// RTCWKDAY
		uint8_t WKDAY : 3;
		uint8_t VBATEN : 1;
		uint8_t PWRFAIL : 1;
		uint8_t OSCRUN : 1;
		uint8_t :2;
		// RTCDATE
		uint8_t DATEONE : 4;
		uint8_t DATETEN : 2;
		uint8_t :2;
		// RTCMTH
		uint8_t MTHONE : 4;
		uint8_t MTHTEN : 1;
		uint8_t LPYR : 1;
		uint8_t :2;
		// RTCYEAR
		uint8_t YRONE : 4;
		uint8_t YRTEN : 4;
	} bits;
} RTC_Time;


/******************************************************************************/
/*                            Global Variables                                */
/******************************************************************************/
static RTC_Time curr_time;
static uint8_t currNumRTC5Clicks = 0;
static ATMO_RTC5Click_Config_t _config[_ATMO_MAX_RTC5_CLICKS];


/******************************************************************************/
/*                      Private Function Definitions                          */
/******************************************************************************/
static void _spi_init( ATMO_DriverInstanceHandle_t gpioDriverIntance, ATMO_SPI_CS_t csPin )
{
	ATMO_GPIO_Config_t pinConfig;

	// copied from SPI_0_init() -- spi_basic.c
	SPI0.CTRLA = 1 << SPI_CLK2X_bp    /* Enable Double Speed: enabled */
				 | 0 << SPI_DORD_bp   /* Data Order Setting: disabled */
				 | 1 << SPI_ENABLE_bp /* Enable Module: enabled */
				 | 1 << SPI_MASTER_bp /* SPI module in master mode */
				 | SPI_PRESC_DIV4_gc; /* System Clock / 4 */

	// disable the slave select line when operating as SPI master
	SPI0.CTRLB |= SPI_SSD_bm;

	// configure the CS pin
	pinConfig.pinMode = ATMO_GPIO_PinMode_Output_PushPull;
	pinConfig.initialState = ATMO_GPIO_PinState_High;
	ATMO_GPIO_SetPinConfiguration( gpioDriverIntance, csPin, &pinConfig );
}


static void _spi_write( ATMO_SPI_CS_t csPin, const uint8_t *cmdBytes, uint16_t numCmdBytes, const uint8_t *writeBytes, uint16_t numWriteBytes )
{
	uint8_t ctrlb;
	
	ctrlb = SPI0.CTRLB;
	
	// Change transfer mode to SPI MODE 3
	SPI0.CTRLB = ( ctrlb & ~SPI_MODE_gm ) | ( SPI_MODE_3_gc);
	
	// assert CS
	ATMO_GPIO_SetPinState( 0, csPin, ATMO_GPIO_PinState_Low );

	// write command and data
	SPI_0_write_block( cmdBytes, numCmdBytes );
	SPI_0_write_block( writeBytes, numWriteBytes );

	// deassert CS
	ATMO_GPIO_SetPinState( 0, csPin, ATMO_GPIO_PinState_High );
	
	// restore SPI0.CTRLB
	SPI0.CTRLB = ctrlb;
}


static void _spi_read( ATMO_SPI_CS_t csPin, const uint8_t *cmdBytes, uint16_t numCmdBytes, uint8_t *readBytes, uint16_t numReadBytes )
{
	uint8_t ctrlb;
	
	ctrlb = SPI0.CTRLB;
	
	// Change transfer mode to SPI MODE 3
	SPI0.CTRLB = ( ctrlb & ~SPI_MODE_gm ) | ( SPI_MODE_3_gc);
	
	// assert CS
	ATMO_GPIO_SetPinState( 0, csPin, ATMO_GPIO_PinState_Low );

	// write command and read data
	SPI_0_write_block( cmdBytes, numCmdBytes );
	SPI_0_read_block( readBytes, numReadBytes );

	// deassert CS
	ATMO_GPIO_SetPinState( 0, csPin, ATMO_GPIO_PinState_High );
	
	// restore SPI0.CTRLB
	SPI0.CTRLB = ctrlb;
}


/******************************************************************************/
/*                       Public Function Definitions                          */
/******************************************************************************/
ATMO_RTC5Click_Status_t ATMO_RTC5Click_Init( ATMO_DriverInstanceHandle_t *handle, ATMO_RTC5Click_Config_t *config )
{
	if ( currNumRTC5Clicks >= _ATMO_MAX_RTC5_CLICKS )
	{
		return ATMO_RTC5Click_Status_Fail;	
	}
	
	*handle = currNumRTC5Clicks;
	currNumRTC5Clicks++;
	
	if ( config != NULL )
	{
		if ( ATMO_RTC5Click_SetConfiguration( *handle, config ) != ATMO_RTC5Click_Status_Success )
		{
			return ATMO_RTC5Click_Status_Fail;
		}
	}
	
	_spi_init( config->gpioDriverInstance, config->cs_pin ); 
	
	if ( ATMO_RTC5Click_OscillatorStart( *handle ) != ATMO_RTC5Click_Status_Success )
	{
		ATMO_RTC5Click_OscillatorStop( *handle );
		return ATMO_RTC5Click_Status_Fail;
	}
	
	return ATMO_RTC5Click_Status_Success;
}

ATMO_RTC5Click_Status_t ATMO_RTC5Click_SetConfiguration( ATMO_DriverInstanceHandle_t handle, const ATMO_RTC5Click_Config_t *config )
{
	if ( handle >= _ATMO_MAX_RTC5_CLICKS )
	{
		return ATMO_RTC5Click_Status_Fail;
	}
	
	if ( config != NULL )
	{
		memcpy( &_config[handle], config, sizeof( ATMO_RTC5Click_Config_t) );
	}
	else
	{
		return ATMO_RTC5Click_Status_Fail;
	}
}


ATMO_RTC5Click_Status_t ATMO_RTC5Click_TimeGet( ATMO_DriverInstanceHandle_t handle, char *time_hhmmss )
{
	uint8_t cmd[2];
	
	// read the timekeeping registers
	cmd[0] = _INSTR_READ;
	cmd[1] = _REG_RTCHSEC;
	_spi_read( _config[handle].cs_pin, cmd, 2, curr_time.memory, 8 );
	
	// print the time in hhmmss format
	sprintf( time_hhmmss, "%u%u:%u%u:%u%u", curr_time.bits.HRTEN, curr_time.bits.HRONE,
											curr_time.bits.MINTEN, curr_time.bits.MINONE,
											curr_time.bits.SECTEN, curr_time.bits.SECONE );
											
	return ATMO_RTC5Click_Status_Success;
}


ATMO_RTC5Click_Status_t ATMO_RTC5Click_OscillatorStart( ATMO_DriverInstanceHandle_t handle )
{
	uint8_t cmd[2];
	uint8_t temp;
	
	// read the RTCSEC register
	cmd[0] = _INSTR_READ;
	cmd[1] = _REG_RTCSEC;
	_spi_read( _config[handle].cs_pin, cmd, 2, &temp, 1 );
	
	// toggle the ST (oscillator start) bit
	temp |= 0x80;
	
	// write the RTCSEC register back
	cmd[0] = _INSTR_WRITE;
	cmd[1] = _REG_RTCSEC;
	_spi_write( _config[handle].cs_pin, cmd, 2, &temp, 1 );
	
	return ATMO_RTC5Click_Status_Success;
}


ATMO_RTC5Click_Status_t ATMO_RTC5Click_OscillatorStop( ATMO_DriverInstanceHandle_t handle )
{
	uint8_t cmd[2];
	uint8_t temp;
	
	// read the RTCSEC register
	cmd[0] = _INSTR_READ;
	cmd[1] = _REG_RTCSEC;
	_spi_read( _config[handle].cs_pin, cmd, 2, &temp, 1 );
	
	// toggle the ST (oscillator start) bit
	temp &= ~0x80;
	
	// write the RTCSEC register back
	cmd[0] = _INSTR_WRITE;
	cmd[1] = _REG_RTCSEC;
	_spi_write( _config[handle].cs_pin, cmd, 2, &temp, 1 );
	
	return ATMO_RTC5Click_Status_Success;
}


/******************************************************************************/
/*                       Interrupt Service Routines                           */
/******************************************************************************/