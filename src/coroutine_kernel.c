#include "coroutine_kernel.h"
#include <stddef.h>

struct CoroutineContext
{
    struct CoroutineHandle handle;
    void(*task)(struct CoroutineHandle* const, void* const);
    void* parameters;
    CoroutineTaskPeriod period;
    CoroutineTaskPriority priority;
    CoroutineTaskPeriod ticks_to_wait;
    CoroutineTaskPeriod ticks_task_delayed;

    CoroutineUnsignedInteger task_ready;
};

static struct CoroutineContext coroutine_contexts[COROUTINE_MAXIMUM_NUMBER_OF_TASKS + 1];
static CoroutineUnsignedInteger coroutine_number_of_tasks = 0;

bool coroutine_internal_is_handle_valid(const struct CoroutineHandle* const);
struct CoroutineContext* coroutine_internal_find_highest_priority_context();
void coroutine_internal_update_contexts();
void coroutine_internal_execute_task(const struct CoroutineContext);
void coroutine_internal_update_task_ready_state(struct CoroutineContext* const);

static void coroutine_idle_task(struct CoroutineHandle* const, void* const);

CoroutineErrorCode coroutine_init_kernel()
{
    if(coroutine_number_of_tasks != 0)
    {

    }

    coroutine_contexts[0].handle.id = 0;
    coroutine_contexts[0].handle.state = COROUTINE_STATE_INITIAL;
    coroutine_contexts[0].task = coroutine_idle_task;
    coroutine_contexts[0].parameters = NULL;
    coroutine_contexts[0].period = 1;
    coroutine_contexts[0].priority = COROUTINE_IDLE_TASK_PRIORITY;
    coroutine_contexts[0].ticks_to_wait = 0;
    coroutine_contexts[0].ticks_task_delayed = 0;

    coroutine_contexts[0].task_ready = 0;

    coroutine_number_of_tasks = 1;

    return COROUTINE_ERROR_SUCCESS;
}

CoroutineErrorCode coroutine_register_task
(
    void(*task)(struct CoroutineHandle* const, void* const),
    void* const parameters,
    const CoroutineTaskPeriod period,
    const CoroutineTaskPriority priority,
    struct CoroutineHandle* const handle
)
{
    if(!coroutine_is_kernel_initialized())
    {
        return COROUTINE_ERROR_KERNEL_NOT_INITIALIZED;
    }

    if(period <= 0)
    {
        return COROUTINE_ERROR_ARGUMENT_RANGE;
    }

    if(priority < COROUTINE_TASK_LOWEST_PRIORITY)
    {
        return COROUTINE_ERROR_ARGUMENT_RANGE;
    }

    if(priority <= COROUTINE_IDLE_TASK_PRIORITY)
    {
        return COROUTINE_ERROR_ARGUMENT_RANGE;
    }
    
    if(priority > COROUTINE_TASK_HIGHEST_PRIORITY)
    {
        return COROUTINE_ERROR_ARGUMENT_RANGE;
    }



    if(coroutine_number_of_tasks >= COROUTINE_MAXIMUM_NUMBER_OF_TASKS + 1)
    {

    }


    coroutine_contexts[coroutine_number_of_tasks].handle.id = coroutine_number_of_tasks;
    coroutine_contexts[coroutine_number_of_tasks].handle.state = COROUTINE_STATE_INITIAL;
    coroutine_contexts[coroutine_number_of_tasks].task = task;
    coroutine_contexts[coroutine_number_of_tasks].parameters = parameters;
    coroutine_contexts[coroutine_number_of_tasks].period = period;
    coroutine_contexts[coroutine_number_of_tasks].priority = priority;
    coroutine_contexts[coroutine_number_of_tasks].ticks_to_wait = 0;
    coroutine_contexts[coroutine_number_of_tasks].ticks_task_delayed = 0;

    coroutine_contexts[coroutine_number_of_tasks].task_ready = 0;

    *handle = coroutine_contexts[coroutine_number_of_tasks].handle;  // 後でタスクを操作できるようにハンドルをコピー

    coroutine_number_of_tasks ++;

    return COROUTINE_ERROR_SUCCESS;
}

CoroutineErrorCode coroutine_delete_task
(
    struct CoroutineHandle* const handle
)
{
    if(!coroutine_is_kernel_initialized())
    {
        return COROUTINE_ERROR_KERNEL_NOT_INITIALIZED;
    }
    
    if(!coroutine_internal_is_handle_valid(handle))
    {
        return COROUTINE_ERROR_INVALID_HANDLE;
    }

    // 削除対象のタスクのコンテキストをコンテキスト配列の末尾要素で上書き
    coroutine_contexts[handle->id] = coroutine_contexts[coroutine_number_of_tasks-1];
    coroutine_number_of_tasks --;

    return COROUTINE_ERROR_SUCCESS;
}

CoroutineErrorCode coroutine_spin_once()
{
    if(!coroutine_is_kernel_initialized())
    {
        return COROUTINE_ERROR_KERNEL_NOT_INITIALIZED;
    }

    coroutine_internal_update_contexts();

    struct CoroutineContext* highest_priority_context = coroutine_internal_find_highest_priority_context();

    coroutine_internal_execute_task(*highest_priority_context);

    
    coroutine_internal_update_task_ready_state(highest_priority_context);

    

    return COROUTINE_ERROR_SUCCESS;
}

CoroutineUnsignedInteger coroutine_get_number_of_tasks()
{
    return coroutine_number_of_tasks;
}

bool coroutine_is_kernel_initialized()
{
    return (coroutine_number_of_tasks != 0);
}

bool coroutine_internal_is_handle_valid(const struct CoroutineHandle* const handle)
{
    if(handle == NULL)
    {
        return false;
    }
    
    if(handle->id > coroutine_number_of_tasks-1)
    {
        return false;
    }

    if(handle->id != coroutine_contexts[handle->id].handle.id)  // The specified task is a deleted task
    {
        return false;
    }

    return true;
}

struct CoroutineContext* coroutine_internal_find_highest_priority_context()
{
    CoroutineUnsignedInteger id = 0;

    for(CoroutineUnsignedInteger i = 0; i < coroutine_number_of_tasks; i ++)
    {
        
        if(coroutine_contexts[i].task_ready == 1)
        {
            if(coroutine_contexts[i].priority > coroutine_contexts[id].priority)
            {
                id = i;
            }
        }
    }

    return &(coroutine_contexts[id]);
}

void coroutine_internal_update_contexts()
{
    for(CoroutineUnsignedInteger i = 0; i < coroutine_number_of_tasks; i ++)
    {
        if(coroutine_contexts[i].ticks_to_wait <= 0)  // The task is ready to be executed
        {
            coroutine_contexts[i].task_ready = 1;
            coroutine_contexts[i].ticks_to_wait = coroutine_contexts[i].period - 1;
        }
        else
        {
            coroutine_contexts[i].ticks_to_wait --;
        }
    }
}

void coroutine_internal_execute_task(const struct CoroutineContext context)
{
    context.task((struct CoroutineHandle* const)(&(context.handle)), context.parameters);
}

void coroutine_internal_update_task_ready_state(struct CoroutineContext* const context)
{
    if(context->handle.state == COROUTINE_STATE_INITIAL)  // If the task is complete
    {
        context->task_ready = 0;
    }
}

static void coroutine_idle_task(struct CoroutineHandle* const handle, void* const parameters)
{
}