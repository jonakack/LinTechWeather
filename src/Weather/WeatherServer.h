#ifndef __WeatherServer_h_
#define __WeatherServer_h_

#include "TaskScheduler.h"
#include "LinkedList.h"
#include "HTTPServer.h"
#include "WeatherServerInstance.h"

typedef struct
{
	HTTPServer httpServer;

	LinkedList* instances;

	Task* task;
	TaskScheduler* scheduler;  // Reference to task scheduler

} WeatherServer;


int WeatherServer_Initiate(WeatherServer* _Server, TaskScheduler* _Scheduler);
int WeatherServer_InitiatePtr(WeatherServer** _ServerPtr, TaskScheduler* _Scheduler);

void WeatherServer_Dispose(WeatherServer* _Server);
void WeatherServer_DisposePtr(WeatherServer** _ServerPtr);

#endif //__WeatherServer_h_
