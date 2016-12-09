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
  if(!list_empty(&keyboardqueue))
  {
      list_add_tail(&(current()->list), &keyboardqueue);
      sched_next_rr();
  }

  int size;
  int writted = 0;
  while(count > 0)
  {
    if(circular.fin < circular.ini)
      size = circular.fin + CIRCULAR_SIZE - circular.ini;
    else size = circular.fin - circular.ini;

    if(size > count) size = count;
    if(size == 0)
    {
      list_add_tail(&(current()->list), &keyboardqueue);
      sched_next_rr();
    }
    else
    {
      copy_to_user(&(circular.buffer[circular.ini]), buffer + writted, size);
      count -= size;
      circular.ini += size;
      circular.ini %= CIRCULAR_SIZE;
      writted = writted + size;
    }
  }
  return writted;
}
