# coroutine

コルーチンのC言語実装。

C implementation of coroutine.

## コルーチンとは / What is Coroutine

マルチタスクを実現するためのプログラム構造の一種。

A coroutine is a kind of program structures for multitasking.

## コルーチンの長所と短所 / Pros and Cons of Coroutine

## 使用方法 / How to Use

### プロジェクトに必要なファイルをコピーする / Copy necessary files into your project

includeフォルダとsrcフォルダを中身ごとコピーし、自分のプロジェクトフォルダにペーストする。

Copy "include" and "src" folders with their contents into your project folder.

```
your_project_root/
┣coroutine/
┃ ┣include/
┃ ┃ ┣coroutine_config.h
┃ ┃ ┣coroutine_kernel.h
┃ ┃ ┗coroutine_system_defines.h
┃ ┗src/
┃   ┗coroutine_kernel.c
┗main.c
```

### coroutine_config.hの編集 / Modify coroutine_config.h

### ヘッダファイルのインクルード / Include necessary header files

```c
#include "coroutine_kernel.h"
```

### タスクの作成 / Create tasks

```c
void do_something(struct CoroutineHandle* const handle, void* const parameters)
{
    static int i;

    coroutine_begin(handle);

    // coroutine_begin()の直後でstatic変数を初期化する / Be sure to initialize static variables immediately after calling coroutine_begin().
    i = 0;

    printf("[INFO] do_something():hello 1\n");

    coroutine_yield(handle);

    printf("[INFO] do_something1():hello 2\n");

    coroutine_end(handle);
}
```

### コルーチンカーネルの初期化とタスクの登録 / Initialize the coroutine kernel and register tasks

```c
int main(void)
{
    CoroutineErrorCode error_code = COROUTINE_ERROR_SUCCESS;
    struct CoroutineHandle handle;
    
    // コルーチンカーネルの初期化 / Initialize the coroutine kernel
    error_code = coroutine_init_kernel();

    // タスクの登録 / Register tasks
    error_code = coroutine_register_task(do_something, NULL, 5, 5, &handle);

    return 0;
}
```

### タスクの実行 / Execute tasks

```c
int main(void)
{
    CoroutineErrorCode error_code = COROUTINE_ERROR_SUCCESS;
    struct CoroutineHandle handle;
    
    // コルーチンカーネルの初期化 / Initialize the coroutine kernel
    error_code = coroutine_init_kernel();

    // タスクの登録 / Register tasks
    error_code = coroutine_register_task(do_something, NULL, 5, 5, &handle);

    // タスクの実行 / Execute tasks
    while(1)
    {
        error_code = coroutine_spin_once();
        wait_ms(1);  // ユーザ定義またはベンダ提供の関数 / User defined or vender provided function
    }

    return 0;
}
```