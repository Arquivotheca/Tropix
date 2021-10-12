#include <sys/types.h>
#include <termio.h>

main ()
{
	PTYIO	 	ptyio;
	TERMIO		t;

	if (getpty (&ptyio) < 0)
		error ("$*Erro em getpty");

	printf
	(	"fdc = %d, fds = %d, nmc = \"%s\", nms = \"%s\"\n",
		ptyio.t_fd_client, ptyio.t_fd_server, ptyio.t_nm_client, ptyio.t_nm_server
	);

	if (ioctl (ptyio.t_fd_client, TCGETS, &t) < 0)
		error ("$*Erro em ioctl");

	printf ("nome do cliente  = \"%s\"\n", t.c_name);

	if (ioctl (ptyio.t_fd_server, TCGETS, &t) < 0)
		error ("$*Erro em ioctl");

	printf ("nome do servidor = \"%s\"\n", t.c_name);

	return (0);
}
