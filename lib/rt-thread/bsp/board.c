/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 * 2024-05-17     Aldlss       modify for nexysA7
 */

#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "psp_api.h"
#include "bsp_printf.h"
#include "bsp_timer.h"
#include "board.h"

extern int printUartPutchar(char ch);
void SW_handler();

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 10240
static uint32_t rt_heap[RT_HEAP_SIZE];     // heap default size: 4K(1024 * 4)
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

void SysTick_Handler(void)
{
    pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);

    /* enter interrupt */
    rt_interrupt_enter();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();

    pspEnableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);
    pspTimerCounterSetupAndRun(E_MACHINE_TIMER, M_MSEC_TO_CYCLES(1));
}

/**
 * @brief bspTimerSetup - Setup & activates core's timer
 *
 * @param void
 *
 */
void SystemCoreClockUpdate(void)
{
    /* INITIALIZE THE INTERRUPT SYSTEM */
    pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);
    pspInterruptsSetVectorTableAddress(&M_PSP_VECT_TABLE);

    // /* register timer interrupt handler */
    pspRegisterInterruptHandler(SW_handler, E_MACHINE_TIMER_CAUSE);

    // reffer to https://blog.csdn.net/ty1121466568/article/details/120455709#2.4%C2%A0%E6%97%B6%E9%92%9F%E8%8A%82%E6%8B%8D%E7%9A%84%E9%85%8D%E7%BD%AE
    pspTimerCounterSetupAndRun(E_MACHINE_TIMER, M_MSEC_TO_CYCLES(10000));

    /* ENABLE INTERRUPTS */
    pspInterruptsEnable(); /* Enable all interrupts in mstatus CSR */
    pspEnableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);
}

void GPIO_Initialization(void)
{
    /* Configure LEDs and Switches */
    M_PSP_WRITE_REGISTER_32(GPIO_INOUT, 0xFFFF); /* GPIO_INOUT */
    M_PSP_WRITE_REGISTER_32(GPIO_LEDs, 0x0);     /* GPIO_LEDs */

    /* Configure GPIO interrupts */
    M_PSP_WRITE_REGISTER_32(RGPIO_INTE, 0x10000);  /* RGPIO_INTE */
    M_PSP_WRITE_REGISTER_32(RGPIO_PTRIG, 0x10000); /* RGPIO_PTRIG */
    M_PSP_WRITE_REGISTER_32(RGPIO_INTS, 0x0);      /* RGPIO_INTS */
    M_PSP_WRITE_REGISTER_32(RGPIO_CTRL, 0x1);      /* RGPIO_CTRL */

    M_PSP_WRITE_REGISTER_32(SegEn_ADDR, 0x0); /* Initialize the 7-Seg Displays */
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init()
{
    uartInit();

    /* INITIALIZE THE PERIPHERALS */
    GPIO_Initialization();                    /* Initialize the GPIO */
    M_PSP_WRITE_REGISTER_32(SegEn_ADDR, 0x0); /* Initialize the 7-Seg Displays */

    // /* System Clock Update */
    SystemCoreClockUpdate();

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

/* 实现 2：输出一个字符，系统函数，函数名不可更改 */
void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = rt_strlen(str);
    char ch;

    for (i = 0; i < size; i++)
    {
        ch = str[i];
        printUartPutchar(ch);
    }
}
