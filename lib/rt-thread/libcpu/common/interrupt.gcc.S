/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-05-17     Aldlss        the first version
 */

#include "cpuport.h"

.global SW_handler
SW_handler:
    /* there not defined D_PSP_VECT_TABLE */

    /* Restore core standard registers,
    Skip global pointer because that does not change */
    /* 把刚存的读出来，因为有过 jump，可能值不一样 */
    LOAD    x1,   3 * REGBYTES(sp)
    LOAD    x5,   4 * REGBYTES(sp)
    LOAD    x6,   5 * REGBYTES(sp)
    LOAD    x7,   6 * REGBYTES(sp)
    LOAD    x8,   7 * REGBYTES(sp)
    LOAD    x9,   8 * REGBYTES(sp)
    LOAD    x10,  9 * REGBYTES(sp)
    LOAD    x11, 10 * REGBYTES(sp)
    LOAD    x12, 11 * REGBYTES(sp)
    LOAD    x13, 12 * REGBYTES(sp)
    LOAD    x14, 13 * REGBYTES(sp)
    LOAD    x15, 14 * REGBYTES(sp)
    LOAD    x16, 15 * REGBYTES(sp)
    LOAD    x17, 16 * REGBYTES(sp)
    LOAD    x18, 17 * REGBYTES(sp)
    LOAD    x19, 18 * REGBYTES(sp)
    LOAD    x20, 19 * REGBYTES(sp)
    LOAD    x21, 20 * REGBYTES(sp)
    LOAD    x22, 21 * REGBYTES(sp)
    LOAD    x23, 22 * REGBYTES(sp)
    LOAD    x24, 23 * REGBYTES(sp)
    LOAD    x25, 24 * REGBYTES(sp)
    LOAD    x26, 25 * REGBYTES(sp)
    LOAD    x27, 26 * REGBYTES(sp)
    LOAD    x28, 27 * REGBYTES(sp)
    LOAD    x30, 29 * REGBYTES(sp)
#ifndef D_COMRV
    LOAD    x29, 28 * REGBYTES(sp)
    LOAD    x31, 30 * REGBYTES(sp)
#else
   LOAD     x4,  28  * REGBYTES(sp)
#endif /* D_COMRV */
    /* Make sure SP is aligned to 16 */
