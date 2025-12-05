#include <stdio.h>
#include "TaskScheduler.h"
#include "Utils.h"
#include "WeatherServer.h"


int main()
{
	TaskScheduler* scheduler = TaskScheduler_Create();
	if(scheduler == NULL)
	{
		return -1; 
	}

	WeatherServer server;
	WeatherServer_Initiate(&server, scheduler);

	while(1)
	{
		TaskScheduler_Work(scheduler, SystemMonotonicMS()); 
	}

	WeatherServer_Dispose(&server);

	TaskScheduler_Dispose(&scheduler);

	return 0;
}