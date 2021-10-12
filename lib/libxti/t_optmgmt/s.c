#include <stdio.h>
#include <stdlib.h>
#include <xti.h>
#include <fcntl.h>

entry const char	*pgname;	/* Nome do programa */

main (int argc, register const char *argv[])
{
	register int	fd;
	T_INFO		info;
	T_OPTMGMT	req_optmgmt, ret_optmgmt;
	TCP_OPTIONS	req_options, ret_options;
	const char	*str;

	pgname = argv[0];

	/*
	 *	Modo de usar: t <flags> <max_silence>
	 *
	 *	    flags:
	 *	 	T_NEGOTIATE	=  4;
	 *	 	T_CHECK		=  8;
	 *	 	T_DEFAULT 	= 16;
	 *
	 *	 	T_SUCCESS	= 32;
	 *	 	T_FAILURE 	= 64;
	 */
	argv += 1;
	argc -= 1;

	/*
	 *	Abre o "endpoint"
	 */
	if ((fd = t_open ("/dev/itntcp", O_RDWR, &info)) < 0)
	{
		fprintf (stderr, "Não consegui abrir \"%s\"\n", "/dev/itntcp");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Associa um endereço
	 */
	if (t_bind (fd, (T_BIND *)NULL, (T_BIND *)NULL) < 0)
	{
		fprintf (stderr, "Não consegui associar um endereço\n");
		perror (pgname);
		exit (1);
	}

	/*
	 *	Retira o valor de "flags"
	 */
	if (argv[0] != NOSTR && ((req_optmgmt.flags = strtol (argv[0], &str, 0)) < 0 || *str != '\0'))
	{
		fprintf (stderr, "Valor inválido: \"%s\"\n", argv[0]);
		exit (1);
	}

	printf ("flags dado:             %d\n", req_optmgmt.flags);

	/*
	 *	Retira o valor de "max_silence"
	 */
	if (argv[1] != NOSTR && ((req_options.max_silence = strtol (argv[1], &str, 0)), *str != '\0'))
	{
		fprintf (stderr, "Valor inválido: \"%s\"\n", argv[1]);
		exit (1);
	}

	printf ("max_silence dado:      %d\n", req_options.max_silence);

	/*
	 *	Negocia o valor (max_silence)
	 */
	req_optmgmt.opt.buf    = &req_options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	if (req_optmgmt.flags == T_DEFAULT)
		req_optmgmt.opt.len    = 0;

	ret_optmgmt.opt.buf    = &ret_options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

   /***	req_optmgmt.flags	   = ...; ***/

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf (stderr, "Não consegui negociar as opções\n");
		perror (pgname);
		exit (1);
	}

	printf ("flags recebido:         %d\n", ret_optmgmt.flags);
	printf ("max_silence recebido:  %d\n", ret_options.max_silence);

	/*
	 *	Lê o valor em vigência (max_silence)
	 */
	req_optmgmt.opt.buf    = &req_options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	req_optmgmt.flags = T_CHECK;

	ret_optmgmt.opt.buf    = &ret_options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	ret_options.max_silence = 0;

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf (stderr, "Não consegui obter o em uso\n");
		perror (pgname);
		exit (1);
	}

	printf ("max_silence em uso:    %d\n", ret_options.max_silence);

	exit (0);

}	/* end s */
