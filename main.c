#include <stdio.h>
#include "smw.h"
#include "utils.h"
#include "WeatherServer.h"


int main()
{
	smw_init();

	WeatherServer server;
	WeatherServer_Initiate(&server);

	while(1)
	{
		smw_work(SystemMonotonicMS()); 
	}

	WeatherServer_Dispose(&server);

	smw_dispose();

	return 0;
}