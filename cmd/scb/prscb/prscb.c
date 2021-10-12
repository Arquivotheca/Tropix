/*
 ****************************************************************
 *								*
 *			prscb.c					*
 *								*
 *	Imprime o bloco de controle do núcleo do TROPIX		*
 *								*
 *	Versão	1.0.0, de 18.06.86				*
 *		4.2.0, de 04.12.01				*
 *								*
 *	Módulo: scb						*
 *		Utilitário especial				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/scb.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	4.2.0, de 04.12.01";

#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */

/*
 ****** Protótipos de funções ***********************************
 */
void		scbprdev (dev_t);
void		help (void);

/*
 ****************************************************************
 *	Imprime o bloco de controle do núcleo do TROPIX		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int	opt;
	SCB		scb;
	register SCB	*sp = &scb;

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			putc ('\n', stderr);
			help ();

		}	/* end switch */

	}	/* end while */

	argv += optind;
	argc -= optind;

	/*
	 *	Obtem o SCB
	 */
	if ((int)getscb (sp) == -1)
	{
		fprintf
		(	stderr,
			"Não consegui obter o SCB (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	/*
	 *	CPUs
	 */
	printf ("ncpu     = %5d        ", sp->y_ncpu);
	printf ("initpid  = %5d        ", sp->y_initpid);
	printf ("cputype  = ");

	switch (sp->y_cputype)
	{
	    case 386:
		printf ("INTEL 386\n");
		break;

	    case 486:
		printf ("INTEL 486\n");
		break;

	    case 586:
		printf ("INTEL PENTIUM\n");
		break;

	    default:
		printf ("????\n");
		break;

	}	/* end switch */

	printf ("DELAY    = %5d        ", sp->y_DELAY_value);
	printf ("pgsz     = %5d        ", sp->y_pgsz);
	printf ("\n");

	printf ("nblkdev  = %5d        ", sp->y_nblkdev);
	printf ("nchrdev  = %5d\n\n",	  sp->y_nchrdev);

	/*
	 *	Tela A do BOOT
	 */
	printf ("nproc    = %5d        ", sp->y_nproc);
	printf ("nphash   = %5d, %-5d ",  sp->y_nphash, sp->y_phmask);
	printf ("ninode   = %5d\n",	  sp->y_ninode);

	printf ("nihash   = %5d        ", sp->y_nihash);
	printf ("nbuf     = %5d        ", sp->y_nbuf);
	printf ("nbhash   = %5d\n",	  sp->y_nbhash);

	printf ("nkfile   = %5d        ", sp->y_nkfile);
	printf ("nclist   = %5d        ", sp->y_nclist);
	printf ("nmap     = %5d\n", 	  sp->y_nmap);

	printf ("ntout    = %5d        ", sp->y_ntout);
	printf ("nlockf   = %5d\n\n",	  sp->y_nlockf);

	/*
	 *	Tela B do BOOT
	 */
	printf ("neventl  = %5d        ", sp->y_nueventl);
	printf ("neventg  = %5d        ", sp->y_nueventg);
	printf ("nsemal   = %5d\n",	  sp->y_nusemal);

	printf ("nsemag   = %5d        ", sp->y_nusemag);
	printf ("nregionl = %5d        ", sp->y_nregionl);
	printf ("nregiong = %5d\n",	  sp->y_nregiong);

	printf ("ndns     = %5d        ", sp->y_n_dns);
	printf ("nroute   = %5d        ", sp->y_n_route);
	printf ("nether   = %5d\n",	  sp->y_n_ether);

	printf ("nrawep   = %5d        ", sp->y_n_raw_ep);
	printf ("nudpep   = %5d        ", sp->y_n_udp_ep);
	printf ("ntcpep   = %5d\n\n",	  sp->y_n_tcp_ep);

	/*
	 *	Dispositivos e memória
	 */
	printf ("rootdev  = "); scbprdev (sp->y_rootdev); printf ("\t");
	printf ("pipedev  = "); scbprdev (sp->y_pipedev); printf ("\t");
	printf ("swapdev  = "); scbprdev (sp->y_swapdev); printf ("\n");

#define	VIRTUALTOKB(i) 	(i == NOVOID ? 0 : (unsigned)((i) - SYS_ADDR) >> KBSHIFT)
#define	VPGTOKB(i) 	(i == 0 ? 0 : (((unsigned)(i) << PGSHIFT) - (int)SYS_ADDR) >> KBSHIFT)

	printf ("swaplow  = %5d        ", sp->y_swaplow);
	printf ("nswap    = %5d        ", sp->y_nswap);
	printf ("endkernel= %5d KB\n",    VPGTOKB (sp->y_ucore1) );

	printf ("ucore    = %5d KB     ", VPGTOKB (sp->y_ucore1) );
	printf ("enducore = %5d KB     ", VPGTOKB (sp->y_enducore1) );
	printf ("endpmem  = %5d KB\n\n",  VIRTUALTOKB (sp->y_endpmem) ); 

	printf ("ramd1sz  = %5d KB     ", BYTOKB (sp->y_ramd1sz));
	printf ("ramd1    = %5d KB     ", VIRTUALTOKB (sp->y_ramd1) );
	printf ("endramd1 = %5d KB\n",    VIRTUALTOKB (sp->y_endramd1) ); 

	printf ("ramd0sz  = %5d KB     ", BYTOKB (sp->y_ramd0sz));
	printf ("ramd0    = %5d KB     ", VIRTUALTOKB (sp->y_ramd0) );
	printf ("endramd0 = %5d KB\n\n",  VIRTUALTOKB (sp->y_endramd0) ); 

	/*
	 *	Scheduler
	 */
	printf ("tslice   = %5d Ticks  ", sp->y_tslice);
	printf ("incpri   = %5d        ", sp->y_incpri);
	printf ("decpri   = %5d\n",	  sp->y_decpri);

	printf ("ttyinc   = %5d        ", sp->y_ttyinc);
	printf ("nfactor  = %5d\n",	  sp->y_nfactor);

	/*
	 *	Processo do usário
	 */
	printf ("maxppu   = %5d        ", sp->y_maxppu);
	printf ("maxpsz   = %5d KB     ", PGTOKB (sp->y_maxpsz));
	printf ("umask    =   %03o\n",	  sp->y_umask);

	printf ("ncexec   = %5d        ", sp->y_ncexec);
	printf ("stksz    = %5d        ", sp->y_stksz);
	printf ("stkincr  = %5d\n\n",	  sp->y_stkincr);

	/*
	 *	Diversos
	 */
	printf ("hz       = %5d        ", sp->y_hz);
	printf ("nmount   = %5d\n",	  sp->y_nmount);

	printf ("nitblock = %5d        ", sp->y_n_itblock);
	printf ("scrsaver = %5d\n\n",	  sp->y_screen_saver_time);

	printf ("preempt  =   0x%02X       ", sp->y_preemption_mask[0]);
	printf ("debug    = %5d\n",	  sp->y_CSW);

	printf ("tzmin    = %5d        ", sp->y_tzmin);
	printf ("utclock  = %5d        ", sp->y_ut_clock);
	printf ("dmaenable= %5d\n",	  sp->y_dma_enable);

	printf ("key      = %5d\n",	  sp->y_keyboard);

	/*
	 *	Portas e interrupções das linhas serias
	 */
	printf ("\nsioport0 =   0x%X\t",  sp->y_sio_port[0]);
	printf ("sioirq0  = %d\t", 	  sp->y_sio_irq[0]);
	printf ("sioport1 =   0x%X\t", 	  sp->y_sio_port[1]);
	printf ("sioirq1  = %d\n", 	  sp->y_sio_irq[1]);
	printf ("sioport2 =   0x%X\t", 	  sp->y_sio_port[2]);
	printf ("sioirq2  = %d\t", 	  sp->y_sio_irq[2]);
	printf ("sioport3 =   0x%X\t", 	  sp->y_sio_port[3]);
	printf ("sioirq3  = %d\n", 	  sp->y_sio_irq[3]);
	printf ("sioport4 =   0x%X\t", 	  sp->y_sio_port[4]);
	printf ("sioirq4  = %d\t", 	  sp->y_sio_irq[4]);
	printf ("sioport5 =   0x%X\t", 	  sp->y_sio_port[5]);
	printf ("sioirq5  = %d\n", 	  sp->y_sio_irq[5]);
	printf ("sioport6 =   0x%X\t", 	  sp->y_sio_port[6]);
	printf ("sioirq6  = %d\t", 	  sp->y_sio_irq[6]);
	printf ("sioport7 =   0x%X\t", 	  sp->y_sio_port[7]);
	printf ("sioirq7  = %d\n", 	  sp->y_sio_irq[7]);

	printf ("\nlpport0  =   0x%X\t",  sp->y_lp_port[0]);
	printf ("lpirq0   = %d\t", 	  sp->y_lp_irq[0]);
	printf ("lpport1  =   0x%X\t", 	  sp->y_lp_port[1]);
	printf ("lpirq1   = %d\n", 	  sp->y_lp_irq[1]);
	printf ("lpport2  =   0x%X\t", 	  sp->y_lp_port[2]);
	printf ("lpirq2   = %d\t", 	  sp->y_lp_irq[2]);
	printf ("lpport3  =   0x%X\t", 	  sp->y_lp_port[3]);
	printf ("lpirq3   = %d\n", 	  sp->y_lp_irq[3]);

	printf ("\nedport0  =   0x%X\t",  sp->y_ed_port[0]);
	printf ("edirq0   = %d\t", 	  sp->y_ed_irq[0]);
	printf ("edport1  =   0x%X\t", 	  sp->y_ed_port[1]);
	printf ("edirq1   = %d\n", 	  sp->y_ed_irq[1]);
	printf ("edport2  =   0x%X\t", 	  sp->y_ed_port[2]);
	printf ("edirq2   = %d\t", 	  sp->y_ed_irq[2]);
	printf ("edport3  =   0x%X\t", 	  sp->y_ed_port[3]);
	printf ("edirq3   = %d\n", 	  sp->y_ed_irq[3]);

	return (exit_code);

}	/* end prscb */

/*
 ****************************************************************
 *	Imprime o Nome do Dispositivo				*
 ****************************************************************
 */
void
scbprdev (dev_t dev)
{
	id_t		id;

	if (dev == NODEV)
	{
		printf ("NODEV       ");
	}
	else
	{
		printf
		(	"%s (%d/%d)",
			kcntl (K_GETDEVNM, dev, id),
			MAJOR (dev), MINOR (dev)
		);
	}

}	/* end scbprdev */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - imprime o bloco de controle do núcleo do TROPIX\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
