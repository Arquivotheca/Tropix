/*
 ****************************************************************
 *								*
 *			stty.c					*
 *								*
 *	consulta/altera parâmetros de terminais/vídeos		*
 *								*
 *	Versão	1.0.0, de 05.02.87				*
 *		3.0.2, de 22.01.98				*
 *								*
 *	Módulo: stty						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão:	3.0.2, de 22.01.98";

#define	elif	else if

entry int	aflag;		/* Todos os argumentos */

entry TERMIO	termio;		/* Modo do Terminal */

/*
 ****** Definição de estruturas *********************************
 */
typedef	struct
{
	char	*m_name;
	ushort	m_reset;
	ushort	m_set;

}	ATAB;

typedef	struct
{
	char	*m_name[3];
	ushort	m_ireset;
	ushort	m_iset;
	ushort	m_oreset;
	ushort	m_oset;
	ushort	m_creset;
	ushort	m_cset;
	ushort	m_lreset;
	ushort	m_lset;

}	MTAB;

/*
 ****** Protótipos de funções ***********************************
 */
void		prflags (void);
void		prcchars (void);
void		prmflags (void);
void		putctr (const char *, int);
void		pr_color_tb (void);
void		help (void);

/*
 ******	Caracteres de Controle **********************************
 */
typedef struct
{
	char	*r_nm;		/* Nome do caractere de controle */
	int	r_type;		/* Tipo da conversão */
	int	r_offset;	/* Deslocamento na estrutura TERMIO */

}	CONTROL;

const CONTROL	control[] =
{
	"intr",		0,	offsetof (TERMIO, t_intr),
	"quit",		0,	offsetof (TERMIO, t_quit),
	"erase",	0,	offsetof (TERMIO, t_erase),
	"kill",		0,	offsetof (TERMIO, t_kill),
	"eof",		0,	offsetof (TERMIO, t_eof),
	"min",		2,	offsetof (TERMIO, t_min),
	"eol",		0,	offsetof (TERMIO, t_eol),
	"time",		2,	offsetof (TERMIO, t_time),
	"res",		0,	offsetof (TERMIO, t_res),
	"switch",	0,	offsetof (TERMIO, t_swtch),
	"page",		1,	offsetof (TERMIO, t_page),
	"aerase",	0,	offsetof (TERMIO, t_aerase),
	"retype",	0,	offsetof (TERMIO, t_retype),
	"word",		0,	offsetof (TERMIO, t_word),
	"color",	3,	offsetof (TERMIO, t_color),

	"nline",	2,	offsetof (TERMIO, t_nline),
	"ncol",		2,	offsetof (TERMIO, t_ncol),
	NOSTR
};

/*
 ******	Modos de Controle ***************************************
 */
const ATAB	cflag[]	=
{
	"parenb",	PARENB,		PARENB,
	"-parenb",	PARENB,		0,
	"parodd",	PARODD,		PARODD,
	"-parodd",	PARODD,		0,
	"cs5",		CSIZE,		CS5,
	"cs6",		CSIZE,		CS6,
	"cs7",		CSIZE,		CS7,
	"cs8",		CSIZE,		CS8,
	"0",		CBAUD,		B0,
	"110",		CBAUD,		B110,
	"134",		CBAUD,		B134,
	"150",		CBAUD,		B150,
	"200",		CBAUD,		B200,
	"300",		CBAUD,		B300,
	"600",		CBAUD,		B600,
	"1200",		CBAUD,		B1200,
	"1800",		CBAUD,		B1800,
	"2400",		CBAUD,		B2400,
	"4800",		CBAUD,		B4800,
	"9600",		CBAUD,		B9600,
	"19200",	CBAUD,		B19200,
	"38400",	CBAUD,		B38400,
	"57600",	CBAUD,		B57600,
	"115200",	CBAUD,		B115200,
	"hupcl",	HUPCL,		HUPCL,
	"-hupcl",	HUPCL,		0,
	"cstopb",	CSTOPB,		CSTOPB,
	"-cstopb",	CSTOPB,		0,
	"cread",	CREAD,		CREAD,
	"-cread",	CREAD,		0,
	"clocal",	CLOCAL,		CLOCAL,
	"-clocal",	CLOCAL,		0,
	NOSTR
};

