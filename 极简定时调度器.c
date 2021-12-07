
//开源个我的极简定时调度器
//就俩文件，五个api：
//ztask.h:
#ifndef _ZTASK_H
#define _ZTASK_H

#define ZT_MAX_TASKS 4         //ms:任务最大数量

typedef void (*zt_func_t)(void);

// should be called in main loop
void zt_poll(void);

// timeout: repeat inteval; en: start immediately or not
int zt_bind(zt_func_t func, int repeat, int en);

// should be called in systick_irqhandler
void zt_tick(void);

void zt_start(int id);
void zt_stop(int id);

#endif


//ztask.c:

#include "ztask.h"

typedef struct {
	zt_func_t func;                //ms:任务函数
	unsigned long timeout, repeat; //ms:任务溢出时间,任务间隔时间
	int en;                        //ms:任务使能标志
} zt_task_t;

static struct {
	unsigned long ticks;            //ms:定时器计数变量
	int num_tasks;                  //ms:创建的任务数量
	zt_task_t tasks[ZT_MAX_TASKS];
} g;

void zt_tick(void)
{
	// overflow not handled. for 1ms tick, it could run 2^32 * 1ms = ~50 days.
	g.ticks++;
}
//ms:任务轮询
void zt_poll(void)
{
	int i;
	for(i = 0; i < g.num_tasks; i++) 
	{
		if(g.ticks >= g.tasks[i].timeout) 
		{
			g.tasks[i].timeout = g.ticks + g.tasks[i].repeat;
			if(g.tasks[i].en)
				g.tasks[i].func();
		}
	}
}

void zt_stop(int id)
{
	if(id < ZT_MAX_TASKS)
		g.tasks[id].en = 0;
}

void zt_start(int id)
{
	if(id < ZT_MAX_TASKS)
		g.tasks[id].en = 1;
}
//ms:创建任务
int zt_bind(zt_func_t func, int repeat, int en)
{
	if(g.num_tasks < ZT_MAX_TASKS) 
	{
		g.tasks[g.num_tasks].func = func;
		g.tasks[g.num_tasks].repeat = repeat;
		g.tasks[g.num_tasks].timeout = 0;
		g.tasks[g.num_tasks].en = en;
		return g.num_tasks++;
	}
	else
		return -1;
}

/*************************ms:使用说明***************************/
//使用方法：
//定义两个任务函数：
void blink(void)
{
	GPIOA->ODR |= GPIO_PIN_1;
	_delay_us(10);
	GPIOA->ODR &= ~GPIO_PIN_1;
}

void hello(void)
{
	printf("Hello, world.\n");
}

//在进入主循环前：
zt_bind(blink, 100, 1);	// 每100ms执行一次
zt_bind(hello, 500, 1);	// 每500ms执行一次

//最后在主循环里调用 zt_poll(), 在systick中断里调用zt_tick()， 就行了。

//如果需要控制任务启动/停止， 需要用一个变量保存zt_bind的返回值，然后执行zt_start和zt_stop即可。





