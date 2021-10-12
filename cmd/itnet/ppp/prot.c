/*
 ****************************************************************
 *								*
 *			prot.c					*
 *								*
 *	Examina o protocolo PPP					*
 *								*
 *	Versão	3.0.0, de 11.06.96				*
 *		3.0.0, de 11.06.96				*
 *								*
 *	Módulo: prot						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <xti.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 3.0.0, de 11.06.96";

#define	DEBUG

#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL
#define	elif	else if
#define	EVER	;;
#define STR(x)	# x
#define XSTR(x)	STR (x)

#define	LINESZ	16

entry int	exit_code = 0;	/* Código de retorno */

entry int	vflag;		/* Verbose */
entry int	Nflag;		/* Nomes dos pgms no <stdin> */

typedef unsigned long	IPADDR;

const unsigned short	fcstab[];

entry int	in_out_status;	/* < 2: entrada; >= 2: saída */

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
 ******	Protótipos de funções ***********************************
 ******	x **************************************************
 */
typedef struct { char code, len; short p; char max_slot, comp_slot; } VJ_OPT;

/*
 ******	Estrutura de controle do protocolo PPP ******************
 */
typedef struct slcompress	SLCOMPRESS;

typedef struct
{
	char			ppp_lcp_state;
	char			ppp_ipcp_state;
	char			ppp_driver_state;
	char			ppp_my_addr_added;

	char			ppp_protocol_field_compression;
	char			ppp_address_field_compression;
	char			ppp_vj_snd_compression;
	char			ppp_vj_rcv_compression;

	char			ppp_vj_slot_compression;
	char			ppp_vj_max_slot;

	unsigned long		ppp_my_magic_number;
	unsigned long		ppp_peer_magic_number;
	unsigned long		ppp_async_char_map;
	char			ppp_char_map_vec[256];

	IPADDR			ppp_my_addr;
	IPADDR			ppp_peer_addr;

	SLCOMPRESS		*ppp_slcompress_ptr;

}	PPP_STATUS;

#if (0)	/*******************************************************/
/*
 ******	Estrutura de controle do protocolo PPP ******************
 */
typedef struct
{
	unsigned long		ppp_magic_number;
	unsigned long		ppp_async_char_map;
	char			ppp_char_map_vec[256];
	char			ppp_protocol_field_compression;
	char			ppp_address_field_compression;

}	PPP_STATUS;
#endif	/*******************************************************/

entry PPP_STATUS	ppp_status;

/*
 ****** Protótipos de funções ***********************************
 */
int		get_hdlc (FILE *);
void		packet_proc (const char *, int, int);
void		lcp_proc (const char *, int);
void		lcp_configure (const char *);
void		lcp_echo (const char *);
void		ipcp_proc (const char *, int);
void		ipcp_configure_request (const char *);
int		pppfcs (int, const char *, int);
void		help (void);