const ATAB	Cflag[]	=
{
	"1.2",		CBAUD,		B1200,
	"1.8",		CBAUD,		B1800,
	"2.4",		CBAUD,		B2400,
	"4.8",		CBAUD,		B4800,
	"9.6",		CBAUD,		B9600,
	"19.2",		CBAUD,		B19200,
	"38.4",		CBAUD,		B38400,
	"57.6",		CBAUD,		B57600,
	"115.2",	CBAUD,		B115200,
	NOSTR
};

/*
 ******	Modos de Entrada ****************************************
 */
const ATAB	iflag[]	=
{
	"ignbrk",	IGNBRK,		IGNBRK,
	"-ignbrk",	IGNBRK,		0,
	"brkint",	BRKINT,		BRKINT,
	"-brkint",	BRKINT,		0,
	"ignpar",	IGNPAR,		IGNPAR,
	"-ignpar",	IGNPAR,		0,
	"parmrk",	PARMRK,		PARMRK,
	"-parmrk",	PARMRK,		0,
	"inpck",	INPCK,		INPCK,
	"-inpck",	INPCK,		0,
	"istrip",	ISTRIP,		ISTRIP,
	"-istrip",	ISTRIP,		0,
	"inlcr",	INLCR,		INLCR,
	"-inlcr",	INLCR,		0,
	"igncr",	IGNCR,		IGNCR,
	"-igncr",	IGNCR,		0,
	"icrnl",	ICRNL,		ICRNL,
	"-icrnl",	ICRNL,		0,
	"iuclc",	IUCLC,		IUCLC,
	"-iuclc",	IUCLC,		0,
	"ixon",		IXON,		IXON,
	"-ixon",	IXON,		0,
	"ixany",	IXANY,		IXANY,
	"-ixany",	IXANY,		0,
	"ixoff",	IXOFF,		IXOFF,
	"-ixoff",	IXOFF,		0,
	NOSTR
};

/*
 ******	Modos de Saida ******************************************
 */
const ATAB	oflag[]	=
{
	"opost",	OPOST,		OPOST,
	"-opost",	OPOST,		0,
	"olcuc",	OLCUC,		OLCUC,
	"-olcuc",	OLCUC,		0,
	"onlcr",	ONLCR,		ONLCR,
	"-onlcr",	ONLCR,		0,
	"ocrnl",	OCRNL,		OCRNL,
	"-ocrnl",	OCRNL,		0,
	"onocr",	ONOCR,		ONOCR,
	"-onocr",	ONOCR,		0,
	"onlret",	ONLRET,		ONLRET,
	"-onlret",	ONLRET,		0,
	"ofill",	OFILL,		OFILL,
	"-ofill",	OFILL,		0,
	"ofdel",	OFDEL,		OFDEL,
	"-ofdel",	OFDEL,		0,
	"cr0",		CRDLY,		CR0,
	"cr1",		CRDLY,		CR1,
	"cr2",		CRDLY,		CR2,
	"cr3",		CRDLY,		CR3,
	"nl0",		NLDLY,		NL0,
	"nl1",		NLDLY,		NL1,
	"tab0",		TABDLY,		TAB0,
	"tab1",		TABDLY,		TAB1,
	"tab2",		TABDLY,		TAB2,
	"tab3",		TABDLY,		TAB3,
	"bs0",		BSDLY,		BS0,
	"bs1",		BSDLY,		BS1,
	"ff0",		FFDLY,		FF0,
	"ff1",		FFDLY,		FF1,
	"vt0",		VTDLY,		VT0,
	"vt1",		VTDLY,		VT1,
	NOSTR
};

/*
 ******	Modos Locais ********************************************
 */
