#include "coroutine_kernel.h"
#include <stddef.h>

#include <stdio.h>

static struct CoroutineContext coroutine_contexts[COROUTINE_MAXIMUM_NUMBER_OF_TASKS + 1];
static CoroutineUnsignedInteger coroutine_number_of_tasks = 0;

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
    coroutine_contexts[0].period = 0;
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
    struct CoroutineHandle* handle
)
{
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

CoroutineErrorCode coroutine_spin_once()
{
    CoroutineUnsignedInteger id = 0;

    if(coroutine_number_of_tasks == 0)
    {
        return COROUTINE_ERROR_KERNEL_NOT_INITIALIZED;
    }

    for(CoroutineUnsignedInteger i = 0; i < coroutine_number_of_tasks; i ++)
    {
        if(coroutine_contexts[i].ticks_to_wait <= 0)
        {
            coroutine_contexts[i].task_ready = 1;

            coroutine_contexts[i].ticks_to_wait = coroutine_contexts[i].period - 1;  // カウンタセット
        }
        else
        {
            coroutine_contexts[i].ticks_to_wait --;
        }

        if(coroutine_contexts[i].task_ready == 1)  // タスクが実行可能状態の場合
        {
            // 優先度最大のタスクを探す
            if(coroutine_contexts[i].priority > coroutine_contexts[id].priority)
            {
                id = i;
            }
        }

        printf("%u,%u,", coroutine_contexts[i].ticks_to_wait, coroutine_contexts[i].ticks_task_delayed);
    }

    //printf(" %d, ", id);
    coroutine_contexts[id].task(&(coroutine_contexts[id].handle), &(coroutine_contexts[id].parameters));  // タスク実行

    
    if(coroutine_contexts[id].handle.state == COROUTINE_STATE_INITIAL)  // タスクが完了した場合
    {
        coroutine_contexts[id].task_ready = 0;  // タスクを待ち状態にする
    }

    

    return COROUTINE_ERROR_SUCCESS;
}

static void coroutine_idle_task(struct CoroutineHandle* const handle, void* const parameters)
{
    printf("[INFO] idle ...\n");
}