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

  // creates the child process

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
