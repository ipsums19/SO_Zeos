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

int globalPID = 1;

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

int ret_from_fork()
{
    return 0;
}

int sys_fork()
{
    //get free pcb
    struct list_head *free_list = list_first(&freequeue);
    if(free_list == NULL) return -ENOMEM;
    struct task_struct *pcb;
    pcb = list_head_to_task_struct(free_list);

    //copy task parent to child
    copy_data(current(), pcb, PAGE_SIZE);
    allocate_DIR(pcb);

    page_table_entry *new_PT = get_PT(pcb);
    page_table_entry *current_PT = get_PT(current());

    int page, frame;
    //copy data from user
    for(page = 0; page < NUM_PAG_KERNEL; ++page)
        set_ss_pag(new_PT, page, page);
    for(page = 0; page < NUM_PAG_CODE; ++page)
        set_ss_pag(new_PT, page + PAG_LOG_INIT_CODE, get_frame(current_PT, page + FRAME_INIT_CODE));

    for(page = 0; page < NUM_PAG_DATA; ++page)
    {
        frame = alloc_frame();
        if(frame == -1)
        {
            for(--page ;page >= 0; --page)
                free_frame(get_frame(new_PT, page + PAG_LOG_INIT_DATA));
            return -EAGAIN;
        }
        set_ss_pag(new_PT, page + PAG_LOG_INIT_DATA, frame);
        set_ss_pag(current_PT, PAG_LOG_INIT_DATA + NUM_PAG_DATA, frame);
        copy_data((void *)PAG_LOG_INIT_DATA + NUM_PAG_DATA * PAGE_SIZE,
                  (void *)PAG_LOG_INIT_DATA + page * PAGE_SIZE, PAGE_SIZE);
    }
    del_ss_pag(current_PT, PAG_LOG_INIT_DATA + NUM_PAG_DATA);

    set_cr3(get_DIR(current()));

    pcb->PID = ++globalPID;

    list_del(free_list);
    list_add_tail(free_list, &readyqueue);
    return globalPID;
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
