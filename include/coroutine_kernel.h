#ifndef COROUTINE_KERNEL_H
#define COROUTINE_KERNEL_H

#include "coroutine_system_defines.h"
#include "coroutine_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define coroutine_begin(handle) do{switch(handle->state){case 0:

#define coroutine_end(handle) } handle->state = COROUTINE_STATE_INITIAL;}while(0)

#define coroutine_yield(handle) do{handle->state = __LINE__; return;\
    case __LINE__:; }while(0)

struct CoroutineHandle
{
    CoroutineId id;
    CoroutineState state;
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

CoroutineErrorCode coroutine_delete_task
(
    struct CoroutineHandle* const
);

CoroutineErrorCode coroutine_spin_once();

/**
 * @brief Get the number of tasks currently registered
 * 
 * @return CoroutineUnsignedInteger Number of tasks
 */
CoroutineUnsignedInteger coroutine_get_number_of_tasks();

#ifdef __cplusplus
}
#endif

#endif