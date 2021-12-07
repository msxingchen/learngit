
//��Դ���ҵļ���ʱ������
//�����ļ������api��
//ztask.h:
#ifndef _ZTASK_H
#define _ZTASK_H

#define ZT_MAX_TASKS 4         //ms:�����������

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
	zt_func_t func;                //ms:������
	unsigned long timeout, repeat; //ms:�������ʱ��,������ʱ��
	int en;                        //ms:����ʹ�ܱ�־
} zt_task_t;

static struct {
	unsigned long ticks;            //ms:��ʱ����������
	int num_tasks;                  //ms:��������������
	zt_task_t tasks[ZT_MAX_TASKS];
} g;

void zt_tick(void)
{
	// overflow not handled. for 1ms tick, it could run 2^32 * 1ms = ~50 days.
	g.ticks++;
}
//ms:������ѯ
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
//ms:��������
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

/*************************ms:ʹ��˵��***************************/
//ʹ�÷�����
//����������������
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

//�ڽ�����ѭ��ǰ��
zt_bind(blink, 100, 1);	// ÿ100msִ��һ��
zt_bind(hello, 500, 1);	// ÿ500msִ��һ��

//�������ѭ������� zt_poll(), ��systick�ж������zt_tick()�� �����ˡ�

//�����Ҫ������������/ֹͣ�� ��Ҫ��һ����������zt_bind�ķ���ֵ��Ȼ��ִ��zt_start��zt_stop���ɡ�





