#include <sys/syscall.h>
#include <stdlib.h>

int
main ()
{
	int		fd;

	if ((fd = creat ("hole", 0666)) < 0)
		error ("$*Não consegui criar HOLE");

	if (write (fd, "Primeira parte", 15) < 0) 
		error ("$*Erro de escrita");

	lseek (fd, 8 * 1024, 0);

	if (write (fd, "Segunda parte", 14) < 0) 
		error ("$*Erro de escrita");

	lseek (fd, 80 * 1024, 0);

	if (write (fd, "Terceira parte", 15) < 0) 
		error ("$*Erro de escrita");

	llseek (fd, (loff_t)8 * 1024 * 1024, NULL, 0);

	if (write (fd, "quarta parte", 15) < 0) 
		error ("$*Erro de escrita");

	close (fd);

	return (0);
}