/*    addi    sp, sp, REGBYTES * 32

/*  不加是因为 进入之前加过，但是它那个 push 的对不上，重新 push 一遍覆盖
/*    addi sp, sp, -32 * REGBYTES*/

    csrr  a0, mepc
    STORE a0,   0 * REGBYTES(sp)
    STORE x1,   1 * REGBYTES(sp)
    li    t0,   0x80
    STORE t0,   2 * REGBYTES(sp)

    STORE x4,   4 * REGBYTES(sp)
    STORE x5,   5 * REGBYTES(sp)
    STORE x6,   6 * REGBYTES(sp)
    STORE x7,   7 * REGBYTES(sp)
    STORE x8,   8 * REGBYTES(sp)
    STORE x9,   9 * REGBYTES(sp)
    STORE x10, 10 * REGBYTES(sp)
    STORE x11, 11 * REGBYTES(sp)
    STORE x12, 12 * REGBYTES(sp)
    STORE x13, 13 * REGBYTES(sp)
    STORE x14, 14 * REGBYTES(sp)
    STORE x15, 15 * REGBYTES(sp)
    STORE x16, 16 * REGBYTES(sp)
    STORE x17, 17 * REGBYTES(sp)
    STORE x18, 18 * REGBYTES(sp)
    STORE x19, 19 * REGBYTES(sp)
    STORE x20, 20 * REGBYTES(sp)
    STORE x21, 21 * REGBYTES(sp)
    STORE x22, 22 * REGBYTES(sp)
    STORE x23, 23 * REGBYTES(sp)
    STORE x24, 24 * REGBYTES(sp)
    STORE x25, 25 * REGBYTES(sp)
    STORE x26, 26 * REGBYTES(sp)
    STORE x27, 27 * REGBYTES(sp)
    STORE x28, 28 * REGBYTES(sp)
    STORE x29, 29 * REGBYTES(sp)
    STORE x30, 30 * REGBYTES(sp)
    STORE x31, 31 * REGBYTES(sp)

    /* switch to interrupt stack */
    /*call  rt_interrupt_enter*/
    
    la    zero, rt_thread_switch_interrupt_flag

    /* 准备直接 mret */
    /*addi sp, sp, -1 * REGBYTES
    STORE ra, 0 * REGBYTES(sp)*/
    csrrw zero, mscratch, sp
    
    call SysTick_Handler
    
    csrrw sp, mscratch, zero
    /*LOAD ra, 0 * REGBYTES(sp)
    addi sp, sp, 1 * REGBYTES*/
    
    /*call  rt_interrupt_leave*/
    /* switch to from thread stack */

    /* if rt_thread_switch_interrupt_flag=1,then clear it  */
    la    s0, rt_thread_switch_interrupt_flag
    LOAD  s1, 0(s0)
    beqz  s1, spurious_interrupt

    sw    zero, 0(s0)
    
    /* s0 = &rt_interrupt_from_thread */
    la    s0, rt_interrupt_from_thread
    /* s1 = rt_interrupt_from_thread(&from_sp)  */
    LOAD  s1, 0(s0)
    /* *(&from_sp) = sp */
    STORE sp, 0(s1)

    /* s0 = &rt_interrupt_to_thread */
    la    s0, rt_interrupt_to_thread
    /* s1 = rt_interrupt_to_thread(&to_sp)  */
    LOAD  s1, 0(s0)
    /* sp = *(&to_sp) */
    LOAD  sp, 0(s1)

spurious_interrupt:
    /* a0 = *(u32*)sp */
    LOAD  s0,  0 * REGBYTES(sp)
    /* mepc = a0 */
    csrw  mepc, s0

    LOAD  x1,   1 * REGBYTES(sp)

    /* Remain in M-mode after mret */
    li    t0, 0x00001800
    csrs  mstatus, t0
    LOAD  t0,   2 * REGBYTES(sp)
    csrs  mstatus, t0

    LOAD  x4,   4 * REGBYTES(sp)
    LOAD  x5,   5 * REGBYTES(sp)
    LOAD  x6,   6 * REGBYTES(sp)
    LOAD  x7,   7 * REGBYTES(sp)
    LOAD  x8,   8 * REGBYTES(sp)
    LOAD  x9,   9 * REGBYTES(sp)
    LOAD  x10, 10 * REGBYTES(sp)
    LOAD  x11, 11 * REGBYTES(sp)
    LOAD  x12, 12 * REGBYTES(sp)
    LOAD  x13, 13 * REGBYTES(sp)
    LOAD  x14, 14 * REGBYTES(sp)
    LOAD  x15, 15 * REGBYTES(sp)
    LOAD  x16, 16 * REGBYTES(sp)
    LOAD  x17, 17 * REGBYTES(sp)
    LOAD  x18, 18 * REGBYTES(sp)
    LOAD  x19, 19 * REGBYTES(sp)
    LOAD  x20, 20 * REGBYTES(sp)
    LOAD  x21, 21 * REGBYTES(sp)
    LOAD  x22, 22 * REGBYTES(sp)
    LOAD  x23, 23 * REGBYTES(sp)
    LOAD  x24, 24 * REGBYTES(sp)
    LOAD  x25, 25 * REGBYTES(sp)
    LOAD  x26, 26 * REGBYTES(sp)
    LOAD  x27, 27 * REGBYTES(sp)
    LOAD  x28, 28 * REGBYTES(sp)
    LOAD  x29, 29 * REGBYTES(sp)
    LOAD  x30, 30 * REGBYTES(sp)
    LOAD  x31, 31 * REGBYTES(sp)

    addi  sp, sp, 32 * REGBYTES
    /* 直接 m 返回，不走之后那些了 */
    mret
