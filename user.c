#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  runjp_rank(0,2);
  runjp_rank(4,7);
  /*runjp_rank(3,3);*/
  /*runjp();*/
  while(1) { }
}