const ATAB	lflag[]	=
{
	"isig",		ISIG,		ISIG,
	"-isig",	ISIG,		0,
	"icanon",	ICANON,		ICANON,
	"-icanon",	ICANON,		0,
	"xcase",	XCASE,		XCASE,
	"-xcase",	XCASE,		0,
	"echo",		ECHO,		ECHO,
	"-echo",	ECHO,		0,
	"echoe",	ECHOE,		ECHOE,
	"-echoe",	ECHOE,		0,
	"echok",	ECHOK,		ECHOK,
	"-echok",	ECHOK,		0,
	"echonl",	ECHONL,		ECHONL,
	"-echonl",	ECHONL,		0,
	"noflsh",	NOFLSH,		NOFLSH,
	"-noflsh",	NOFLSH,		0,

	"cntrlx",	CNTRLX,		CNTRLX,
	"-cntrlx",	CNTRLX,		0,
	"isokey",	ISOKEY,		ISOKEY,
	"-isokey",	ISOKEY,		0,
	"iso",		CODE,		ISO,
	"ascii",	CODE,		ASCII,
	"user1",	CODE,		USER1,
	"user2",	CODE,		USER2,
	"video",	VIDEO,		VIDEO,
	"-video",	VIDEO,		0,
	"icomm",	ICOMM,		ICOMM,
	"-icomm",	ICOMM,		0,
	NOSTR
};

/*
 ******	Tabela de Macro Argumentos ******************************
 */
const MTAB	mflag[] =
{
    "evenp",	"parity",		"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		PARENB|CSIZE|PARODD,	PARENB|CS7,		/* control */
		0,			0,			/* local */

    "oddp",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		PARENB|CSIZE|PARODD,	PARENB|CS7|PARODD,	/* control */
		0,			0,			/* local */

    "-parity",	"-evenp",		"-oddp",		/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		PARENB|CSIZE|PARODD,	CS8,			/* control */
		0,			0,			/* local */

    "cooked",	"-raw",			"-comm",		/* names */
		0,			0,			/* input */
		OPOST,			OPOST,			/* output */
		0,			0,			/* control */
		ICANON|ICOMM,		ICANON,			/* local */

    "raw",	"-cooked",		"",			/* names */
		0,			0,			/* input */
		OPOST,			0,			/* output */
		0,			0,			/* control */
		ICANON|ICOMM,		0,			/* local */

    "comm",	"",			"",			/* names */
		0,			0,			/* input */
		OPOST,			0,			/* output */
		0,			0,			/* control */
		ICANON|ICOMM,		ICOMM,			/* local */

    "nl",	"",			"",			/* names */
		ICRNL,			0,			/* input */
		ONLCR,			0,			/* output */
		0,			0,			/* control */
		0,			0,			/* local */

    "-nl",	"",			"",			/* names */
		ICRNL|INLCR|IGNCR,	ICRNL,			/* input */
		ONLCR|OCRNL|ONLRET,	ONLCR,			/* output */
		0,			0,			/* control */
		0,			0,			/* local */

    "lcase",	"LCASE",		"",			/* names */
		IUCLC,			IUCLC,			/* input */
		OLCUC,			OLCUC,			/* output */
		0,			0,			/* control */
		XCASE,			XCASE,			/* local */

    "-lcase",	"-LCASE",		"",			/* names */
		IUCLC,			0,			/* input */
		OLCUC,			0,			/* output */
		0,			0,			/* control */
		XCASE,			0,			/* local */

    "tabs",	"",			"",			/* names */
		0,			0,			/* input */
		TABDLY,			TAB0,			/* output */
		0,			0,			/* control */
		0,			0,			/* local */

    "-tabs",	"",			"",			/* names */
		0,			0,			/* input */
		TABDLY,			TAB3,			/* output */
		0,			0,			/* control */
		0,			0,			/* local */

    "echo",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		ECHO|ECHOE|ECHOK|ECHONL,ECHO|ECHOE|ECHOK|ECHONL,/* local */

    "-echo",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		ECHO|ECHOE|ECHOK|ECHONL,0,			/* local */

    "video",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		VIDEO,			VIDEO,			/* local */

    "-video",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		VIDEO,			0,			/* local */

    "isokey",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		ISOKEY,			ISOKEY,			/* local */

    "-isokey",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		ISOKEY,			0,			/* local */

    "iso",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		CODE,			ISO,			/* local */

    "ascii",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		CODE,			ASCII,			/* local */

    "user1",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		CODE,			USER1,			/* local */

    "user2",	"",			"",			/* names */
		0,			0,			/* input */
		0,			0,			/* output */
		0,			0,			/* control */
		CODE,			USER2,			/* local */

    NOSTR
};

