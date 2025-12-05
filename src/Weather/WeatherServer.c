#include <stdlib.h>
#include "WeatherServer.h"

//-----------------Internal Functions-----------------

void WeatherServer_TaskWork(void* _Context, uint64_t _MonTime);
int WeatherServer_OnHTTPConnection(void* _Context, HTTPServerConnection* _Connection);

//----------------------------------------------------

int WeatherServer_Initiate(WeatherServer* _Server, TaskScheduler* _Scheduler)
{
	if(_Scheduler == NULL)
		return -1;

	_Server->scheduler = _Scheduler;

	HTTPServer_Initiate(&_Server->httpServer, _Scheduler, WeatherServer_OnHTTPConnection);

	_Server->instances = LinkedList_create();

	_Server->task = TaskScheduler_CreateTask(_Scheduler, _Server, WeatherServer_TaskWork, TASK_PRIORITY_NORMAL);

	return 0;
}

int WeatherServer_InitiatePtr(WeatherServer** _ServerPtr, TaskScheduler* _Scheduler)
{
	if(_ServerPtr == NULL)
		return -1;

	WeatherServer* _Server = (WeatherServer*)malloc(sizeof(WeatherServer));
	if(_Server == NULL)
		return -2;

	int result = WeatherServer_Initiate(_Server, _Scheduler);
	if(result != 0)
	{
		free(_Server);
		return result;
	}

	*(_ServerPtr) = _Server;

	return 0;
}

int WeatherServer_OnHTTPConnection(void* _Context, HTTPServerConnection* _Connection)
{
	WeatherServer* _Server = (WeatherServer*)_Context;

	WeatherServerInstance* instance = NULL;
	int result = WeatherServerInstance_InitiatePtr(_Connection, &instance);
	if(result != 0)
	{
		printf("WeatherServer_OnHTTPConnection: Failed to initiate instance\n");
		return -1;
	}

	LinkedList_append(_Server->instances, instance);

	return 0;
}

void WeatherServer_TaskWork(void* _Context, uint64_t _MonTime)
{
	WeatherServer* _Server = (WeatherServer*)_Context;

	// Process all instances
	LinkedList_foreach(_Server->instances, node)
	{
		WeatherServerInstance* instance = (WeatherServerInstance*)node->item;
		WeatherServerInstance_Work(instance, _MonTime);
	}
	
	// Remove completed instances
	Node* current = _Server->instances->head;
	
	while (current != NULL) {
		WeatherServerInstance* instance = (WeatherServerInstance*)current->item;
		Node* next = current->front;  // Save next before potential removal
		
		if (instance->completed) {
			// printf("WeatherServer: Removing completed instance\n");
			LinkedList_remove(_Server->instances, current, (void(*)(void*))WeatherServerInstance_Dispose);
		}
		
		current = next;
	}
}

void WeatherServer_Dispose(WeatherServer* _Server)
{
	HTTPServer_Dispose(&_Server->httpServer);
	TaskScheduler_DestroyTask(_Server->scheduler, _Server->task);
}

void WeatherServer_DisposePtr(WeatherServer** _ServerPtr)
{
	if(_ServerPtr == NULL || *(_ServerPtr) == NULL)
		return;

	WeatherServer_Dispose(*(_ServerPtr));
	free(*(_ServerPtr));
	*(_ServerPtr) = NULL;
}