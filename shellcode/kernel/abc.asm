// method2
void ret_error(char *name, int canary) {
disp_str("\n\n\n\n\n\n\n\n\n\n\n");
disp_str("Bufferoverflow! in proc ");
isp_str(name);
disp_str(" the value of canary is ");
disp_int(canary);
}
PUBLIC void sys_check_stack() {
 struct proc * p_proc = p_proc_ready;
 if (p_proc - &FIRST_PROC >= NR_TASKS + NR_NATIVE_PROCS) { // 普通用户进程
 int ebp = p_proc->regs.ebp;
 int canary_address_offset = ebp-16;
 int ss = p_proc->regs.ss; // 由ss获得描述符
 int base = reassembly(p_proc->ldts[ss »3].base_high, 24,
 p_proc->ldts[ss »3].base_mid, 16,
 p_proc->ldts[ss »3].base_low);
 unsigned int canary = *(int*)(canary_address_offset + base);
 if (canary != 0x88) { // canary的值被改变
 ret_error(p_proc->name, canary);
 }

unsigned int canary = 0x88

if (ticks%2 == 0) // 每过2个时钟周期，检查内存布局
{
  //printf(p_proc_ready->name);
check_stack(); // 查看当前进程栈
}
