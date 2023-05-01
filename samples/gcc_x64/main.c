#include <stdio.h>
#include <unistd.h>

#include "coroutine_kernel.h"

void do_something1(struct CoroutineHandle* const handle, void* const parameters)
{
    static int i;

    coroutine_begin(handle);

    // coroutine_begin~1個目のcoroutine_yieldの間で変数を初期化する
    i = 0;

    printf("[INFO] do_something1():hello 1\n");
#if 0
    coroutine_yield(handle);

    printf("[INFO] do_something1():hello 2\n");
#endif
    coroutine_end(handle);
}

void do_something2(struct CoroutineHandle* const handle, void* const parameters)
{
    static int i;

    coroutine_begin(handle);

    // coroutine_begin~1個目のcoroutine_yieldの間で変数を初期化する
    i = 0;

    printf("[INFO] do_something2():hello 1\n");
#if 0
    coroutine_yield(handle);

    printf("[INFO] do_something2():hello 2\n");

    coroutine_yield(handle);

    printf("[INFO] do_something2():hello 3\n");
#endif
    coroutine_end(handle);
}

int main()
{
    CoroutineErrorCode error_code = COROUTINE_ERROR_SUCCESS;
    struct CoroutineHandle handle1, handle2;
    
    error_code = coroutine_init_kernel();
    if(error_code != COROUTINE_ERROR_SUCCESS)
    {
        printf("[ERROR] coroutine_init_kernel():%d\n", error_code);
        return 1;
    }
    
    error_code = coroutine_register_task(do_something1, NULL, 5, 5, &handle1);
    if(error_code != COROUTINE_ERROR_SUCCESS)
    {
        printf("[ERROR] coroutine_register_task():%d\n", error_code);
        return 2;
    }

    error_code = coroutine_register_task(do_something2, NULL, 10, 4, &handle2);
    if(error_code != COROUTINE_ERROR_SUCCESS)
    {
        printf("[ERROR] coroutine_register_task():%d\n", error_code);
        return 3;
    }

    for(CoroutineUnsignedInteger i = 0; i < 72; i ++)
    {
        error_code = coroutine_spin_once();

        usleep(1000000);
    }

    return 0;
}