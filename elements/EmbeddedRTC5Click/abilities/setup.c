	ATMO_RTC5Click_Config_t config;
	
	config.myproperty = ATMO_PROPERTY( RTC5Click, myproperty );
	ATMO_RTC5Click_Init( &config );
	
	return ATMO_Status_Success;