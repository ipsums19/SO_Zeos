#include <io.h>
#include <utils.h>
#include <list.h>

// Queue for blocked processes in I/O 
struct list_head blocked;

int sys_write_console(char *buffer,int size)
{
  int i;
  
  for (i=0; i<size; i++)
    printc(buffer[i]);
  
  return size;
}


int sys_write(int fd, char * buffer, int size)
{
  int check = check_fd(fd,1);
  if(check < 0) return check;
  else if(buffer == NULL) return -1;
  else if(size < 0) return -1;
  else if(buffer.size() != size) return -1;

  int ret = sys_write_console(buffer, size);

  return ret;
}
