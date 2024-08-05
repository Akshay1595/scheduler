/**
 * @file        scheduler.c
 * @author      Akshay Godase (akshagodase15@gmail.com)
 * @brief       Simple round robin scheduler which will have fixed tasks
 *              that will executed for fixed time.
 * @version     0.1
 * @date        2024-08-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

/* ########################################################################### */
/* INCLUDES */
/* ########################################################################### */

#include <windows.h>
#include <stdio.h>
#include <stdint.h>

/* ########################################################################### */
/* DEFINES */
/* ########################################################################### */

#define MAX_TASKS       (3u)

/* ########################################################################### */
/* LOCAL TYPES */
/* ########################################################################### */

/**
 * @brief This is a structure that will define task 
 */
typedef struct
{
    /** Holds taskId */
    uint32_t            u32_TaskId;
    /** Holds task_handler to be called */
    void                (*task_handler)(void);
    /** Hodls Total time expected to be executed for */
    const uint32_t      cu32_TimeInSecs;
    /** Holds Time elapsed till now */
    uint32_t            u32_TimeElapsed;
} task_t;

/* ########################################################################### */
/* LOCAL DATA */
/* ########################################################################### */

static task_t task_queue[MAX_TASKS] =
{
    { 0u,  &task_1, 2u, 0u},
    { 1u,  &task_2, 5u, 0u},
    { 2u,  &task_3, 3u, 0u}
};

/** This variable will hold number of seconds elapsed */
static volatile uint32_t u32_CurrSecs = 0u;
static volatile UINT_PTR g_timerId = NULL;
static volatile uint32_t task_executing = 0u;

/* ########################################################################### */
/* LOCAL PROTOTYPES */
/* ########################################################################### */

static void scheduler_start(uint32_t u32_NoOfSecsToExecute);
static void scheduler_stop(void);
static void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static void scheduler_check(void);
static void task_1(void);
static void task_2(void);
static void task_3(void);

/* ########################################################################### */
/* LOCAL FUNCTIONS */
/* ########################################################################### */

/** Handlers for each task that will be called */
static void task_1(void)
{
    printf("Executing Task 1\n");
}

static void task_2(void)
{
    printf("Executing Task 2\n");
}

static void task_3(void)
{
    printf("Executing Task 3\n");
}

/**
 * @brief This fucntion will be called whenever timer interrupt is called
 *        It will check if task_switch is needed or not based on the current
 *        situation or task being executed.
 */
static void scheduler_check(void)
{
    if ((task_queue[task_executing].cu32_TimeInSecs - 1u) > task_queue[task_executing].u32_TimeElapsed)
    {
        task_queue[task_executing].u32_TimeElapsed += 1u;
    }
    else
    {
        printf("Task Stopped = %d \n", task_executing);
        task_queue[task_executing].u32_TimeElapsed = 0u;
        task_executing = (task_executing == (MAX_TASKS - 1u)) ? 0u : (task_executing + 1u); 
        task_queue[task_executing].task_handler();
    }
}

/**
 * @brief This function will start the scheduler.
 * 
 * @param u32_NoOfSecsToExecute Number of seconds scheduler should run
 */
static void scheduler_start(uint32_t u32_NoOfSecsToExecute)
{
    // Create a timer that triggers every 1000 ms (1 second)
    g_timerId = SetTimer(NULL, 0, 1000, (TIMERPROC)TimerProc);
    
    if (g_timerId == 0)
    {
        fprintf(stderr, "Failed to create timer!\n");
        return;
    }

    task_queue[task_executing].task_handler();
    // Run a message loop to keep the application alive and process timer messages.
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (u32_CurrSecs >= u32_NoOfSecsToExecute)
        {
            scheduler_stop();
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

/**
 * @brief This function will stop the scheduler.
 */
static void scheduler_stop(void)
{
    // Cleanup the timer when done
    KillTimer(NULL, g_timerId);
}

static void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    // This function is called every time the timer elapses.
    printf("Timer triggered for sec = %u!\n", u32_CurrSecs++);
    printf("Task executing = %d for time: %d time elapsed %d \n", task_executing, task_queue[task_executing].cu32_TimeInSecs, task_queue[task_executing].u32_TimeElapsed);
    // You can call your desired function here.
    scheduler_check();
}

int main()
{
    scheduler_start(20);
    return 0;
}