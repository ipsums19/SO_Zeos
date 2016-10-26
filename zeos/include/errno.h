#ifndef __ERRNO__
#define __ERRNO__


#define EBADF 1 	/* Bad file descriptor */
#define EACCES 2 	/* Permission denied */
#define ENOSYS 3 	/* Function not implemented */
#define EPNULL 4 	/* Pointer is null */
#define ESIZENEG 5 	/* Size is not positive */
#define ENOMEM 6    /*failed to allocate the necessary kernel structures because memory is tight*/
#define EAGAIN 7    /*cannot allocate sufficient memory to copy the parent's page tables*/

#endif
