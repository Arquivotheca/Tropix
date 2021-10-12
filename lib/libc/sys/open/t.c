#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

main ()
{
	register 	fd;
	register char	*cp;

	cp = "a";
	if ((fd = open (cp, O_RDONLY|O_APPEND, 0)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd, F_GETFL, 0) );

	cp = "b";
	if ((fd = open (cp, O_WRONLY|O_NDELAY, 0)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd, F_GETFL, 0) );

	cp = "c";
	if ((fd = open (cp, O_WRONLY|O_CREAT|O_LOCK, 0742)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd, F_GETFL, 0) );

	cp = "d";
	if ((fd = open (cp, O_WRONLY|O_TRUNC, 0)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd, F_GETFL, 0) );

	cp = "d";
	if ((fd = open (cp, O_WRONLY|O_CREAT|O_EXCL, 0)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd, F_GETFL, 0) );

	cp = "c";
	if ((fd = open (cp, O_RDONLY, 0)) < 0)
		printf ("Erro no open de \"%s\": %s\n", cp, strerror (errno) );
	printf ("\"%s\": %x\n", cp, fcntl (fd, F_GETFL, 0) );

}
