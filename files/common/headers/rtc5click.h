#ifndef __RTC5CLICK_H_
#define __RTC5CLICK_H_

/******************************************************************************/
/*                                Includes                                    */
/******************************************************************************/
#include "../app_src/atmosphere_platform.h"


/******************************************************************************/
/*                                 Defines                                    */
/******************************************************************************/


/******************************************************************************/
/*                       Structs, Typedefs, and Enums                         */
/******************************************************************************/
typedef struct  
{
	uint32_t cs_pin;
	ATMO_DriverInstanceHandle_t gpioDriverInstance;
} ATMO_RTC5Click_Config_t;


typedef enum
{
	ATMO_RTC5Click_Status_Success = 0,
	ATMO_RTC5Click_Status_Fail,
} ATMO_RTC5Click_Status_t;


/******************************************************************************/
/*                       Public Function Declarations                         */
/******************************************************************************/
ATMO_RTC5Click_Status_t ATMO_RTC5Click_Init( ATMO_DriverInstanceHandle_t *handle, ATMO_RTC5Click_Config_t *config );
ATMO_RTC5Click_Status_t ATMO_RTC5Click_SetConfiguration( ATMO_DriverInstanceHandle_t handle, const ATMO_RTC5Click_Config_t *config );
ATMO_RTC5Click_Status_t ATMO_RTC5Click_TimeGet( ATMO_DriverInstanceHandle_t handle, char *time_hhmmss );
ATMO_RTC5Click_Status_t ATMO_RTC5Click_OscillatorStart( ATMO_DriverInstanceHandle_t handle );
ATMO_RTC5Click_Status_t ATMO_RTC5Click_OscillatorStop( ATMO_DriverInstanceHandle_t handle );

#endif