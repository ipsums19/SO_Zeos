/*
 * libc.c
 */

#include <libc.h>

#include <types.h>

int errno;

void perror()
{
    switch(errno)
    {
        case 0: write(1, "No error",9);break;
        case EBADF: write(1,"File descriptor wrong",21);break;
        case EACCES:write(1,"Permission denied",strlen("Permission denied"));break;
        case ENOSYS:write(1,"Function not implemented",strlen("Function not implemented"));break;
        case EPNULL:write(1,"Null pointer",strlen("Null pointer"));break;
        case ESIZENEG:write(1,"Negative size",strlen("Negative size"));break;
        case ENOMEM:write(1,"Failed to allocate memory",strlen("Failed to allocate memory"));break;
        case EAGAIN:write(1,"Cannot allocate sufficient memory to copy parent's page table",
                            strlen("Cannot allocate sufficient memory to copy parent's page table"));break;
    }
}

int write( int fd, char * buffer, int size)
{
    int retorno;
    __asm__("int $  0x80"
            :"=a" (retorno)
            :"a" (4),
            "b" (fd),
            "c" (buffer),
            "d" (size)
    );
    if(retorno < 0)
    {
        errno = -retorno;
        return -1;
    }
    return retorno;
}

int gettime()
{
    int ret;
    __asm__("int $ 0x80"
            :"=a" (ret)
            :"a" (10)
    );
    return ret;
}

int fork()
{
    int ret;
    __asm__("int $ 0x80"
            :"=a" (ret)
            :"a" (2)
    );
    if(retorno < 0)
    {
        errno = -retorno;
        return -1;
    }
    return ret;

}

int getpid()
{
    int ret;
    __asm__("int $ 0x80"
            :"=a" (ret)
            :"a" (20)
    );
    return ret;
}
void itoa(int a, char *b)
{
  int i, i1;
  char c;

  if (a==0) { b[0]='0'; b[1]=0; return ;}

  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }

  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  i=0;
  while (a[i]!=0) i++;

  return i;
}

