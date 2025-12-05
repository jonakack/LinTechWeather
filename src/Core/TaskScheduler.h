#ifndef __TaskScheduler_h_
#define __TaskScheduler_h_

#include <stdint.h>
#include "LinkedList.h"

/**
 * Task priority levels - higher values execute first
 */
typedef enum {
	TASK_PRIORITY_LOW = 0,
	TASK_PRIORITY_NORMAL = 1,
	TASK_PRIORITY_HIGH = 2,
	TASK_PRIORITY_CRITICAL = 3,
	TASK_PRIORITY_COUNT = 4  // Number of priority levels
} TaskPriority;

/**
 * Task structure representing a single scheduled task
 */
typedef struct
{
	void* context;
	void (*callback)(void* context, uint64_t monTime);
	TaskPriority priority;
	int destroy;  // Flag: 1 if task should be destroyed after current work cycle
} Task;

/**
 * Instance-based task scheduler with dynamic allocation and prioritization
 * Tasks are organized by priority level for efficient execution
 */
typedef struct
{
	LinkedList* priority_queues[TASK_PRIORITY_COUNT];  // One queue per priority level
	size_t total_tasks;  // Total number of tasks across all priorities
} TaskScheduler;

/**
 * Create a new task scheduler instance
 * Returns NULL on allocation failure
 */
TaskScheduler* TaskScheduler_Create();

/**
 * Dispose of a task scheduler instance and all its tasks
 * Double pointer is used to prevent dangling pointers
 */
void TaskScheduler_Dispose(TaskScheduler** _Scheduler);

/**
 * Create a new task with specified priority
 * Returns pointer to created task, or NULL if allocation fails
 *
 * @param _Scheduler The scheduler instance
 * @param _Context User context passed to callback
 * @param _Callback Function to call when task executes
 * @param _Priority Priority level for the task
 */
Task* TaskScheduler_CreateTask(TaskScheduler* _Scheduler, void* _Context,
                                void (*_Callback)(void* _Context, uint64_t _MonTime),
                                TaskPriority _Priority);

/**
 * Destroy a task and remove it from the scheduler
 *
 * @param _Scheduler The scheduler instance
 * @param _Task Task to destroy
 */
void TaskScheduler_DestroyTask(TaskScheduler* _Scheduler, Task* _Task);

/**
 * Execute all scheduled tasks in priority order (highest first)
 *
 * @param _Scheduler The scheduler instance
 * @param _MonTime Monotonic timestamp to pass to task callbacks
 */
void TaskScheduler_Work(TaskScheduler* _Scheduler, uint64_t _MonTime);

/**
 * Get total number of tasks across all priority levels
 *
 * @param _Scheduler The scheduler instance
 * @return Total task count
 */
int TaskScheduler_GetTaskCount(TaskScheduler* _Scheduler);

/**
 * Get number of tasks at a specific priority level
 *
 * @param _Scheduler The scheduler instance
 * @param _Priority Priority level to query
 * @return Task count for that priority
 */
int TaskScheduler_GetTaskCountByPriority(TaskScheduler* _Scheduler, TaskPriority _Priority);

#endif //__TaskScheduler_h_
