/*
 ****************************************************************
 *								*
 *			itck.c					*
 *								*
 *	Examina a consistência dos blocos de E/S da INTERNET	*
 *								*
 *	Versão	3.0.0, de 23.02.91				*
 *		3.0.0, de 23.02.91				*
 *								*
 *	Módulo: itck						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/scb.h>
#include <sys/itnet.h>
#include <sys/a.out.h>
#include <sys/times.h>
#include <sys/kcntl.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <id.h>
#include <time.h>
#include <termio.h>
#include <fcntl.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.0, de 23.02.98";

#define	NOVOID	(void *)NULL
#define	NOIP	(IP_H *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	LINESZ	16

#define	ACV(a)	((int)(a) + INCR) /* Conversão de endereço para PHYS */

entry const char *pgname;	/* Nome do programa */

entry int	exit_code = 0;	/* Código de retorno */

#if (0)	/*******************************************************/
entry int	eflag;		/* TCP EPs */
entry int	dflag;		/* Debug */
entry int	pflag;		/* Parametros */
entry int	tflag;		/* Tabela de dispositivos */
entry int	fflag;		/* Tamanho do FIFO */
#endif	/*******************************************************/

entry int	vflag;		/* Verbose */
entry int	oflag;		/* old */

entry int	global_itn_fd;	/* Para "error" */

/*
 ******	Verifica se as macro variáveis estão definidas **********
 */
#if	!defined (BIG_ENDIAN) && !defined (LITTLE_ENDIAN)
#error	"Uma das macrovariáveis \"BIG/LITTLE_ENDIAN\" deve estar definida"
#endif	BIG_ENDIAN/LITTLE_ENDIAN

/*
 ******	Definições relativas à ordem de bytes da CPU ************
 */
#ifdef	LITTLE_ENDIAN
#define	ENDIAN_LONG(x)	long_endian_cv (x)
#define	ENDIAN_SHORT(x)	short_endian_cv (x)
#else
#define	ENDIAN_LONG(x)	(x)
#define	ENDIAN_SHORT(x)	(x)
#endif	LITTLE_ENDIAN

/*
 ****** Tabela dos nomes dos protocolos *************************
 */
typedef struct
{
	int	t_proto;	/* O no. do protocolo */
	char	t_proto_nm[8];	/* O nome. do protocolo */

}	PROTO_TB;

const PROTO_TB	proto_tb[] =
{
	ICMP_PROTO,	"ICMP",
	TCP_PROTO,	"TCP",
	UDP_PROTO,	"UDP",
	0
};

/*
 ****** Tabela dos nomes dos controles TCP **********************
 */
typedef struct
{
	int	t_ctl;		/* O bit de controle */
	char	t_ctl_nm[8];	/* O nome do bit de controle */

}	CTL_TB;

const CTL_TB	ctl_tb[] =
{
	C_URG, 	"URG",
	C_ACK, 	"ACK",
	C_PSH, 	"PSH",
	C_RST, 	"RST",
	C_SYN, 	"SYN",
	C_FIN, 	"FIN",
	C_DATA,	"DATA",
	0
};

/*
 ****** Protótipos de funções ***********************************
 */
void		pr_datagram (const IP_H *, int);
void		try_to_print_ctls (const IP_H *);
void		print_tcp_ep_queues (const TCP_EP *, const SCB *, long);
void		print_ip_header (const IP_H *, int);
void		print_tcp_header (const void *, int, int);
const char	*get_proto_name (int);
char		*edit_itblenum (ITBLENUM);
void		print_data_dump (const void *, int);
void		error (const char *, ...);
void		help (void);

#if (0)	/*******************************************************/
void		help (void);
void		pr_datagram (IP_H *, int);
void		print_queue_datagrams (QUEUE_TB *, int);
void		print_ip_header (IP_H *, int);
void		print_tcp_header (void *, int, int);

#endif	/*******************************************************/