/*
 ****** Tabela das cores ****************************************
 *
 *	0 == Black		Preto
 *	1 == Blue		Azul escuro
 *	2 == Green		Verde
 *	3 == Cyan		Azul claro
 *	4 == Red		Vermelho
 *	5 == Magenta		Violeta
 *	6 == Brown		Laranja
 *	7 == White		Branco
 *	8 == Gray
 *	9 == Bright blue
 *	A == Bright green
 *	B == Bright cyan
 *	C == Bright red
 *	D == Bright magenta
 *	E == Yellow
 *	F == Bright white
 */

/*
 ****************************************************************
 *	consulta/altera parâmetros de terminais/vídeos		*
 ****************************************************************
 */
int
main (int argc, register const char *argv[])
{
	register const CONTROL	*cp;
	register const char	*argp;
	register const ATAB	*ap;
	register const MTAB	*mp;
	register int		i, c;
	const char		*str;

	/*
	 *	Analisa as opções (não pode usar "getopt") 
	 */
	if (*++argv != NOSTR)
	{
		if   (streq (*argv, "-a") || streq (*argv, "-A"))
			{ aflag++; argv++; }
		elif (streq (*argv, "-t"))
			pr_color_tb ();
		elif (streq (*argv, "-H"))
			help ();
	}

	/*
	 *	Obtém o modo do terminal
	 */
	if (ioctl (2, TCGETS, &termio) < 0)
	{
		printf ("O dispositivo de \"stderr\" NÃO é um terminal\n");
		exit (1);
	}

	/*
	 *	Se não forem dados argumentos, apenas lista os modos
	 */
	if (*argv == NOSTR)
	{
		prflags ();
		exit (0);
	}

	/*
	 *	Coloca valores razoáveis para "nline" e "ncol"
	 */
	if (termio.t_nline == 0)
		termio.t_nline = 24;

	if (termio.t_ncol == 0)
		termio.t_ncol = 80;

	/*
	 *	Examina os argumentos dados
	 */
	while ((argp = *argv++) != NOSTR)
	{
		/*
		 *	"-page" tem tratamento especial
		 */
		if (streq (argp, "-page"))
			{ termio.t_page = 0; goto nextarg; }

		/*
		 *	"std" e "sane" tem tratamento especial
		 */
		if (streq (argp, "std") || streq (argp, "sane"))
		{
			termio.c_iflag =  ICRNL|IXON;
			termio.c_oflag =  OPOST|ONLCR;
			termio.c_cflag =  B9600|CS8|CLOCAL;
			termio.c_lflag =  ISIG|ICANON|ECHO|ECHOE|ECHOK|
				   	  CNTRLX|ISO|VIDEO;

			termio.t_intr   = CINTR;
			termio.t_quit   = CQUIT;
			termio.t_erase  = CERASE;
			termio.t_kill   = CKILL;
			termio.t_eof    = CEOT;
			termio.t_page   = termio.t_nline - 1;
			termio.t_aerase = CAERASE;
			termio.t_retype = CRTYPE;
			termio.t_word   = CWORD;

			goto nextarg;
		}

		/*
		 *	"ebc" tem tratamento especial
		 */
		if (streq (argp, "ebc"))
		{
			time_t		tempo;
			TM		*tp;

			printf ("\ef");		/* Acentos compridos */
	
			printf ("\e5");		/* Tira a última linha */
			for (i = 0; i < 159; i++)
				putchar (' ');
			putchar ('\f');
	
			time (&tempo);		/* Relógio */
	
			tp = localtime (&tempo);
	
			printf
			(	"\et%02d:%02d:%02d%02d/%02d/%02d",
				tp->tm_hour,
				tp->tm_min,
				tp->tm_sec,
				tp->tm_mday,
				tp->tm_mon + 1,
				tp->tm_year
			);

			goto nextarg;

		}	/* end "ebc" */

		/*
		 *	Verifica se é um caractere de controle
		 */
		for (cp = control; cp->r_nm != NOSTR; cp++)
		{
			char		*tp = (char *)&termio;

			if (!streq (cp->r_nm, argp))
				continue;

			if ((argp = *argv++) == NOSTR)
			{
				if (cp->r_type != 1)	/* Diferente de "page" */
				{
					printf
					(	"%s: Faltando o "
						"caracter de controle "
						"de \"%s\"\n",
						pgname, cp->r_nm
					);
					exit (1);
				}
			} 

			if   (cp->r_type == 1)
			{
				/* "page" */

				if (argp == NOSTR)
				{
					tp[cp->r_offset] = termio.t_nline - 1;
					argv--;
				}
				else
				{
					tp[cp->r_offset] = strtol (argp, &str, 0);

					if (*str != '\0')
					{
						printf
						(	"%s: Número de "
							"controle inválido: "
							"\"%s\"\n",
							pgname, argp
						);
						exit (1);
					}
				}
			}
			elif (cp->r_type == 2)
			{
				/* "min", "time", "nline", "ncol" */

				tp[cp->r_offset] = strtol (argp, &str, 0);

				if (cp->r_offset == offsetof (TERMIO, t_nline) && termio.t_page != 0)
					termio.t_page = termio.t_nline - 1;

				if (*str != '\0')
				{
					printf
					(	"%s: Número de "
						"controle inválido: "
						"\"%s\"\n",
						pgname, argp
					);
					exit (1);
				}
			}
			elif (cp->r_type == 3)	/* color */
			{
				c = strtol (argp, &str, 0);

				if ((unsigned)c > 15 || (*str != '\0' && *str != ','))
				{
					printf
					(	"%s: Cor inválida: "
						"\"%s\"\n",
						pgname, argp
					);
					exit (1);
				}

				tp[cp->r_offset] = c;

				if (*str != ',')
					goto nextarg;

				c = strtol (str + 1, &str, 0);

				if ((unsigned)c > 15 || *str != '\0')
				{
					printf
					(	"%s: Cor inválida: "
						"\"%s\"\n",
						pgname, argp
					);
					exit (1);
				}

				if (c == tp[cp->r_offset])
				{
					printf
					(	"%s: As 2 cores iguais?\n",
						pgname
					);
					exit (1);
				}

				tp[cp->r_offset] |= c << 4;
			}
			elif (argp[0] == '^')
			{
				tp[cp->r_offset] = argp[1] & 0x1F;
			}
			else
			{
				tp[cp->r_offset] = argp[0];
			}

			goto nextarg;

		}	/* end for */

		/*
		 *	Verifica se é um Modo de Controle
		 */
		for (ap = cflag; ap->m_name != NOSTR; ap++)
		{ 
			if (streq (argp, ap->m_name))
			{
				termio.c_cflag &= ~ap->m_reset;
				termio.c_cflag |=  ap->m_set;

				goto nextarg;
			}

		}	/* end for */

		/*
		 *	Verifica se é um sinônimo de velocidade
		 */
		for (ap = Cflag; ap->m_name != NOSTR; ap++)
		{ 
			if (streq (argp, ap->m_name))
			{
				termio.c_cflag &= ~ap->m_reset;
				termio.c_cflag |=  ap->m_set;

				goto nextarg;
			}

		}	/* end for */

		/*
		 *	Verifica se é um Modo de Entrada
		 */
		for (ap = iflag; ap->m_name != NOSTR; ap++)
		{ 
			if (streq (argp, ap->m_name))
			{
				termio.c_iflag &= ~ap->m_reset;
				termio.c_iflag |=  ap->m_set;

				goto nextarg;
			}

		}	/* end for */

		/*
		 *	Verifica se é um Modo de Saida
		 */
		for (ap = oflag; ap->m_name != NOSTR; ap++)
		{ 
			if (streq (argp, ap->m_name))
			{
				termio.c_oflag &= ~ap->m_reset;
				termio.c_oflag |=  ap->m_set;

				goto nextarg;
			}

		}	/* end for */

		/*
		 *	Verifica se é um Modo Local
		 */
		for (ap = lflag; ap->m_name != NOSTR; ap++)
		{ 
			if (streq (argp, ap->m_name))
			{
				termio.c_lflag &= ~ap->m_reset;
				termio.c_lflag |=  ap->m_set;

				goto nextarg;
			}

		}	/* end for */

		/*
		 *	Verifica se é um Macro Argumento
		 */
		for (mp = mflag; mp->m_name[0] != NOSTR; mp++)
		{ 
			if
			(	streq (argp, mp->m_name[0])	||
				streq (argp, mp->m_name[1])	||
				streq (argp, mp->m_name[2])
			)
			{
				termio.c_iflag &= ~mp->m_ireset;
				termio.c_iflag |=  mp->m_iset;
				termio.c_oflag &= ~mp->m_oreset;
				termio.c_oflag |=  mp->m_oset;
				termio.c_cflag &= ~mp->m_creset;
				termio.c_cflag |=  mp->m_cset;
				termio.c_lflag &= ~mp->m_lreset;
				termio.c_lflag |=  mp->m_lset;

				goto nextarg;
			}

		}	/* end for */

		printf ("%s: Argumento inválido: \"%s\"\n", pgname, argp);
		exit (1);

	    nextarg:
		/* vazio */ ;

	}	/* end while (argv) */

	if (ioctl (2, TCSETAW, &termio) < 0)
	{
		printf
		(	"Não consegui alterar o modo do terminal (%s)\n",
			strerror (errno)
		);
		exit (1);
	}

	return (0);

}	/* end stty */

