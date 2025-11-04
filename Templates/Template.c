#include "Template.h"
#include <stdlib.h>

//-----------------Internal Functions-----------------

void Template_TaskWork(void* _Context, uint64_t _MonTime);

//----------------------------------------------------

int Template_Initiate(Template* _Server)
{

	_Server->task = smw_createTask(_Server, Template_TaskWork);

	return 0;
}

int Template_InitiatePtr(Template** _ServerPtr)
{
	if(_ServerPtr == NULL)
		return -1;

	Template* _Server = (Template*)malloc(sizeof(Template));
	if(_Server == NULL)
		return -2;

	int result = Template_Initiate(_Server);
	if(result != 0)
	{
		free(_Server);
		return result;
	}

	*(_ServerPtr) = _Server;

	return 0;
}

void Template_TaskWork(void* _Context, uint64_t _MonTime)
{
	//Template* _Server = (Template*)_Context;
	
}

void Template_Dispose(Template* _Server)
{
	smw_destroyTask(_Server->task);
}

void Template_DisposePtr(Template** _ServerPtr)
{
	if(_ServerPtr == NULL || *(_ServerPtr) == NULL)
		return;

	Template_Dispose(*(_ServerPtr));
	free(*(_ServerPtr));
	*(_ServerPtr) = NULL;
}
