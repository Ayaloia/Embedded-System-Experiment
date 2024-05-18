#include"bsp_mem_map.h"
#include"psp_api.h"
#include "rtthread.h"

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

#define THREAD_STACK_SIZE 1024
#define THREAD_PRIORITY 5
#define THREAD_TIMESLICE 10

/* 线程入口 */
void thread_entry(void *parameter)
{
    rt_uint32_t value;
    rt_uint32_t count = 0;

    value = (rt_uint32_t)parameter;
    while (1)
    {
        if (value == 1)
        {
            M_PSP_WRITE_REGISTER_32(SegDig_ADDR, count);
        }
        if (0 == (count % 1000000))
        {
            rt_kprintf("thread %d is running ,thread %d count = %d\n", value, value, count);
        }
        // if (count == 100000)
        //     break;
        count++;
    }
    rt_kprintf("thread %d exit\n", value);
}

void LedCtrol()
{
    rt_uint32_t sws = 0;
    while (1)
    {
        if (sws == 0)
            sws = 0xFFFF0000;
        sws = sws >> 1;
        M_PSP_WRITE_REGISTER_32(GPIO_LEDs, sws | (M_PSP_READ_REGISTER_32(GPIO_SWs) >> 16));
        rt_thread_mdelay(10);
    }
}

int timeslice_sample(void)
{
    rt_thread_t tid = RT_NULL;
    /* 创建线程 1 */
    tid = rt_thread_create("t1", thread_entry, (void *)1, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE - 8);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    /* 创建线程 2 */
    tid = rt_thread_create("t2", thread_entry, (void *)2, THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE - 9);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    tid = rt_thread_create("t3", LedCtrol, RT_NULL, 512, THREAD_PRIORITY, THREAD_TIMESLICE - 9);
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    return 0;
}

int main() {
    M_PSP_WRITE_REGISTER_32(SegDig_ADDR, 116814);
    rt_thread_mdelay(1);
    timeslice_sample();
}