/*
 ****************************************************************
 *	Imprime os Modos					*
 ****************************************************************
 */
void
prflags (void)
{
	register int		flag, i;
	register const ATAB	*mp;

	/*
	 *	Imprime o Nome da Linha
	 */
	printf ("Terminal/vídeo: %s\n\n", termio.c_name);

	prcchars ();

	/*
	 *	Verifica se foi dado a opção '-a'
	 */
	if (aflag == 0)
	{
		prmflags ();
		exit (0);
	}

	/*
	 *	Modos de Controle
	 */
	printf ("\nModos de Controle:");

	flag = termio.c_cflag;

	for (i = 0, mp = cflag; mp->m_name != NOSTR; mp++)
	{ 
		if ((flag & mp->m_reset) == mp->m_set)
		{
			if (i % 8 == 0)
				putc ('\n', stdout);

			printf ("\t%s", mp->m_name);	
			i++;
		}
	}

	putc ('\n', stdout);

	/*
	 *	Modos de Entrada
	 */
	printf ("\nModos de Entrada:");

	flag = termio.c_iflag;

	for (i = 0, mp = iflag; mp->m_name != NOSTR; mp++)
	{ 
		if ((flag & mp->m_reset) == mp->m_set)
		{
			if (i % 8 == 0)
				putc ('\n', stdout);
			printf ("\t%s", mp->m_name);	
			i++;
		}
	}

	putc ('\n', stdout);

	/*
	 *	Modos de Saida
	 */
	printf ("\nModos de Saida:");

	flag = termio.c_oflag;

	for (i = 0, mp = oflag; mp->m_name != NOSTR; mp++)
	{ 
		if ((flag & mp->m_reset) == mp->m_set)
		{
			if (i % 8 == 0)
				putc ('\n', stdout);
			printf ("\t%s", mp->m_name);	
			i++;
		}
	}

	putc ('\n', stdout);

	/*
	 *	Modos Locais
	 */
	printf ("\nModos Locais:");

	flag = termio.c_lflag;

	for (i = 0, mp = lflag; mp->m_name != NOSTR; mp++)
	{ 
		if ((flag & mp->m_reset) == mp->m_set)
		{
			if (i % 8 == 0)
				putc ('\n', stdout);
			printf ("\t%s", mp->m_name);	
			i++;
		}
	}

	putc ('\n', stdout);

}	/* end prflags */