/*
 ****************************************************************
 *	Examina o protocolo PPP					*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register int		opt, c = 0;
	register FILE		*fp;
	register char		*cp;
	register PPP_STATUS	*sp;
	int			count, oldfcs;
	const char		*log_nm;
	char			area[512];

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

	if ((log_nm = argv[0]) == NOSTR)
		log_nm = "log";

	/*
	 *	Abre o arquivo contendo a entrada PPP
	 */
	if ((fp = fopen (log_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"Não consegui abrir \"%s\" (%s)\n",
			log_nm, strerror (errno)
		);
		exit (1);
	}

	/*
	 *	Inicializa a estrutura de controle
	 */
	sp = &ppp_status;

	sp->ppp_my_magic_number = 0;
	memset (sp->ppp_char_map_vec, 0, sizeof (sp->ppp_char_map_vec));
	sp->ppp_protocol_field_compression = 0;
	sp->ppp_address_field_compression = 0;

	memset (sp->ppp_char_map_vec, 1, 32);
	sp->ppp_char_map_vec[0x7E] = 1;

	/*
	 *	Compõe um pacote PPP
	 */
	for (EVER)
	{
		/* Procura um 0x7E */

		if (c != 0x100)		/* Na verdade 0x7E */
		{
			if   ((c = get_hdlc (fp)) < 0)
				break;
			elif (c != 0x100)	/* Na verdade 0x7E */
				continue;
		}

		/* Procura um 0xFF ou 0x21 */

		if   ((c = get_hdlc (fp)) < 0)
			break;
		elif (c == 0x21)
		{
			cp = area; *cp++ = 0x21; oldfcs = 0xFFFF; goto found;
		}
		elif (c != 0xFF)
			continue;

		/* Procura um 0x03 */

		if   ((c = get_hdlc (fp)) < 0)
			break;
		elif (c != 0x03)
			continue;

		oldfcs = 0x3DE3; /* o valor 0x3DE3 já engloba o 0xFF e 0x03 */

		/* Procura o final do pacote */

		cp = area;

	    found:
		for (EVER)
		{
			if   ((c = get_hdlc (fp)) < 0)
				break;
			elif (c == 0x100)
				break;

			*cp++ = c;

		}

		if (c < 0)
			break;

		/* Temos um pacote */

		if ((count = cp - area) > 0)
			packet_proc (area, count, oldfcs);
	}

	printf ("\nEOF\n");

	return (0);

}	/* end prot */

/*
 ****************************************************************
 *	Processa os HDLC					*
 ****************************************************************
 */
int
get_hdlc (register FILE *fp)
{
	register int	c;
	static int	old_c = 0;

	if   ((c = getc (fp)) == '<')
	{
		if (old_c == '<')
			in_out_status++;
	}
	elif (c == '>')
	{
		if (old_c == '>')
			in_out_status--;
	}
	elif (c < 0)
	{
		return (c);
	}

	old_c = c;

	if   (c == 0x7E)
		return (0x100);
	if   (c != 0x7D)
		return (c);
	elif ((c = getc (fp)) <= 0)
		return (c);
	else
		return (c ^ 0x20);

}	/* end get_hdlc */

/*
 ****************************************************************
 *	Processa o pacote PPP					*
 ****************************************************************
 */
