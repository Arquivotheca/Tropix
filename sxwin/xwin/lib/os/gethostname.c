/*
 ****************************************************************
 *								*
 *			gethostname.c				*
 *								*
 *	Obtém o nome da máquina					*
 *								*
 *	Versão	1.0.0, de 18.03.97				*
 *		1.0.2, de 29.04.98				*
 *								*
 *	Módulo: xc/lib/aux					*
 *		X Windows - Bibliotecas				*
 *		Baseado no software homônimo do XFree86		*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright (c) 1998 TROPIX Technologies Ltd.	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/syscall.h>
#include <sys/itnet.h>

#include <string.h>
#include <fcntl.h>
#include <xti.h>

static const char	tcpdevname[] =	"/dev/itntcp";
static unsigned long	myipaddr;
static int		mynamelen;
static char		myname[NETNM_SZ];

/*
 ****************************************************************
 *	Obtém o nome da máquina					*
 ****************************************************************
 */
int
gethostname (char *buf, int maxlen)
{
	int	fd;

	if (myipaddr == 0)
	{	/*
		 *	Obtém o endereço IP da máquina.
		 *	Guarda para que, futuramente, não precise obter de novo.
		 */
		if ((fd = t_open (tcpdevname, O_RDWR, (T_INFO *)NULL)) < 0)
			return (-1);

		myipaddr = t_node_to_addr (fd, NULL, NULL);
		strcpy (myname, t_addr_to_node (fd, myipaddr));
		mynamelen = strlen (myname) + 1;

		t_close (fd);
	}

	/*
	 *	Copia para o buffer dado.
	 */
	if (maxlen >= mynamelen)
	{
		memmove (buf, myname, mynamelen);
		return (mynamelen - 1);
	}

	maxlen--;
	memmove (buf, myname, maxlen);
	buf[maxlen] = '\0';

	return (maxlen);

}	/* end gethostname */
