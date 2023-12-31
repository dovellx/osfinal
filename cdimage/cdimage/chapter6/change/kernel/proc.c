
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"

/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS* p;
	int	 greatest_ticks = 0;
    
	int i, temp;
	while (!greatest_ticks) {
		temp = highI;
		for(i = 0; i < temp; i++) {
			if(highP[i] != -1 && proc_table[highP[i]].ticks > 0) { //we find the first task to be scheduled
				greatest_ticks = proc_table[highP[i]].ticks;
				p_proc_ready = &proc_table[highP[i]];
				break;
			}else if(highP[i] != -1 && proc_table[highP[i]].ticks <= 0) { //this task should be put into the midP
				proc_table[highP[i]].ticks = MID_TICK;
				midP[midI++] = highP[i];

				int j;

				disp_color_str(proc_table[highP[i]].p_name, 4);
				disp_color_str(":high->mid    ", 4);

				for(j = i; j < temp; j++) {
					highP[j] = highP[j + 1];
				}
				highP[j] = -1;
				highI--;
				i--;
				temp--;
			}
		}
		if(greatest_ticks) {
			break;
		}

		temp = midI;
		for(i = 0; i < temp; i++) {
			if(midP[i] != -1 && proc_table[midP[i]].ticks > 0) { //we find the first task to be scheduled
				greatest_ticks = proc_table[midP[i]].ticks;
				p_proc_ready = &proc_table[midP[i]];
				break;
			}else if(midP[i] != -1 && proc_table[midP[i]].ticks <= 0) { //this task should be put into the lowP
				proc_table[midP[i]].ticks = LOW_TICK;
				lowP[lowI++] = lowP[i];

				disp_color_str(proc_table[midP[i]].p_name, 4);
				disp_color_str(":mid->low    ", 4);

				int j;
				for(j = i; j < temp; j++) {
					midP[j] = midP[j + 1];
				}
				midP[j] = -1;
				midI--;
				i--;
				temp--;
                
			}
		}
		if(greatest_ticks) {
			break;
		}

		temp = lowI;
		for(i = 0; i < temp; i++) {
			if(lowP[i] != -1 && proc_table[lowP[i]].ticks > 0) { //we find the first task to be scheduled
				greatest_ticks = proc_table[lowP[i]].ticks;
				p_proc_ready = &proc_table[lowP[i]];
				break;
			}else if(lowP[i] != -1 && proc_table[lowP[i]].ticks <= 0) { //this task will not have the chance to be run

				disp_color_str(proc_table[midP[i]].p_name, 4);
				disp_color_str(":have no ticks    ", 4);

				int j;
				for(j = i; j < temp; j++) {
					lowP[j] = lowP[j + 1];
				}
				lowP[j] = -1;
				lowI--;
				i--;
				temp--;
			}
		}
		if(greatest_ticks) {
			break;
		}

		if (!greatest_ticks) {
			for (p = proc_table; p < proc_table+NR_TASKS; p++) {
				p->ticks =  HIGH_TICK;
			}
			disp_color_str("    restart    ", 2);
			//ticks = 0;
			highP[0] = 0;
			highP[1] = 1;
			highI = 2;
		}
	}
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

