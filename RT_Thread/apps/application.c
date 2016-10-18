
/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef RT_USING_DFS
/* dfs init */
#include <dfs_init.h>
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_LWIP
#include <lwip/sys.h>
#include <lwip/api.h>
#include <netif/ethernetif.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#endif

#include "led.h"


ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[ 512 ];
static struct rt_thread led_thread;
static void led_thread_entry(void* parameter)
{
    unsigned int count=0;

    rt_hw_led_init();

    while (1)
    {
        /* led1 on */
        rt_kprintf("led0 on, count : %d\r\n",count);
        count++;
        rt_hw_led_on(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

        /* led1 off */
        rt_kprintf("led0 off\r\n");
        rt_hw_led_off(0);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
    }
}

static rt_uint8_t led1_stack[ 512 ];
static struct rt_thread led1_thread;
static void led1_thread_entry(void* parameter)
{
    unsigned int count=0;

    rt_hw_led_init();

    while (1)
    {
        /* led1 on */

        rt_kprintf("led1 on, count : %d\r\n",count);
        count++;
			  rt_hw_led_on(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

        /* led1 off */
        rt_kprintf("led1 off\r\n");
			  rt_hw_led_off(1);
        rt_thread_delay( RT_TICK_PER_SECOND/2 );
    }
}


/***********************************************************************************************************************/
//rt_thread_t tid1 = RT_NULL;  
//rt_thread_t tid2 = RT_NULL;  

//static void thread1_entry(void* parameter)  
//{  
//   rt_uint32_t count = 0;  
//    rt_kprintf("thread1 dynamicly created ok\n");  
//    while(1)  
//    {  
//        rt_kprintf("thread1 count: %d\n",count++);  
//        rt_thread_delay(RT_TICK_PER_SECOND);  
//    }  
//}  


//static void thread2_entry(void* parameter)  
//{  
//    rt_kprintf("thread2 dynamicly created ok\n");  
//  
//    rt_thread_delay(RT_TICK_PER_SECOND * 4);  
//      
//    rt_thread_delete(tid1);  
//    rt_kprintf("thread1 deleted ok\n");  
//}  


/*****************************************************************************************************************************/

void rt_init_thread_entry(void* parameter)
{
/* Filesystem Initialization */
#ifdef RT_USING_DFS
	{
		/* init the device filesystem */
		dfs_init();

#ifdef RT_USING_DFS_ELMFAT
		/* init the elm chan FatFs filesystam*/
		elm_init();

		/* mount sd card fat partition 1 as root directory */
		if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
		{
			rt_kprintf("File System initialized!\n");
		}
		else
			rt_kprintf("File System initialzation failed!\n");
#endif
	}
#endif

/* LwIP Initialization */
#ifdef RT_USING_LWIP
	{
		extern void lwip_sys_init(void);

		/* register ethernetif device */
		eth_system_device_init();

#ifdef STM32F10X_CL
		rt_hw_stm32_eth_init();
#else
	/* STM32F103 */
	#if STM32_ETH_IF == 0
			rt_hw_enc28j60_init();
	#elif STM32_ETH_IF == 1
			rt_hw_dm9000_init();
	#endif
#endif

		/* re-init device driver */
		rt_device_init_all();

		/* init lwip system */
		lwip_sys_init();
		rt_kprintf("TCP/IP initialized!\n");
	}
#endif

#ifdef RT_USING_RTGUI
	{
	    extern void rtgui_system_server_init(void);
	    extern void rt_hw_lcd_init();
	    extern void rtgui_touch_hw_init(void);

		rt_device_t lcd;

		/* init lcd */
		rt_hw_lcd_init();

		/* init touch panel */
		rtgui_touch_hw_init();

		/* re-init device driver */
		rt_device_init_all();

		/* find lcd device */
		lcd = rt_device_find("lcd");

		/* set lcd device as rtgui graphic driver */
		rtgui_graphic_set_device(lcd);

		/* init rtgui system server */
		rtgui_system_server_init();
	}
#endif /* #ifdef RT_USING_RTGUI */
}

int rt_application_init()
{
		
	/******************************************************************************************************************/
//	/*创建动态线程*/
//	tid1 = rt_thread_create("thread1",  
//        thread1_entry,   
//        RT_NULL,  
//        512, 6, 10);  
//    if (tid1 != RT_NULL)  
//        rt_thread_startup(tid1);  
//  
//    tid2 = rt_thread_create("thread2",  
//        thread2_entry, RT_NULL,  
//        512, 6, 10);  
//    if (tid2 != RT_NULL)  
//        rt_thread_startup(tid2);  
//  
//    return 0;  
//}
	/******************************************************************************************************************/
	
	
	rt_thread_t init_thread;

	rt_err_t result;

    /* init led thread */
	result = rt_thread_init(&led_thread,
		"led",
		led_thread_entry, RT_NULL,
		(rt_uint8_t*)&led_stack[0], sizeof(led_stack), 20, 5);
	if (result == RT_EOK)
	{
        rt_thread_startup(&led_thread);
	}
	

	
		result = rt_thread_init(&led1_thread,
		"led1",
		led1_thread_entry, RT_NULL,
		(rt_uint8_t*)&led1_stack[0], sizeof(led1_stack), 20, 8);
	if (result == RT_EOK)
	{
        rt_thread_startup(&led1_thread);
	}
	
	
	
	

#if (RT_THREAD_PRIORITY_MAX == 32)
	init_thread = rt_thread_create("init",
								rt_init_thread_entry, RT_NULL,
								2048, 8, 20);
#else
	init_thread = rt_thread_create("init",
								rt_init_thread_entry, RT_NULL,
								2048, 80, 20);
#endif

	if (init_thread != RT_NULL)
		rt_thread_startup(init_thread);

	return 0;
}

/*@}*/





