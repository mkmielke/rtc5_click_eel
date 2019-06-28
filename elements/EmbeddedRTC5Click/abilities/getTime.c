	char curr_time[15];
	
	ATMO_RTC5Click_TimeGet( curr_time );
	ATMO_CreateValueString( out, curr_time );	
	
	return ATMO_Status_Success;