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
	int myproperty;
} ATMO_RTC5Click_Config_t;


typedef enum
{
	ATMO_RTC5Click_Status_Success = 0,
	ATMO_RTC5Click_Status_Fail,
} ATMO_RTC5Click_Status_t;


/******************************************************************************/
/*                       Public Function Declarations                         */
/******************************************************************************/
ATMO_RTC5Click_Status_t ATMO_RTC5Click_Init( ATMO_RTC5Click_Config_t *config );
ATMO_RTC5Click_Status_t ATMO_RTC5Click_TimeGet( char *time_hhmmss );
ATMO_RTC5Click_Status_t ATMO_RTC5Click_OscillatorStart( void );
ATMO_RTC5Click_Status_t ATMO_RTC5Click_OscillatorStop( void );

#endif