/*
 ****************************************************************
 *	Examina a consistência dos blocos de E/S da INTERNET	*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt;
	register int		*ack_log, *ack_log_original, *ack_log_ptr, *ack_ptr;
	register int		*rcv_ack_log, *rcv_ack_log_original, *rcv_ack_log_ptr, *rcv_ack_ptr;
	register const ITSCB	*ip;
	register const TCP_EP	*tep;
	register const ITBLOCK	*bp = NOITBLOCK, *end_bp;
	const ITBLOCK		*bp_exemplar = NOITBLOCK, *bp_perdido = NOITBLOCK;
	register const IP_H	*ihp = NOIP, *ihp_perdido = NOIP, *ihp_exemplar = NOIP;
	int			index = 0, index_perdido = 0, index_exemplar = 0, given_index = -1;
	const char		*ptr, *str;
	int			in_free_list, i;
	int			n_tep_free = 0, n_tep_busy = 0;			
	long			INCR;
	time_t			now;
#if (0)	/*******************************************************/
	int			get_it_block_other, get_it_block_max_other, get_it_block_DATA_sema;
	int			OUT_DATA, IN_OUT_CTL;
	int			errors = 0;
#endif	/*******************************************************/
	lid_t			lid;
	SYM			sym;
	SCB			scb;


	pgname = argv[0];

	/*
	 *	Analisa as opções
	 */
	while ((opt = getopt (argc, argv, "ovH")) != EOF)
	{
		switch (opt)
		{
		    case 'o':			/* Old */
			oflag++;
			break;

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
	 *	Verifica se deu um índice
	 */
	if (argv[0] != NOSTR)
		given_index = strtol (argv[0], &str, 0);

	/*
	 *	Obtém o SCB
	 */
	if ((int)getscb (&scb) == -1)
		error ("$*Não consegui obter o SCB");

	/*
	 *	Mapeia o KERNEL inteiro
	 */
	if ((int)(ptr = phys (SYS_ADDR, 8 * MBSZ, O_RDONLY|O_KERNEL)) == -1)
		error ("$*Não consegui mapear o KERNEL");

	INCR = ((int)ptr - (int)SYS_ADDR);

	/*
	 *	Mapeia o "ack_log"
	 */
	lidcan (lid, "_ack_log");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	ack_log_original = (int *)sym.s_value;
	printf ("ack_log (original) = %P\n", sym.s_value);
	ack_log = (int *)ACV (sym.s_value);

	/*
	 *	Mapeia o "ack_log_ptr"
	 */
	lidcan (lid, "_ack_log_ptr");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	ack_log_ptr = (int *)ACV (sym.s_value);

	index = (int *)*ack_log_ptr - ack_log_original;

	printf ("index = %d\n", index);

	/*
	 *	Imprime os valores
	 */
	ack_ptr = (int *)ACV (*ack_log_ptr);
	printf ("index = %d\n", ack_ptr - ack_log);

	for (i = 20; i > 0; i--)
	{
		ulong	rcv_wnd, snd_nxt, rcv_nxt;

		if (ack_ptr >= ack_log + 60)
			ack_ptr = ack_log;

		rcv_wnd = *ack_ptr++;
		snd_nxt = *ack_ptr++;
		rcv_nxt = *ack_ptr++;

		printf ("rcv_wnd = %u, ", rcv_wnd - rcv_nxt);
		printf ("snd_nxt = %u, ", snd_nxt);
		printf ("rcv_nxt = %u\n", rcv_nxt);
	}

	/*
	 *	Mapeia o "rcv_ack_log"
	 */
	lidcan (lid, "_rcv_ack_log");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	rcv_ack_log_original = (int *)sym.s_value;
	printf ("rcv_ack_log (original) = %P\n", sym.s_value);
	rcv_ack_log = (int *)ACV (sym.s_value);

	/*
	 *	Mapeia o "rcv_ack_log_ptr"
	 */
	lidcan (lid, "_rcv_ack_log_ptr");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	rcv_ack_log_ptr = (int *)ACV (sym.s_value);

	index = (int *)*rcv_ack_log_ptr - rcv_ack_log_original;

	printf ("index = %d\n", index);

	/*
	 *	Imprime os valores
	 */
	rcv_ack_ptr = (int *)ACV (*rcv_ack_log_ptr);
	printf ("index = %d\n", rcv_ack_ptr - rcv_ack_log);

	for (i = 20; i > 0; i--)
	{
		ulong	seg_wnd, seg_ack;

		if (rcv_ack_ptr >= rcv_ack_log + 40)
			rcv_ack_ptr = rcv_ack_log;

		seg_ack = *rcv_ack_ptr++;
		seg_wnd = *rcv_ack_ptr++;

		printf ("seg_ack = %u, ", seg_ack);
		printf ("seg_wnd = %u\n", seg_wnd - seg_ack);
	}

	exit (0);

	/*
	 *	Mapeia a ITSCB
	 */
	lidcan (lid, "_itscb");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	ip = (ITSCB *)ACV (sym.s_value);
#if (0)	/*******************************************************/
	ip = (ITSCB *)(sym.s_value - (int)SYS_ADDR + ptr);

	printf ("it_bl_count = %d\n",  ip->it_bl_count);
	printf ("it_tep_total = %d\n",  ip->it_tep_total);
	printf ("it_tep_count = %d\n",  ip->it_tep_count);
	printf ("it_block_free = %P\n",  ip->it_block_free);
#endif	/*******************************************************/

	/*
	 *	Conta o No. de endpoints TCP
	 */
	for (tep = ip->it_tep_busy; /* abaixo */; tep = tep->tp_next, n_tep_busy++)
	{
		if (tep == NO_TCP_EP)
		{
			printf ("TCP_EP busy: %d", n_tep_busy);
			break;
		}

		if (tep < scb.y_tcp_ep || tep >= scb.y_end_tcp_ep)
		{
			printf ("Ponteiro TCP EP busy inválido: %P\n", tep);
			break;
		}

		tep = (TCP_EP *)ACV (tep);
	}

	for (tep = ip->it_tep_free; /* abaixo */; tep = tep->tp_next, n_tep_free++)
	{
		if (tep == NO_TCP_EP)
		{
			printf (", TCP_EP free: %d", n_tep_free);
			break;
		}

		if (tep < scb.y_tcp_ep || tep >= scb.y_end_tcp_ep)
		{
			printf ("Ponteiro TCP EP free inválido: %P\n", tep);
			break;
		}

		tep = (TCP_EP *)ACV (tep);
	}

	printf (", TCP_EP free+busy: %d\n", n_tep_free + n_tep_busy);

	/*
	 *	Examina as filas de cada endpoint
	 */
	for (tep = ip->it_tep_busy; /* abaixo */; tep = tep->tp_next)
	{
		if (tep == NO_TCP_EP)
			break;

		if (tep < scb.y_tcp_ep || tep >= scb.y_end_tcp_ep)
		{
			printf ("Ponteiro TCP EP busy inválido: %P\n", tep);
			break;
		}

		tep = (TCP_EP *)ACV (tep);

		print_tcp_ep_queues (tep, &scb, INCR);
	}

	/*
	 *	x
	 */
	time (&now);

	end_bp = (ITBLOCK *)ACV (scb.y_end_itblock);

	for
	(	bp = (ITBLOCK *)ACV (scb.y_itblock), index = 0, in_free_list = 0;
		bp < end_bp;
		bp++, index++
	)
	{
		if   (bp->it_in_free_list)
		{
			in_free_list++;
		}
		elif (given_index < 0 && (!oflag || now - bp->it_time > 120))
		{
			printf ("Em uso (%d): Time = %24.24s", index, ctime (&bp->it_time));
#if (0)	/*******************************************************/
			printf (", route = %P", bp->it_route);
#endif	/*******************************************************/
			printf (", enum = %s", edit_itblenum (bp->it_get_enum));
			printf (", it_u_count = %d ", bp->it_u_count);
			if (bp->it_ip_header)
				try_to_print_ctls ((IP_H *)ACV (bp->it_ip_header));
			printf ("\n");
		}

		if   (given_index == index && bp->it_ip_header)
		{
			bp_perdido    = bp;
			ihp_perdido   = (IP_H *)ACV (bp->it_ip_header);
			index_perdido = index;
		}
		elif (bp_perdido == NOITBLOCK && bp->it_ip_header)
		{
			bp_perdido    = bp;
			ihp_perdido   = (IP_H *)ACV (bp->it_ip_header);
			index_perdido = index;
		}

		if (bp_exemplar == NOITBLOCK && bp->it_ip_header)
		{
			bp_exemplar    = bp;
			ihp_exemplar   = (IP_H *)ACV (bp->it_ip_header);
			index_exemplar = index;
		}
	}

	/*
	 *	Imprime o bloco encontrado
	 */
	if   (bp_perdido)
	{
		bp    = bp_perdido;
		ihp   = ihp_perdido;
		index = index_perdido;
	}
	elif (bp_exemplar)
	{
		bp    = bp_exemplar;
		ihp   = ihp_exemplar;
		index = index_exemplar;
	}
	else
	{
		printf ("Não obtive bloco\n");
		exit (0);
	}

	printf
	(	"Exemplar: index = %d, data = %24.24s, enum = %s, src = %s, dst = %s\n",
		index,
		ctime (&bp->it_time),
		edit_itblenum (bp->it_get_enum), 
		t_addr_to_str (bp->it_src_addr),
		t_addr_to_str (bp->it_dst_addr)
	);

	printf
	(	"ip_h_size = %d, ip_version = %d\n",
		ihp->ip_h_size, ihp->ip_version
	);

	pr_datagram (ihp, bp->it_get_enum == IT_IN ? 1 /* tem PDH */ : 0);

#if (0)	/*******************************************************/
	printf ("Resultado da busca pelo vetor: Em uso: %d\n\n", scb.y_n_itblock - in_free_list);

	printf ("\nin_free_list = %d\n", in_free_list);

	printf ("scb.y_itblock = %P\n", scb.y_itblock);

	bp = (ITBLOCK *)ACV (scb.y_itblock);
	bp = (ITBLOCK *)ACV (scb.y_end_itblock - 1);
	printf ("bp = %P\n", bp);
	printf ("Time = %s", ctime (&bp->it_time));
	printf ("Route = %P\n", bp->it_route);
#endif	/*******************************************************/


#if (0)	/*******************************************************/
	/*
	 *	x
	 */
	lidcan (lid, "_get_it_block_other");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	get_it_block_other = *(int *)ACV (sym.s_value);

	/*
	 *	x
	 */
	lidcan (lid, "_get_it_block_max_other");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	get_it_block_max_other = *(int *)ACV (sym.s_value);

	/*
	 *	x
	 */
	lidcan (lid, "_get_it_block_DATA_sema");

	if (kcntl (K_GETSYMGN, lid, &sym) < 0)
		error ("$*Não consegui obter o endereço de \"%s\"", lid);

	get_it_block_DATA_sema = ((SEMA *)ACV (sym.s_value))->e_count;

	/*
	 *	Imprime os valores
	 */
#if (0)	/*******************************************************/
	printf ("scb.y_n_itblock = %d\n", scb.y_n_itblock);

	printf ("\nin_free_list = %d\t", in_free_list);
	printf ("it_bl_count = %d\n",  ip->it_bl_count);

	printf ("\nget_it_block_other = %d\t", get_it_block_other);
	printf ("get_it_block_max_other = %d\t", get_it_block_max_other);
	printf ("_get_it_block_DATA_sema = %d\n", get_it_block_DATA_sema);
#endif	/*******************************************************/


	printf
	(	"Alocados em OUT_DATA:\t%d\n", 
		OUT_DATA = scb.y_n_itblock - get_it_block_max_other - get_it_block_DATA_sema
	);

	printf
	(	"Alocados em IN/OUT_CTL:\t%d\n", 
		IN_OUT_CTL = get_it_block_other
	);

	/*
	 *	Verifica a coerência
	 */
	if (ip->it_bl_count != scb.y_n_itblock - in_free_list)
	{
		printf
		(	"Valor de it_bl_count incorreto: %d :: %d\n", 
			ip->it_bl_count, scb.y_n_itblock - in_free_list
		);
		errors++;
	}

	if (OUT_DATA + IN_OUT_CTL != ip->it_bl_count)
	{
		printf
		(	"Valores de de (OUT_DATA + IN_OUT_CTL) incorreto: %d :: %d\n", 
			OUT_DATA + IN_OUT_CTL, ip->it_bl_count
		);
		errors++;
	}

	if (errors == 0)
		printf ("\nContadores coerentes: Blocos em uso: %d\n", ip->it_bl_count);
#endif	/*******************************************************/

	exit (exit_code);

}	/* end itck */

/*
 ****************************************************************
 *	Imprime as filas de um TCP EP				*
 ****************************************************************
 */
void
print_tcp_ep_queues (register const TCP_EP *tep, const SCB *sp, long INCR)
{
	register const ITBLOCK	*bp;
	int			n = 0;

	/*
	 *	Escreve as portas
	 */
	printf ("Portas = (%d, %d)\t", tep->tp_my_port, tep->tp_dst_port);

	/*
	 *	Bloco em construção
	 */
	if   ((bp = tep->tp_incomplete_outb) != NOITBLOCK)
	{
		n = 1;

		if (bp < sp->y_itblock || bp >= sp->y_end_itblock)
			printf ("Ponteiro INCOMPLETE inválido: %P\n", bp);
	}

	printf ("INCOMPLETE = %d", n);

	/*
	 *	Fila de saída
	 */
	for (bp = tep->tp_outq_first, n = 0; /* abaixo */; bp = bp->it_outq_forw, n++)
	{
		if (bp == NOITBLOCK)
		{
			printf (", OUTQ = %d", n);
			break;
		}

		if (bp < sp->y_itblock || bp >= sp->y_end_itblock)
		{
			printf ("Ponteiro OUTQ inválido: %P\n", bp);
			break;
		}

		bp = (ITBLOCK *)ACV (bp);
	}

	/*
	 *	Fila de ordenação
	 */
	for (bp = tep->tp_sortq_first, n = 0; /* abaixo */; bp = bp->it_inq_forw.inq_ptr, n++)
	{
		if (bp == NOITBLOCK)
		{
			printf (", SORTQ = %d", n);
			break;
		}

		if (bp < sp->y_itblock || bp >= sp->y_end_itblock)
		{
			printf ("Ponteiro SORTQ inválido: %P\n", bp);
			break;
		}

		bp = (ITBLOCK *)ACV (bp);
	}

	/*
	 *	Fila de entrada
	 */
	for (bp = tep->tp_inq_first, n = 0; /* abaixo */; bp = bp->it_inq_forw.inq_ptr, n++)
	{
		if (bp == NOITBLOCK)
		{
			printf (", INQ = %d", n);
			break;
		}

		if (bp < sp->y_itblock || bp >= sp->y_end_itblock)
		{
			printf ("Ponteiro INQ inválido: %P\n", bp);
			break;
		}

		bp = (ITBLOCK *)ACV (bp);
	}

	/*
	 *	Fila de listen
	 */
	for (bp = tep->tp_listenq_first, n = 0; /* abaixo */; bp = bp->it_inq_forw.inq_ptr, n++)
	{
		if (bp == NOITBLOCK)
		{
			printf (", LISTENQ = %d\n", n);
			break;
		}

		if (bp < sp->y_itblock || bp >= sp->y_end_itblock)
		{
			printf ("Ponteiro LISTENQ inválido: %P\n", bp);
			break;
		}

		bp = (ITBLOCK *)ACV (bp);
	}

}	/* end print_tcp_ep_queues */

/*
 ****************************************************************
 *	Imprime dados sobre um datagrama			*
 ****************************************************************
 */
void
pr_datagram (register const IP_H *iph, int PDH)
{
	int		ip_h_size, count;
	void		*area;
	register PSD_H	*ph;

	/*
	 *	x
	 */
	print_ip_header (iph, PDH);

	ip_h_size = iph->ip_h_size << 2;	/* Tamanho longs => bytes */

	area = (char *)iph + ip_h_size;
	count = ENDIAN_SHORT (iph->ip_size) - ip_h_size;

	printf ("Count = %d\n", count);

	/*
	 *	x
	 */
	if   (iph->ip_proto == TCP_PROTO)
	{
		printf ("NÃO tem pseudo ");
		print_tcp_header (area, count, 0);
	}
	else
	{
		ph = (PSD_H *)(area - sizeof (PSD_H));

		if (ENDIAN_SHORT (ph->ph_proto) == TCP_PROTO)
		{
			printf ("TEM pseudo ");
			print_tcp_header (area, count, 1);
		}
		else
		{
			printf ("Não achei o TCP\n");
		}
	}

#if (0)	/*******************************************************/

	ph = (PSD_H *)(area - sizeof (PSD_H));

	if   (ph->ph_proto == TCP_PROTO)
		print_tcp_header (area, count, PDH);
	elif (iph->ip_proto == TCP_PROTO)
		print_tcp_header (area, count, PDH);
#endif	/*******************************************************/

#if (0)	/*******************************************************/
	if   (iph->ip_proto == TCP_PROTO)
	{
		print_tcp_header (area, count, 0);
	}
	elif (PDH)
	{
		register PSD_H	*ph;

		ph = (PSD_H *)(area - sizeof (PSD_H));

		if (ph->ph_proto == TCP_PROTO)
			print_tcp_header (area, count, PDH);

	}
	else
	{
		if (iph->ip_proto == TCP_PROTO)
			print_tcp_header (area, count, PDH);
	}
#endif	/*******************************************************/

	printf ("\n");

}	/* end pr_datagram */

/*
 ****************************************************************
 *	Tenta imprimir os contrôles do segmento TCP		*
 ****************************************************************
 */
void
try_to_print_ctls (register const IP_H *iph)
{
	register TT_H		*thp;
	int			ip_h_size, count;
	void			*area;
	register PSD_H		*ph;
	int			tcp_h_size, data_cnt, flag, ctl;
	register const CTL_TB	*cp;

	if (!iph)
		return;

	/*
	 *	x
	 */
	ip_h_size = iph->ip_h_size << 2;	/* Tamanho longs => bytes */

	area = (char *)iph + ip_h_size;
	count = ENDIAN_SHORT (iph->ip_size) - ip_h_size;

	/*
	 *	x
	 */
	if   (iph->ip_proto == TCP_PROTO)
	{
		/* segue */
	}
	else
	{
		ph = (PSD_H *)(area - sizeof (PSD_H));

		if (ENDIAN_SHORT (ph->ph_proto) == TCP_PROTO)
		{
			/* segue */
		}
		else
		{
			return;
		}
	}

	/*
	 *	x
	 */
	thp = (TT_H *)(area - sizeof (PSD_H));

#if (0)	/*******************************************************/
	if (PDH)
	{
		printf ("src_addr = %s, ", t_addr_to_str (ENDIAN_LONG (thp->ph.ph_src_addr)));
		printf ("dst_addr = %s\n", t_addr_to_str (ENDIAN_LONG (thp->ph.ph_dst_addr)));
	}

	printf ("src_port = %d, ", ENDIAN_SHORT (thp->th.th_src_port));
	printf ("dst_port = %d\n", ENDIAN_SHORT (thp->th.th_dst_port));
#endif	/*******************************************************/

	tcp_h_size = thp->th.th_h_size << 2;

	data_cnt = count - tcp_h_size;

	ctl = thp->th.th_ctl;

	if (data_cnt > 0)
		ctl |= C_DATA;

#if (0)	/*******************************************************/
	printf ("seq = %u", ENDIAN_LONG (thp->th.th_seq_no));

	printf ("  ack = %u", ENDIAN_LONG (thp->th.th_ack_no));

	printf ("  wnd = %u", ENDIAN_SHORT (thp->th.th_window));
#endif	/*******************************************************/

	printf ("  (");

	for (cp = ctl_tb, flag = 0; cp->t_ctl != 0; cp++)
	{
		if (ctl & cp->t_ctl)
		{
			if (flag)
				printf ("|");

			printf (cp->t_ctl_nm); 	flag++;
		}
	}

	if (data_cnt > 0)
		printf ("=%d", data_cnt);

	printf (")");

#if (0)	/*******************************************************/
	if (data_cnt > 0)
		print_data_dump (area + tcp_h_size, data_cnt);
#endif	/*******************************************************/

}	/* end try_to_print_ctls */

/*
 ****************************************************************
 *	Imprime alguns dados sobre o cabeçalho IP		*
 ****************************************************************
 */
void
print_ip_header (register const IP_H *iph, int PDH)
{
	printf
	(	"h_size = %2d, id = %d, time_to_live = %d, proto = %d (%s)\n",
		iph->ip_h_size << 2, ENDIAN_SHORT (iph->ip_id),  iph->ip_time_to_live, 
		iph->ip_proto, get_proto_name (iph->ip_proto)
	);

	if (PDH == 0)
	{
		printf ("src_addr = %s, ", t_addr_to_str (ENDIAN_LONG (iph->ip_src_addr)));
		printf ("dst_addr = %s\n", t_addr_to_str (ENDIAN_LONG (iph->ip_dst_addr)));
	}

}	/* end print_ip_header */

/*
 ****************************************************************
 *	Obtém o nome do protocolo				*
 ****************************************************************
 */
const char *
get_proto_name (register int proto)
{
	register const PROTO_TB	*pp;

	for (pp = proto_tb; pp->t_proto != 0; pp++)
	{
		if (pp->t_proto == proto)
			return (pp->t_proto_nm);
	}

	return ("???");

}	/* end get_proto_name */

/*
 ****************************************************************
 *	Imprime alguns dados sobre o cabeçalho TCP		*
 ****************************************************************
 */
void
print_tcp_header (const void *area, int count, int PDH)
{
	register TT_H		*thp;
	register const CTL_TB	*cp;
	int			tcp_h_size, data_cnt, flag, ctl;

	thp = (TT_H *)(area - sizeof (PSD_H));

	printf ("\n");

	if (PDH)
	{
		printf ("src_addr = %s, ", t_addr_to_str (ENDIAN_LONG (thp->ph.ph_src_addr)));
		printf ("dst_addr = %s\n", t_addr_to_str (ENDIAN_LONG (thp->ph.ph_dst_addr)));
	}

	printf ("src_port = %d, ", ENDIAN_SHORT (thp->th.th_src_port));
	printf ("dst_port = %d\n", ENDIAN_SHORT (thp->th.th_dst_port));

	tcp_h_size = thp->th.th_h_size << 2;

	data_cnt = count - tcp_h_size;

	ctl = thp->th.th_ctl;

	if (data_cnt > 0)
		ctl |= C_DATA;

	printf ("seq = %u", ENDIAN_LONG (thp->th.th_seq_no));

	printf ("  ack = %u", ENDIAN_LONG (thp->th.th_ack_no));

	printf ("  wnd = %u", ENDIAN_SHORT (thp->th.th_window));

	printf ("  (");

	for (cp = ctl_tb, flag = 0; cp->t_ctl != 0; cp++)
	{
		if (ctl & cp->t_ctl)
		{
			if (flag)
				printf ("|");

			printf (cp->t_ctl_nm); 	flag++;
		}
	}

	if (data_cnt > 0)
		printf ("=%d", data_cnt);

	printf (")\n");

	if (data_cnt > 0)
		print_data_dump (area + tcp_h_size, data_cnt);

}	/* end print_tcp_header */

/*
 ****************************************************************
 *	Imprime uma ára de dados (no formato de "xd")		*
 ****************************************************************
 */
void
print_data_dump (const void *area, int count)
{
	register int		n, i, start = 0;
	register const char	*dump = area, *cp;

	printf ("\n");

	for (EVER)
	{
		n = (count >= LINESZ ? LINESZ : count);

		count -= LINESZ;

		if (n <= 0)
			break;

		printf ("%P:   ", start);

		for (i = 0; i < LINESZ; i++)
		{
			if ((i & 1) == 0)
				printf (" ");

			if (i == LINESZ/2)
				printf (" ");

			if (i < n)
				printf ("%02X", dump[i]);
			else
				printf ("  ");
		}

		printf ("     ");

		/*
		 *	Escreve caracteres em ascii
		 */
		for
		(	cp = dump, i = 0;
			i < LINESZ && i < n;
			cp++, i++
		)
		{
			register int	c;

			c = *cp;
			if (c < 0x20)
				c = '.';
			putchar (c);
		}

		putchar ('\n');

		/*
		 *	Se o endereço é multiplo de 128, pula uma linha
		 */
		start += n;

		dump += n;

		if ((start & 127) == 0)
			putchar ('\n');

	}	/* end for (EVER) */

}	/* end print_data_dump */


#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
receive_frame (register ITBLOCK *bp)
{
	register ETH_H	*ep;

#if (0)	/*******************************************************/
	/*
	 *	Se for um pacote PPP, ...
	 */
	if (bp->it_ppp_dev)
		{ receive_ppp_packet (bp); return; }
#endif	/*******************************************************/

	/*
	 *	Se não for um pacote ETHERNET, processa logo o datagrama IP
	 */
	if (!bp->it_ether_dev)
		{ receive_ip_datagram (bp); return; }

	/*
	 *	É ETHERNET: Verifica o protocolo do pacote
	 */
	ep = bp->it_u_area;

	switch (ENDIAN_SHORT (ep->it_type))
	{
	    case ETHERTYPE_IP:			/* Datagramas IP */
		bp->it_u_area  += sizeof (ETH_H);
		bp->it_u_count -= sizeof (ETH_H);

		receive_ip_datagram (bp);
		return;

	    case ETHERTYPE_ARP:			/* Protocolo ARP */
		ether_receive_arp_frame (bp);
		return;

	    default:				/* Protocolos desconhecidos */
#ifdef	MSG
#if (0)	/*******************************************************/
		if (itscb.debug.it_report_error)
		{
			prpgnm ("ether_in");
			printf
			(	"Recebendo pacote com protocolo %04X desconhecido\n",
				ENDIAN_SHORT (ep->it_type)
			);
		}
#endif	/*******************************************************/
#endif	MSG
		return;

	}	/* end switch */

}	/* end receive_frame */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
receive_ip_datagram (register ITBLOCK *bp)
{

}	/* end receive_ip_datagram */

/*
 ****************************************************************
 *	Edita o nome do modo					*
 ****************************************************************
 */
char *
edit_itblenum (ITBLENUM mode)
{
	switch (mode)
	{
	    case IT_IN:
		    return ("IT_IN");

	    case IT_OUT_DATA:
		    return ("IT_OUT_DATA");

	    case IT_OUT_CTL:
		    return ("IT_OUT_CTL");

	    default:
		    return ("???");

	}	/* end switch */

}	/* end edit_itblenum */

/*
 ****************************************************************
 *	Imprime uma mensagem de erro (cliente)			*
 ****************************************************************
 */
void
error (const char *format, ...)
{
	register va_list	args;
	register const char	*fmt = format;
	char			err, abrt;
	int			event, save_errno = errno;

	va_start (args, format);

	if (fmt == NOSTR)
		fmt = "$*Erro de entrada/saída de TCP";

	if ((abrt = *fmt) == '$')
		fmt++;

	if ((err = *fmt) == '*')
		fmt++;

	fprintf  (stderr, "%s: ", pgname);
	vfprintf (stderr, fmt, args);

	if (err == '*')
	{
		if (save_errno == TLOOK)
		{
			event = t_look (global_itn_fd);
	
			fprintf (stderr, " (%s)", t_strevent (global_itn_fd, event));
		}
		elif (save_errno != 0)
		{
			fprintf (stderr, " (%s)", strerror (save_errno));
		}
	}

	putc ('\n', stderr);

	if (abrt == '$')
		exit (1);

	va_end (args);

}	/* end error */

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
		"%s - edita/imprime o bloco de controle da INTERNET\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-etv] [-f <dev> [<sz>]] [-d [+-][<arg>]] [-p <arg>=<valor>]\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t--: Imprime os valores atuais dos parâmetros\n"
		"\t-e: Imprime as estruturas TCP em uso\n"
		"\t-t: Imprime as tabelas de endereços IP, roteamento e \"ethernet\"\n"
		"\t-v: Verboso\n"
		"\t-f: Consulta/altera o tamanho do FIFO de saída de <dev>\n"
		"\t-d: Depuração/controle (ver abaixo)\n"
		"\t-p: Altera parâmetros (ver abaixo)\n"
	);

	fprintf
	(	stderr,
		"\nArgumentos de depuração/controle:\n"
		"\t-:      Imprime o estado atual do argumento\n"
		"\t<arg>:  Liga o argumento <arg>\n"
		"\t-<arg>: Desliga o argumento <arg>\n\n"
		"\twi: Grava a entrada no arquivo \"itnet.debug\"\n"
		"\two: Grava a saída   no arquivo \"itnet.debug\"\n"
		"\tli: Relata a entrada de datagramas\n"
		"\tlo: Relata a saída   de datagramas\n"
		"\trt: Relata retransmissões\n"
		"\toq: Relata remoções da fila de retransmissão\n"
		"\tce: Causa erros propositais de \"checksum\"\n"
		"\ter: Relata erros\n"
		"\tin: Lista algumas infomações adicionais\n"
		"\tbs: Relata segmentos recusados\n"
		"\tlw: Relata tamanhos das janelas\n"
		"\ttt: Relata cálculos do SRTT\n"
		"\tgw: Executa a função de GATEWAY\n"
	);

	fprintf
	(	stderr,
		"\nArgumentos dos PARÂMETROS:\n"
		"\tWND_SZ:   Tamanho total da janela\n"
		"\tGOOD_WND: Tamanho \"bom\" de janela\n"
		"\tALPHA:    Amortecimento do SRTT (porcentagem)\n"
		"\tBETA:     Fator de variância do SRTT\n"
		"\tSRTT:     Valor inicial de SRTT em segundos\n"
		"\tN_TRANS:  No. de transmissões máximo para TCP\n"
		"\tWAIT:     Tempo máximo (seg.) permitido para TCP ainda não completado\n"
		"\tSILENCE:  Tempo máximo (segundos) permitido para TCP sem I/O\n"
		"\tMAX_SGSZ: Tamanho máximo para segmentos TCP\n"
	);

	exit (2);

}	/* end help */