void
packet_proc (const char *area, int count, int oldfcs)
{
	register const char	*cp = area;
	register int		i;
	register int		c;
	int			n, cnt, start = 0, prot;
	unsigned short		fcs;

	/*
	 *	Verifica se é entrada ou saída
	 */
	if (in_out_status >= 2)
		printf ("\n***** Pacote de saída:\n");
	else
		printf ("\n***** Pacote de entrada:\n");

	/*
	 *	Realiza um dump
	 */
	for (cnt = count, cp = area; cnt > 0; cnt -= n)
	{
		if (cnt > LINESZ)
			n = LINESZ;
		else
			n = cnt;

		printf ("%P:   ", start);

		for (i = 0; i < LINESZ; i++)
		{
			if ((i & 1) == 0)
				printf (" ");

			if (i == LINESZ/2)
				printf (" ");

			if (i < n)
				printf ("%02X", cp[i]);
			else
				printf ("  ");
		}

		printf ("     ");

		/*
		 *	Escreve caracteres em ISO
		 */
		for
		(	i = 0;
			i < LINESZ && i < n;
			cp++, i++
		)
		{
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

		if (((start) & 127) == 0)
			putchar ('\n');

	}	/* end while */

	count -= 2;	/* Tira o FCS */

	/*
	 *	Calcula o CRC; o valor 0x3DE3 já engloba o 0xFF e 0x03
	 *	Repare que estamos comparando coisas com o mesmo ENDIAN
	 */
	fcs = pppfcs (oldfcs, area, count); fcs = ~fcs;

	if (fcs != (*(unsigned short *)&area[count]))
	{
		printf
		(	"PPP: Erro no FCS (0x%04X :: 0x%04X)\n",
			fcs, (*(unsigned short *)&area[count])
		);
		return;
	}
#if (0)	/*******************************************************/
	else
	{
		printf ("PPP: FCS OK\n");
	}
#endif	/*******************************************************/

	/*
	 *	Examina o protocolo
	 */
	switch (prot = ENDIAN_SHORT (*(short *)area))
	{
	    case 0xC021:
		lcp_proc (area + 2, count - 2);	/* Ajusta o 0xC021 */
		break;

	    case 0x8021:
		ipcp_proc (area + 2, count - 2); /* Ajusta o 0x8021 */
		break;

	    default:
		printf ("PPP: Protocolo %04X desconhecido\n", prot);
		return;
	}

}	/* end packet_proc */

/*
 ****************************************************************
 *	Processa o protocolo LCP				*
 ****************************************************************
 */
void
lcp_proc (register const char *area, int count)
{
	switch (*area)
	{
	    case 1:	/* Configure Request */
		printf ("LCP: Configure Request\n");
		lcp_configure (area + 1);
		return;

	    case 2:	/* Configure Acknowledge */
		printf ("LCP: Configure Acknowledge\n");
		lcp_configure (area + 1);
		return;

	    case 9:	/* Echo request */
		printf ("LCP: Echo request\n");
		lcp_echo (area + 1);
		return;

	    case 10:	/* Echo reply */
		printf ("LCP: Echo reply\n");
		lcp_echo (area + 1);
		return;

	    default:
		printf ("LCP: Código desconhecido: %d\n", *area);
		return;
	}

}	/* end lcp_proc */

/*
 ****************************************************************
 *	LCP: Configure request					*
 ****************************************************************
 */
void
lcp_configure (const char *area)
{
	register const char	*cp = area;
	register PPP_STATUS 	*sp = &ppp_status;
	int			id, count, op, len;
	unsigned long		mask;

	id    = *cp++;
	count = ENDIAN_SHORT (*(short *)cp); cp += 2;
	count -= 4;	/* Desconta o "conf req", ID, e o próprio tamanho */

	printf ("ID = %d, count = %d\n", id, count);

	for (EVER)
	{
		if (count <= 0)
			break;

		op  = cp[0];
		len = cp[1];

		switch (op)
		{
		    case 2:	/* Async-Control-Character-Map */
			mask = ENDIAN_LONG (*(long *)(cp + 2));

			sp->ppp_async_char_map = mask;
#ifdef	DEBUG
			printf ("Char_map %P\n", mask);
#endif	DEBUG
#if (0)	/*******************************************************/
	register int		c;
			for (c = 0; c < 32; c++)
			{
				sp->ppp_char_map_vec[c] = (mask & 1); mask >>= 1;
#ifdef	DEBUG
				if (sp->ppp_char_map_vec[c])
					printf ("Char_map ON para 0x%02X\n", c);
#endif	DEBUG
			}
#endif	/*******************************************************/
			break;

		    case 5:	/* Magic-Number */
			sp->ppp_my_magic_number = ENDIAN_LONG (*(long *)(cp + 2));
#ifdef	DEBUG
			printf ("Magic_number = %P\n", sp->ppp_my_magic_number);
#endif	DEBUG
			break;

		    case 7:	/* Protocol-Field-Compression */
			sp->ppp_protocol_field_compression++;
#ifdef	DEBUG
			printf ("Protocol-Field-Compression\n");
#endif	DEBUG
			break;

		    case 8:	/* Address-and-Control-Field-Compression */
			sp->ppp_address_field_compression++;
#ifdef	DEBUG
			printf ("Address-and-Control-Field-Compression\n");
#endif	DEBUG
			break;

		    default:
			printf ("LCP: Configure request: Opção %d desconhecida\n", op);
			break;
		}

		cp += len; count -= len;

	}	/* end for (EVER) */

	if (count != 0)
		printf ("LCP: Configure request: Count (%d) NÃO zerou\n", count);


}	/* end lcp_configure */

/*
 ****************************************************************
 *	LCP: Echo						*
 ****************************************************************
 */
void
lcp_echo (const char *area)
{
	register const char	*cp = area;
	int			id, count;

	id    = *cp++;
	count = ENDIAN_SHORT (*(short *)cp); cp += 2;
	count -= 4;	/* Desconta o "conf req", ID, e o próprio tamanho */

	printf
	(	"LCP: Echo: id = %d, count = %d\n",
		id, count, ENDIAN_LONG (*(long *)cp)
	);

}	/* end lcp_echo */

/*
 ****************************************************************
 *	Processa o protocolo IPCP				*
 ****************************************************************
 */
void
ipcp_proc (register const char *area, int count)
{
	switch (*area)
	{
	    case 1:	/* Configure Request */
		printf ("IPCP: Configure Request\n");
		ipcp_configure_request (area + 1);
		return;

	    case 2:	/* Configure Acknowledge */
		printf ("IPCP: Configure Acknowledge\n");
		ipcp_configure_request (area + 1);
		return;

	    case 3:	/* Configure NAK */
		printf ("IPCP: Configure NAK\n");
		ipcp_configure_request (area + 1);
		return;

	    case 4:	/* Configure Reject */
		printf ("IPCP: Configure REJECT\n");
		ipcp_configure_request (area + 1);
		return;

	    default:
		printf ("IPCP: Código desconhecido: %d\n", *area);
		return;
	}

}	/* end ipcp_proc */

/*
 ****************************************************************
 *	IPCP: Configure request					*
 ****************************************************************
 */
void
ipcp_configure_request (const char *area)
{
	register const char	*cp = area;
	int			id, count, op, len;
	int			compression;
	IPADDR			ipaddr;
	const VJ_OPT		*vp;

	id    = *cp++;
	count = ENDIAN_SHORT (*(short *)cp); cp += 2;
	count -= 4;	/* Desconta o "conf req", ID, e o próprio tamanho */

	printf ("IPCP: id = %d, count = %d\n", id, count);

	for (EVER)
	{
		if (count <= 0)
			break;

		op  = cp[0];
		len = cp[1];

		switch (op)
		{
		    case 2:	/* IP-Compression-Protocol */
			vp = (VJ_OPT *)cp;

			printf
			(	"len = %d, comp = %02X, "
				"slot_comp = %02X, max_slot = %02X\n",
				len, vp->p, vp->comp_slot, vp->max_slot
			);
			compression = ENDIAN_SHORT (*(unsigned short *)(cp + 2));
#ifdef	DEBUG
			printf ("Compression = %04X\n", compression);
#endif	DEBUG
			break;

		    case 3:	/* IP address */
			ipaddr = ENDIAN_LONG (*(long *)(cp + 2));
#ifdef	DEBUG
			printf ("IP Address = %s\n", t_addr_to_str (ipaddr));
#endif	DEBUG
			break;

		    default:
			printf ("IPCP: Configure request: Opção %d desconhecida\n", op);
			break;
		}

		cp += len; count -= len;

	}	/* end for (EVER) */

	if (count != 0)
		printf ("IPCP: Configure request: Count (%d) NÃO zerou\n", count);


}	/* end ipcp_configure_request */

/*
 ****************************************************************
 *	Cálculo do CRC						*
 ****************************************************************
 */
int
pppfcs (register int fcs, register const char *cp, register int count)
{
	while (count-- > 0)
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xFF];

	return (fcs);

}	/* end pppfcs */

