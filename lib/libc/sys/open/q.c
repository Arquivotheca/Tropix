#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

main ()
{
	register 	fd1, fd2;
	register char	*cp;

	cp = "a";
	if ((fd1 = open (cp, O_WRONLY|O_CREAT, 0555)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd1, F_GETFL, 0) );

	if ((fd2 = open (cp, O_WRONLY, 0)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd2, F_GETFL, 0) );

	write (fd1, "BBBBBBBBBBBBBBBBBBBB\n", 21);
	write (fd2, "CCCCCCCCCCCCCCCCCCCC\n", 21);

	close (fd1);
	close (fd2);

	cp = "b";
	if ((fd1 = open (cp, O_WRONLY|O_CREAT|O_APPEND, 0555)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd1, F_GETFL, 0) );

	if ((fd2 = open (cp, O_WRONLY|O_APPEND, 0)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd2, F_GETFL, 0) );

	write (fd1, "BBBBBBBBBBBBBBBBBBBB\n", 21);
	write (fd2, "CCCCCCCCCCCCCCCCCCCC\n", 21);

	close (fd1);
	close (fd2);
}
