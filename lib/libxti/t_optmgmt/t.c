#include <stdio.h>
#include <stdlib.h>
#include <xti.h>
#include <fcntl.h>
#include <errno.h>

char		*edit_ret_flag (int);

entry const char	*pgname;	/* Nome do programa */

main (int argc, register const char *argv[])
{
	register int	fd;
	T_INFO		info;
	T_OPTMGMT	req_optmgmt, ret_optmgmt;
	TCP_OPTIONS	options;

	pgname = argv[0];

	/*
	 *	Modo de usar: t
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
	 ******	Obtém os valores PADRÃO *************************
	 */
	req_optmgmt.flags = T_DEFAULT;

   /***	req_optmgmt.opt.buf    = (TCP_OPTIONS *)NULL; ***/
   /***	req_optmgmt.opt.len    = 0; ***/

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (DEFAULT) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* DEFAULT ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	/*
	 ****** Negocia o "max_seg_size" ************************
	 */
	req_optmgmt.flags = T_NEGOTIATE;

	options.max_seg_size = 500;

	req_optmgmt.opt.buf    = &options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (NEGOCIATE) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* NEGOCIEI \"max_seg_size\" ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	/*
	 ****** Negocia o "max_wait" ************************
	 */
	req_optmgmt.flags = T_NEGOTIATE;

	options.max_wait = 7;

	req_optmgmt.opt.buf    = &options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (NEGOCIATE) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* NEGOCIEI \"max_wait\" ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	/*
	 ****** Negocia o "max_silence" ************************
	 */
	req_optmgmt.flags = T_NEGOTIATE;

	options.max_silence = 999;

	req_optmgmt.opt.buf    = &options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (NEGOCIATE) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* NEGOCIEI \"max_silence\" ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	/*
	 ****** Negocia o "window_size" ************************
	 */
	req_optmgmt.flags = T_NEGOTIATE;

	options.window_size = 20000;

	req_optmgmt.opt.buf    = &options;
	req_optmgmt.opt.len    = sizeof (TCP_OPTIONS);

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (NEGOCIATE) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* NEGOCIEI \"window_size\" ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	/*
	 ******	Obtém os valores PADRÃO *************************
	 */
	req_optmgmt.flags = T_DEFAULT;

   /***	req_optmgmt.opt.buf    = (TCP_OPTIONS *)NULL; ***/
   /***	req_optmgmt.opt.len    = 0; ***/

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (DEFAULT) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* DEFAULT ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	/*
	 ******	Obtém os valores em USO *************************
	 */
	req_optmgmt.flags = T_CHECK;

   /***	req_optmgmt.opt.buf    = (TCP_OPTIONS *)NULL; ***/
   /***	req_optmgmt.opt.len    = 0; ***/

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (DEFAULT) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* CHECK ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	/*
	 ****** Testa um valor inválido *************************
	 */
	req_optmgmt.flags = T_CHECK;

	options.max_seg_size = 3000;

   /***	req_optmgmt.opt.buf    = (TCP_OPTIONS *)NULL; ***/
   /***	req_optmgmt.opt.len    = 0; ***/

	ret_optmgmt.opt.buf    = &options;
	ret_optmgmt.opt.maxlen = sizeof (TCP_OPTIONS);

	if (t_optmgmt (fd, &req_optmgmt, &ret_optmgmt) < 0)
	{
		fprintf
		(	stderr,
			"%s: Não consegui negociar (DEFAULT) (%s)\n",
			pgname, strerror (errno)
		);
		exit (1);
	}

	printf ("********* CHECK inválido ***********\n");
	printf ("flags recebido:         %s\n", edit_ret_flag (ret_optmgmt.flags));
	printf ("wait recebido:          %d\n", options.max_wait);
	printf ("silence recebido:       %d\n", options.max_silence);
	printf ("max_seg_size recebido:  %d\n", options.max_seg_size);
	printf ("window_size recebido:   %d\n", options.window_size);
	printf ("\n");

	exit (0);

}	/* end t */

/*
 ****************************************************************
 *	Imprime o indicador de retorno				*
 ****************************************************************
 */
char *
edit_ret_flag (int f)
{
	switch (f)
	{
	    case T_SUCCESS:
		    return ("SUCCESS");

	    case T_FAILURE:
		    return ("FAILURE");

	    case 0:
		    return ("-");

	    default:
		    return ("???");
	}

} 	/* end edit_ret_flag */
