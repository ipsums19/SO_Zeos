/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <errno.h>
#include <system.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -EACCES; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
    int PID=-1;

    list_head l = list_first(&freequeue);
    if(l == NULL) return -1;
    struct task_struct *pcb;
    pcb = list_head_to_task_struct(l);

    copy_data(current(), pcb, KERNEL_STACK_SIZE*4);
    allocate_DIR(pcb);
    int frame = alloc_frame();
    if(frame == -1) return -2;


    list_add_tail(&readyqueue, l);
    return PID;
}

void sys_exit()
{
}

int sys_write(int fd, char * buffer, int size)
{
    int err = check_fd(fd, ESCRIPTURA);
    if(err < 0) return err;
    if(buffer == NULL) return -EPNULL;
    if(size <= 0) return -ESIZENEG;

    int escr = 0;
    char buffSys[4];
    int despl = 0;

    while(size > 4)
    {
        copy_from_user(buffer + despl, buffSys, 4);
        escr += sys_write_console(buffSys, 4);
        size -= 4;
        despl += 4;
    }
    copy_from_user(buffer + despl, buffSys, size);
    escr += sys_write_console(buffSys, size);

    return escr;

}

int sys_gettime()
{
    return zeos_ticks;
}
