#ifndef __ERRNO__
#define __ERRNO__


#define EBADF 9 	/* Bad file descriptor */
#define EACCES 13 	/* Permission denied */
#define ENOSYS 38 	/* Function not implemented */
#define EPNULL 4 	/* Pointer is null */
#define ESIZENEG 5 	/* Size is not positive */
#define ENOMEM 12   /*failed to allocate the necessary kernel structures because memory is tight*/
#define EAGAIN 11   /*cannot allocate sufficient memory to copy the parent's page tables*/

#endif