/*
 ****************************************************************
 *	Tabela de CRC						*
 ****************************************************************
 */
const unsigned short	fcstab[256] =
{
	0x0000,0x1189,0x2312,0x329B,0x4624,0x57AD,0x6536,0x74BF,
	0x8C48,0x9DC1,0xAF5A,0xBED3,0xCA6C,0xDBE5,0xE97E,0xF8F7,
	0x1081,0x0108,0x3393,0x221A,0x56A5,0x472C,0x75B7,0x643E,
	0x9CC9,0x8D40,0xBFDB,0xAE52,0xDAED,0xCB64,0xF9FF,0xE876,
	0x2102,0x308B,0x0210,0x1399,0x6726,0x76AF,0x4434,0x55BD,
	0xAD4A,0xBCC3,0x8E58,0x9FD1,0xEB6E,0xFAE7,0xC87C,0xD9F5,
	0x3183,0x200A,0x1291,0x0318,0x77A7,0x662E,0x54B5,0x453C,
	0xBDCB,0xAC42,0x9ED9,0x8F50,0xFBEF,0xEA66,0xD8FD,0xC974,
	0x4204,0x538D,0x6116,0x709F,0x0420,0x15A9,0x2732,0x36BB,
	0xCE4C,0xDFC5,0xED5E,0xFCD7,0x8868,0x99E1,0xAB7A,0xBAF3,
	0x5285,0x430C,0x7197,0x601E,0x14A1,0x0528,0x37B3,0x263A,
	0xDECD,0xCF44,0xFDDF,0xEC56,0x98E9,0x8960,0xBBFB,0xAA72,
	0x6306,0x728F,0x4014,0x519D,0x2522,0x34AB,0x0630,0x17B9,
	0xEF4E,0xFEC7,0xCC5C,0xDDD5,0xA96A,0xB8E3,0x8A78,0x9BF1,
	0x7387,0x620E,0x5095,0x411C,0x35A3,0x242A,0x16B1,0x0738,
	0xFFCF,0xEE46,0xDCDD,0xCD54,0xB9EB,0xA862,0x9AF9,0x8B70,
	0x8408,0x9581,0xA71A,0xB693,0xC22C,0xD3A5,0xE13E,0xF0B7,
	0x0840,0x19C9,0x2B52,0x3ADB,0x4E64,0x5FED,0x6D76,0x7CFF,
	0x9489,0x8500,0xB79B,0xA612,0xD2AD,0xC324,0xF1BF,0xE036,
	0x18C1,0x0948,0x3BD3,0x2A5A,0x5EE5,0x4F6C,0x7DF7,0x6C7E,
	0xA50A,0xB483,0x8618,0x9791,0xE32E,0xF2A7,0xC03C,0xD1B5,
	0x2942,0x38CB,0x0A50,0x1BD9,0x6F66,0x7EEF,0x4C74,0x5DFD,
	0xB58B,0xA402,0x9699,0x8710,0xF3AF,0xE226,0xD0BD,0xC134,
	0x39C3,0x284A,0x1AD1,0x0B58,0x7FE7,0x6E6E,0x5CF5,0x4D7C,
	0xC60C,0xD785,0xE51E,0xF497,0x8028,0x91A1,0xA33A,0xB2B3,
	0x4A44,0x5BCD,0x6956,0x78DF,0x0C60,0x1DE9,0x2F72,0x3EFB,
	0xD68D,0xC704,0xF59F,0xE416,0x90A9,0x8120,0xB3BB,0xA232,
	0x5AC5,0x4B4C,0x79D7,0x685E,0x1CE1,0x0D68,0x3FF3,0x2E7A,
	0xE70E,0xF687,0xC41C,0xD595,0xA12A,0xB0A3,0x8238,0x93B1,
	0x6B46,0x7ACF,0x4854,0x59DD,0x2D62,0x3CEB,0x0E70,0x1FF9,
	0xF78F,0xE606,0xD49D,0xC514,0xB1AB,0xA022,0x92B9,0x8330,
	0x7BC7,0x6A4E,0x58D5,0x495C,0x3DE3,0x2C6A,0x1EF1,0x0F78
};

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
		"%s - examina o protocolo PPP\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [log_nm]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v: Verboso\n"
	);

	exit (2);

}	/* end help */
