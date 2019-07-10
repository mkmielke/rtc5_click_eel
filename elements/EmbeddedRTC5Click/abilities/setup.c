	ATMO_RTC5Click_Config_t config;
	
	config.cs_pin = ATMO_PROPERTY( undefined, csPin );
	config.gpioDriverInstance = ATMO_PROPERTY( undefined, gpioDriverInstance );
	ATMO_RTC5Click_Init( &ATMO_VARIABLE( undefined, handle ), &config );
	
	return ATMO_Status_Success;
