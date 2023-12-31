
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"


/*======================================================================*
                            kernel_main
 *======================================================================*/
PUBLIC int kernel_main()
{
	disp_str("-----\"kernel_main\" begins-----\n");

	struct task* p_task;
	struct proc* p_proc= proc_table;
	char* p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16   selector_ldt = SELECTOR_LDT_FIRST;
        u8    privilege;
        u8    rpl;
	int   eflags;
	int   i;
	int   prio;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
	        if (i < NR_TASKS) {     /* 任务 */
                        p_task    = task_table + i;
                        privilege = PRIVILEGE_TASK;
                        rpl       = RPL_TASK;
                        eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
			prio      = 15;
                }
                else {                  /* 用户进程 */
                        p_task    = user_proc_table + (i - NR_TASKS);
                        privilege = PRIVILEGE_USER;
                        rpl       = RPL_USER;
                        eflags    = 0x202; /* IF=1, bit 2 is always 1 */
			prio      = 5;
                }

		strcpy(p_proc->name, p_task->name);	/* name of the process */
		p_proc->pid = i;			/* pid */

		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(struct descriptor));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		p_proc->nr_tty		= 0;

		p_proc->p_flags = 0;
		p_proc->p_msg = 0;
		p_proc->p_recvfrom = NO_TASK;
		p_proc->p_sendto = NO_TASK;
		p_proc->has_int_msg = 0;
		p_proc->q_sending = 0;
		p_proc->next_sending = 0;

		p_proc->ticks = p_proc->priority = prio;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

        proc_table[NR_TASKS + 0].nr_tty = 2;
        proc_table[NR_TASKS + 1].nr_tty = 2;
        proc_table[NR_TASKS + 2].nr_tty = 2;
	proc_table[NR_TASKS + 3].nr_tty = 2;

	k_reenter = 0;
	ticks = 0;

	p_proc_ready	= proc_table;

	init_clock();
        init_keyboard();

	restart();

	while(1){}
}


/*****************************************************************************
 *                                get_ticks
 *****************************************************************************/
PUBLIC int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type = GET_TICKS;
	send_recv(BOTH, TASK_SYS, &msg);
	return msg.RETVAL;
}


/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{
    while (1)
    {
        MESSAGE msg;
        do
        {
            reset_msg(&msg);
            msg.source = TEST_A;
            msg.type = REQUEST;
            send_recv(BOTH, TestD, &msg);
        } while (msg.type == REJECT);
        assert(msg.type == ACCEPT);
        printf("A is eating ");
        milli_delay(1000);
        reset_msg(&msg);
        msg.source = TEST_A;
        msg.type = FINISHED;
        send_recv(SEND, TestD, &msg);
    }
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
    {
        MESSAGE msg;
        do
        {
            reset_msg(&msg);
            msg.source = TEST_B;
            msg.type = REQUEST;
            send_recv(BOTH, TestD, &msg);
        } while (msg.type == REJECT);
        assert(msg.type == ACCEPT);
        printf("B is eating ");
        milli_delay(1000);
        reset_msg(&msg);
        msg.source = TEST_B;
        msg.type = FINISHED;
        send_recv(SEND, TestD, &msg);
    }
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestC()
{
    {
        MESSAGE msg;
        do
        {
            reset_msg(&msg);
            msg.source = TEST_C;
            msg.type = REQUEST;
            send_recv(BOTH, TestD, &msg);
        } while (msg.type == REJECT);
        assert(msg.type == ACCEPT);
        printf("C is eating ");
        milli_delay(1000);
        reset_msg(&msg);
        msg.source = TEST_C;
        msg.type = FINISHED;
        send_recv(SEND, TestD, &msg);
    }
}

/*======================================================================*
                               TestB
 *======================================================================*/
void TestD()
{
	MESSAGE msg;
	MESSAGE ret_msg;
	int chopsticks[3] = {0};
	while(1){
	reset_msg(&msg);
	reset_msg(&ret_msg);
	ret_msg.source = TestD;
	send_recv(RECEIVE, ANY, &msg);
	int i = msg.source - 2;
	if(msg.type == REQUEST){
	if(chopsticks[i] == 0 && chopsticks[(i + 1)% 3] == 0){
	ret_msg.type = ACCEPT;
	chopsticks[i] = 1;
	chopsticks[(i + 1) % 3] = 1;
	}
	else{
	ret_msg.type = REJECT;
	}
	send_recv(SEND, msg.source, &ret_msg);
	}
	else if(msg.type == FINISHED){
	chopsticks[i] = 0;
	chopsticks[(i + 1) % 3] = 0;
	}
	}
}

/*****************************************************************************
 *                                panic
 *****************************************************************************/
PUBLIC void panic(const char *fmt, ...)
{
	int i;
	char buf[256];

	/* 4 is the size of fmt in the stack */
	va_list arg = (va_list)((char*)&fmt + 4);

	i = vsprintf(buf, fmt, arg);

	printl("%c !!panic!! %s", MAG_CH_PANIC, buf);

	/* should never arrive here */
	__asm__ __volatile__("ud2");
}

