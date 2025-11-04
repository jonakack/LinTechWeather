
#ifndef __Template_h_
#define __Template_h_

#include "smw.h"

typedef struct
{

	smw_task* task;

} Template;


int Template_Initiate(Template* _Server);
int Template_InitiatePtr(Template** _ServerPtr);


void Template_Dispose(Template* _Server);
void Template_DisposePtr(Template** _ServerPtr);

#endif //__Template_h_
