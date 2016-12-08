#include <io.h>
#include <utils.h>
#include <list.h>
#include <sched.h>

// Queue for blocked processes in I/O
struct list_head blocked;

int sys_write_console(char *buffer,int size)
{
  int i;

  for (i=0; i<size; i++)
    printc(buffer[i]);

  return size;
}

int sys_read_keyboard(char * buffer, int count)
{
  if(!list_empty(&keyboardqueue)){
      list_add_tail(&(current()->list), &keyboardqueue);
      sched_next_rr();
  }
  return 0;
}
