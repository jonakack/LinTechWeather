#include <stdio.h>
#include "smw.h"
#include "utils.h"
#include "libs/WeatherServer/WeatherServer.h"


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


// Ta bort rad 92 i TCPServer.c innan vi pushar till riktig server
// ^ Printf kan störa non-blocking processer i high load