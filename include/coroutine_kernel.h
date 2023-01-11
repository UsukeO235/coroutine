#ifndef COROUTINE_KERNEL_H
#define COROUTINE_KERNEL_H

#include "coroutine_system_defines.h"
#include "coroutine_config.h"

#define coroutine_begin(handle) do{switch(handle->state){case 0:

#define coroutine_end(handle) } handle->state = COROUTINE_STATE_INITIAL;}while(0)

#define coroutine_yield(handle) do{handle->state = __LINE__; return;\
    case __LINE__:; }while(0)

struct CoroutineHandle
{
    CoroutineId id;
    CoroutineState state;
};

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

CoroutineErrorCode coroutine_init_kernel();

CoroutineErrorCode coroutine_register_task
(
    void(*)(struct CoroutineHandle* const, void* const),
    void* const,
    const CoroutineTaskPeriod,
    const CoroutineTaskPriority,
    struct CoroutineHandle*
);

CoroutineErrorCode coroutine_spin_once();

#endif