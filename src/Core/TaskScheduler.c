#include <stdlib.h>
#include <stdio.h>
#include "TaskScheduler.h"

TaskScheduler* TaskScheduler_Create()
{
	TaskScheduler* scheduler = (TaskScheduler*)calloc(1, sizeof(TaskScheduler));
	if(scheduler == NULL)
	{
		printf("[TaskScheduler] Failed to allocate scheduler instance\n");
		return NULL;
	}

	// Initialize priority queues
	int i;
	for(i = 0; i < TASK_PRIORITY_COUNT; i++)
	{
		scheduler->priority_queues[i] = LinkedList_create();
		if(scheduler->priority_queues[i] == NULL)
		{
			// Cleanup already created queues on failure
			while(--i >= 0)
			{
				LinkedList_dispose(&scheduler->priority_queues[i], NULL);
			}
			free(scheduler);
			printf("[TaskScheduler] Failed to allocate priority queue\n");
			return NULL;
		}
	}

	scheduler->total_tasks = 0;
	return scheduler;
}

void TaskScheduler_Dispose(TaskScheduler** _Scheduler)
{
	if(_Scheduler == NULL || *_Scheduler == NULL)
		return;

	TaskScheduler* scheduler = *_Scheduler;

	// Free all priority queues and their tasks
	int i;
	for(i = 0; i < TASK_PRIORITY_COUNT; i++)
	{
		if(scheduler->priority_queues[i] != NULL)
		{
			// Free all tasks in this priority queue
			LinkedList_dispose(&scheduler->priority_queues[i], free);
		}
	}

	free(scheduler);
	*_Scheduler = NULL;
}

Task* TaskScheduler_CreateTask(TaskScheduler* _Scheduler, void* _Context,
                                void (*_Callback)(void* _Context, uint64_t _MonTime),
                                TaskPriority _Priority)
{
	if(_Scheduler == NULL || _Callback == NULL)
		return NULL;

	// Validate priority
	if(_Priority < 0 || _Priority >= TASK_PRIORITY_COUNT)
	{
		printf("[TaskScheduler] Invalid priority level: %d\n", _Priority);
		return NULL;
	}

	// Allocate new task
	Task* task = (Task*)malloc(sizeof(Task));
	if(task == NULL)
	{
		printf("[TaskScheduler] Failed to allocate task\n");
		return NULL;
	}

	task->context = _Context;
	task->callback = _Callback;
	task->priority = _Priority;
	task->destroy = 0;  // Initialize: task is not marked for deletion

	// Add task to appropriate priority queue
	LinkedList* queue = _Scheduler->priority_queues[_Priority];
	if(LinkedList_append(queue, task) != 0)
	{
		printf("[TaskScheduler] Failed to append task to priority queue\n");
		free(task);
		return NULL;
	}

	_Scheduler->total_tasks++;
	return task;
}

void TaskScheduler_DestroyTask(TaskScheduler* _Scheduler, Task* _Task)
{
	if(_Scheduler == NULL || _Task == NULL)
		return;
	// Mark task for deletion; actual removal happens in TaskScheduler_Work
	_Task->destroy = 1;
}

void TaskScheduler_Work(TaskScheduler* _Scheduler, uint64_t _MonTime)
{
	if(_Scheduler == NULL)
		return;

	// Execute tasks in priority order (highest to lowest)
	int priority;
	for(priority = TASK_PRIORITY_COUNT - 1; priority >= 0; priority--)
	{
		LinkedList* queue = _Scheduler->priority_queues[priority];
		if(queue == NULL || queue->size == 0)
			continue;

		// Execute all tasks at this priority level
		// Safe to use LinkedList_foreach since we don't modify the list during iteration
		LinkedList_foreach(queue, node)
		{
			Task* task = (Task*)node->item;
			if(task != NULL && task->callback != NULL && !task->destroy)
			{
				task->callback(task->context, _MonTime);
			}
		}

		// Remove tasks marked for deletion (deferred deletion pattern)
		Node* node = queue->head;
		while(node != NULL)
		{
			Node* next = node->front;  
			Task* task = (Task*)node->item;

			if(task != NULL && task->destroy)
			{
				LinkedList_remove(queue, node, free);
				_Scheduler->total_tasks--;
			}

			node = next;
		}
	}
}

int TaskScheduler_GetTaskCount(TaskScheduler* _Scheduler)
{
	if(_Scheduler == NULL)
		return 0;

	return (int)_Scheduler->total_tasks;
}

int TaskScheduler_GetTaskCountByPriority(TaskScheduler* _Scheduler, TaskPriority _Priority)
{
	if(_Scheduler == NULL)
		return 0;

	if(_Priority < 0 || _Priority >= TASK_PRIORITY_COUNT)
		return 0;

	LinkedList* queue = _Scheduler->priority_queues[_Priority];
	if(queue == NULL)
		return 0;

	return (int)queue->size;
}
