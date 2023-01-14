#ifndef COROUTINE_CONFIG_H
#define COROUTINE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define COROUTINE_TASK_LOWEST_PRIORITY 1
#define COROUTINE_TASK_HIGHEST_PRIORITY 7

#define COROUTINE_MAXIMUM_NUMBER_OF_TASKS 16

typedef int CoroutineSignedInteger;
typedef unsigned int CoroutineUnsignedInteger;

typedef CoroutineUnsignedInteger CoroutineId;
typedef CoroutineUnsignedInteger CoroutineState;
typedef int CoroutineErrorCode;
typedef unsigned int CoroutineTaskPeriod;
typedef unsigned int CoroutineTaskPriority;

#ifdef __cplusplus
}
#endif

#endif