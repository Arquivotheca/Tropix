
#include <sys/common.h>
#include <sys/sync.h>
#include <sys/itnet.h>
#include <sys/times.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <xti.h>
#include <errno.h>

const char              udp_dev[] = "/dev/itnudp"; 

int
main ()
{
	int	udp_fd;
	DNS	dns[2];

	memsetl (dns,    0, sizeof (dns)   / sizeof (long));

	if ((udp_fd = t_open (udp_dev, O_RDWR, (T_INFO *)NULL)) < 0)
		error ("$*Não consegui abrir \"%s\"", udp_dev);

	strcpy (dns[0].d_host_nm, "marte2.nce.ufrj.br");
        dns[0].d_host_addr = 0xC0A80002;

        dns[0].d_entry_code = 'C';
        dns[0].d_error_code = DNS_NO_ERROR;
        dns[0].d_server_index = 1;

        dns[0].d_expir_time = 12000000;
   /*** dns[0].d_preference = ...; ***/

        if (ioctl (udp_fd, I_DNS_PUT_INFO, dns) < 0)
                 error ("*Erro no \"ioctl\" I_DNS_PUT_INFO");

	return (0);
}
