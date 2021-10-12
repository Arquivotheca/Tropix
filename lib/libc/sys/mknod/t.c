#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <errno.h>

main ()
{
	if (mknod ("bobo", S_IFCHR|0321, 0x0203) < 0)
		printf ("Errno no mknod: %s\n", strerror (errno) ); 

	if (mknod ("/sdfsdf/wertwe/bobo", S_IFCHR|0321, 0x0203) < 0)
		printf ("Errno no mknod: %s\n", strerror (errno) ); 
}