/*
 ****************************************************************
 *	Imprime os Caracteres de Controle			*
 ****************************************************************
 */
void
prcchars (void)
{
	register int		i;
	register const CONTROL	*cp;
	register const char	*tp = (char *)&termio;

	/*
	 *	Caracteres de Controle
	 */
	printf ("Caracteres de Controle:");

	for (cp = control, i = 0; cp->r_nm != NOSTR; cp++, i++)
	{
		if (i % 4 == 0)
			putc ('\n', stdout);

		if   (cp->r_type == 1)	/* "page" */
		{
			if (tp[cp->r_offset] == 0)
				printf ("\t-%s\t", cp->r_nm);
			else
				printf ("\t%s = %d ", cp->r_nm, tp[cp->r_offset]);
		}
		elif (cp->r_type == 2)
		{
			/* "min", "time", "nline", "ncol" */

			printf ("\t%s = %d ", cp->r_nm, tp[cp->r_offset]);
		}
		elif (cp->r_type == 3)	/* color */
		{
			register int	c;

			c = tp[cp->r_offset];

			printf ("\t%s = %d,%d", cp->r_nm, c & 0x0F, c >> 4);
		}
		else
		{
			putctr  (cp->r_nm, tp[cp->r_offset]);
		}
	}

	putc ('\n', stdout);

}	/* end prcchars */

