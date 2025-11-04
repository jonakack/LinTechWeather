
#ifndef __WeatherServer_h_
#define __WeatherServer_h_

#include "smw.h"
#include "linked_list.h"
#include "HTTPServer/HTTPServer.h"

#include "WeatherServerInstance.h"

typedef struct
{
	HTTPServer httpServer;

	LinkedList* instances;

	smw_task* task;

} WeatherServer;


int WeatherServer_Initiate(WeatherServer* _Server);
int WeatherServer_InitiatePtr(WeatherServer** _ServerPtr);


void WeatherServer_Dispose(WeatherServer* _Server);
void WeatherServer_DisposePtr(WeatherServer** _ServerPtr);

#endif //__WeatherServer_h_
