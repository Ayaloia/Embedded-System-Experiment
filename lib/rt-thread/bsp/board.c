/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-07-24     Tanek        the first version
 * 2018-11-12     Ernest Chen  modify copyright
 */
 
#include <stdint.h>
#include <rthw.h>
#include <rtthread.h>
#include "psp_api.h"
#include "bsp_printf.h"
#include "bsp_timer.h"
#include "bsp_external_interrupts.h"
#include "demo_platform_al.h"

#define SegEn_ADDR 0x80001038
#define SegDig_ADDR 0x8000103C

#define GPIO_SWs 0x80001400
#define GPIO_LEDs 0x80001404
#define GPIO_INOUT 0x80001408
#define RGPIO_INTE 0x8000140C
#define RGPIO_PTRIG 0x80001410
#define RGPIO_CTRL 0x80001418
#define RGPIO_INTS 0x8000141C

#define RPTC_CNTR 0x80001200
#define RPTC_HRC 0x80001204
#define RPTC_LRC 0x80001208
#define RPTC_CTRL 0x8000120c

#define Select_INT 0x80001018

#define M_MSEC_TO_CYCLES(duration) (duration * (D_CLOCK_RATE / D_PSP_MSEC))

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

int countttt = 0;

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
    // u32_t no;
    // pspInterruptsDisable(&no);
    pspDisableInterruptNumberMachineLevel(D_PSP_INTERRUPTS_MACHINE_TIMER);
    pspInterruptsSetVectorTableAddress(&M_PSP_VECT_TABLE);

    // /* register timer interrupt handler */
    pspRegisterInterruptHandler(SW_handler, E_MACHINE_TIMER_CAUSE);
    // pspRegisterExceptionHandler(SysTick_Handler, E_MACHINE_TIMER_CAUSE);

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