/*
 ****************************************************************
 *	Imprime os Macro Argumentos				*
 ****************************************************************
 */
void
prmflags (void)
{
	register int		i;
	register int		flag;
	register const MTAB	*mp;
	register const ATAB	*ap;

	printf ("\nMacro Argumentos:");

	/*
	 *	Põe a velocidade
	 */
	flag = termio.c_cflag;

	for (i = 0, ap = cflag; ap->m_name != NOSTR; ap++)
	{ 
		if (ap->m_reset != CBAUD)
			continue;

		if ((flag & ap->m_reset) == ap->m_set)
		{
			if (i % 8 == 0)
				putc ('\n', stdout);

			printf ("\t%s", ap->m_name);	
			i++;
		}
	}

	/*
	 *	Macro argumentos
	 */
	for (/*** i = ..., ***/ mp = mflag; mp->m_name[0] != NOSTR; mp++)
	{ 
		if
		(	(termio.c_iflag & mp->m_ireset) == mp->m_iset	&&
			(termio.c_oflag & mp->m_oreset) == mp->m_oset	&&
			(termio.c_cflag & mp->m_creset) == mp->m_cset	&&
			(termio.c_lflag & mp->m_lreset) == mp->m_lset
		)
		{
			if (i % 8 == 0)
				putc ('\n', stdout);

			printf ("\t%s", mp->m_name[0]);	
			i++;
		}
	}

	putc ('\n', stdout);

}	/* end prmflags */ 

/*
 ****************************************************************
 *	Poe um caracter visivelmente na tela			*
 ****************************************************************
 */
void
putctr (const char *name, register int c)
{
	if    (c < ' ')
		printf ("\t%s = '^%c'", name, 'A' - 1 + c);
	elif  (c != 0177)
		printf ("\t%s = '%c'", name, c);
	else
		printf ("\t%s = '^*'", name);

}	/* end putctr */

/*
 ****************************************************************
 *	Imprime a tabela de cores				*
 ****************************************************************
 */
void
pr_color_tb (void)
{
	printf
	(	"Tabela de cores:\n\n"
		" 0: Preto		 1: Azul escuro\n"
		" 2: Verde		 3: Azul claro\n"
		" 4: Vermelho		 5: Magenta\n"
		" 6: Marron		 7: Branco\n"
		" 8: Cinza		 9: Azul escuro brilhante\n"
		"10: Verde brilhante	11: Azul claro brilhante\n"
		"12: Vermelho brilhante	13: Magenta brilhante\n"
		"14: Amarelo		15: Branco brilhante\n"
	);

	exit (0);

}	/* end pr_color_tb */

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
		"%s - consulta/altera parâmetros de terminais/vídeos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-at] [<param> ou -<param> ....]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nOpções:"
		"\t-a: Imprime a tabela completa de parâmetros\n"
		"\t-t: Imprime a tabela de cores\n"
		"\nObs.:\tSem argumentos, é impressa uma tabela com os "
		"estados atuais\n\tdos parâmetros; com argumentos, os parâmetros "
		"são ativados\n\t(na forma <param>), ou desativados "
		"(na forma -<param>).\n"
	);

	exit (2);

}	/* end help */
