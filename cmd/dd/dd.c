/*
 ****************************************************************
 *								*
 *			dd.c					*
 *								*
 *	Copia e converte arquivos				*
 *								*
 *	Versão	1.0.0, de 01.10.86				*
 *		4.6.0, de 14.10.04				*
 *								*
 *	Módulo: dd						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] =  "Versão: 4.6.0, de 14.10.04";

#define	elif	else if
#define	EVER	;;
#define	FMODE	0666

typedef	enum	{ FALSE, TRUE }	BOOL;
typedef	unsigned short		ushort;

/*
 ******	Variáveis globais ***************************************
 */
entry int		vflag;			/* Verboso */

entry const char	*nmin,			/* Arquivo de entrada */
			*nmout;			/* Arquivo de saida */

entry char 		*bufin,			/* Buffer para leitura */
			*bufout,		/* Buffer para escrita */
			*ptout;			/* Ponteiro para buffer de saida */

entry int		inblsz = BL4SZ,		/* Tam. do registro de entrada */
			outblsz = BL4SZ,	/* Tam. do registro de saida */
			blsz,			/* Tam. dos dois registros */
	 		convbsz,		/* Tam. do buffer de conversão */
			count = -1,		/* Numero de registros a copiar */
			skip,			/* Posição de 'skip' na entrada */
			seek_given,		/* Foi dada o argumento 'seek' */
			seekn;			/* 'seek' na saida */

entry int		noconv,			/* Nenhuma conversão */
			noerrflag,		/* Opção 'noerror' */
			syncflag,		/* Opção 'sync' */
			sflag,			/* Fazer swap */
			mflag,			/* Conversão p/ minusculas */
			Mflag;			/* Conversão p/ Maiusculas */

entry int		fdin, fdout,		/* Descritores dos arquivos */
			bytesin,		/* Bytes lidos da entrada */
			bytesout,		/* Bytes a escrever */
			contconv,		/* Contador para opção 'cbs=' */
			partial_r,		/* Registros lidos incompletos */
			full_r,			/* Registros lidos completos */
			partial_w,		/* Registros escritos incompl. */
			full_w,			/* Registros escritos completos */
			trunc;			/* Registros truncados */

entry const char	*tabconv;		/* Tabela usada na conversão */

extern const char	etoa[],			/* Tabelas de conversão */
			atoe[],
			atoibm[];

entry void		(*swap) (void),		/* rotina de troca de bytes */
			(*convcar) (void);	/* conversão de caracteres */

/*
 ******	Protótipos de funções ***********************************
 */
long			getint (const char *, int);
BOOL			if_rmt (const char *);
void			swab0 (void);
void			swab1 (void);
void			swab2 (void);
void			direto (void);
void			parascii (void);
void			deascii (void);
void			convcase (char *, int);
void			guarda (int);
void			escreve (void);
void			listat (int);
void			on_signal (int, ...);
void			help (void);

/*
 ****************************************************************
 *	Copia e converte arquivos				*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	char		*cp;

	convcar = direto; argv++; /* Pula o argv[0] */

	/*
	 *	Examina os argumentos/opções
	 */
	for (/* vazio */; *argv != NOSTR; argv++)
	{
		if   (streq (*argv, "-v"))
		{
			vflag++;
		}
		elif (streq (*argv, "-H"))
		{
			help ();
		}
		elif (strncmp (*argv, "ibs=", 4) == 0)
		{
			inblsz = getint (*argv + 4, 0);
		}
		elif (strncmp (*argv, "obs=", 4) == 0)
		{
			outblsz = getint (*argv + 4, 0);
		}
		elif (strncmp (*argv, "cbs=", 4) == 0)
		{
			convbsz = getint (*argv + 4, 0);
		}
		elif (strncmp (*argv, "bs=", 3) == 0)
		{
			blsz = getint (*argv + 3, 0);
		}
		elif (strncmp (*argv, "if=", 3) == 0)
		{
			nmin = *argv + 3;
		}
		elif (strncmp (*argv, "of=", 3) == 0)
		{
			nmout = *argv + 3;
		}
		elif (strncmp (*argv, "skip=", 5) == 0)
		{
			skip = getint (*argv + 5, 1 /* zero OK */);
		}
		elif (strncmp (*argv, "seek=", 5) == 0)
		{
			seekn = getint (*argv + 5, 1 /* zero OK */); seek_given++;
		}
		elif (strncmp (*argv, "count=", 6) == 0)
		{
			count = getint (*argv + 6, 0);
		}
		elif (strncmp (*argv, "conv=", 5) == 0)
		{
			cp = (char *)*argv + 5;

			while (*cp != '\0')	
			{
				if   (*cp == ',')
				{
					cp++;
				}
				elif (strncmp (cp, "ebcdic", 6) == 0)
				{
					cp += 6; tabconv = atoe; convcar = deascii;
				}
				elif (strncmp (cp, "ibm", 3) == 0)
				{
					cp += 3; tabconv = atoibm; convcar = deascii;
				}
				elif (strncmp (cp, "ascii", 5) == 0)
				{
					cp += 5; tabconv = etoa; convcar = parascii;
				}
				elif (strncmp (cp, "lcase", 5) == 0)
				{
					cp += 5; mflag = 1;
				}
				elif (strncmp (cp, "ucase", 5) == 0)
				{
					cp += 5; Mflag = 1;
				}
				elif (strncmp (cp, "swab", 4) == 0)
				{
					switch (cp[4])
					{
					    case '0':
						sflag = 1; swap = swab0; break;

					    case '1':
						sflag = 1; swap = swab1; break;

					    case '2':
						sflag = 1; swap = swab2; break;

					    default:
						cp[5] = '\0'; *argv = cp; goto operror;

					}	/* end switch */

					cp += 5;
				}
				elif (strncmp (cp, "noerror", 7) == 0)
				{
					cp +=  7; noerrflag = 1;
				}
				elif (strncmp (cp, "sync", 4) == 0)
				{
					cp +=  4; syncflag = 1;
				}
				else
				{
					*argv = cp; goto operror;
				}

			}	/* end while () */

		}	/* end if strncmp ("conv=") */
		else
		{
		    operror:
			error ("Argumento inválido: \"%s\"\n", *argv); help ();
		}

	}	/* end for (..; *argv; ..) */

	/*
	 *	Um pouco de consistencia
	 */
	if (mflag && Mflag)
		{ error ("Conversão para maiúscula ou minúscula?\n"); help (); }

	/*
	 *	Abre o arquivo de entrada
	 */
	if (nmin != NOSTR)
		fdin = open (nmin, O_RDONLY);
	else
		fdin = dup (0);

	if (fdin < 0)
		error ("$*Não consegui abrir \"%s\"", nmin);

	/*
	 *	Abre o arquivo de saída
	 */
	if (nmout != NOSTR)
	{
		if (seek_given)
			fdout = open (nmout, O_WRONLY|O_CREAT,		FMODE);
		else
			fdout = open (nmout, O_WRONLY|O_CREAT|O_TRUNC,	FMODE);
	}
	else
	{
		fdout = dup (1);
	}

	if (fdout < 0)
		error ("$*Não consegui abrir \"%s\"", nmout);

	/*
	 *	Reserva area para buffers, verificando antes
	 *	opções relativas aos mesmos
	 */	
	if (blsz)
	{
		inblsz = outblsz = blsz;

		if (convcar == direto && !(mflag|Mflag))
			noconv++;
	}

	bufin = malloc (inblsz);

	if (noconv || (inblsz == outblsz && convbsz == 0))
		bufout = bufin;
	else
		bufout = malloc (outblsz);

	if (bufin == NOSTR || bufout == NOSTR)
		error (NOSTR);

	/*
	 *	Intercepta interrupções
	 */
	if (signal (SIGHUP, SIG_IGN) != SIG_IGN)
		signal (SIGHUP, on_signal);

	if (signal (SIGINT, SIG_IGN) != SIG_IGN)
		signal (SIGINT, on_signal);

	if (signal (SIGTERM, SIG_IGN) != SIG_IGN)
		signal (SIGTERM, on_signal);

	/*
	 *	Verifica opções skip e seek
	 */
	if (skip)
	{
		if (nmin != NOSTR && if_rmt (nmin))
		{
			while (skip--)
			{
				if (read (fdin, bufin, inblsz) < 0)
					goto seekerr;
			}
		}
		elif (llseek (fdin, (loff_t)inblsz * skip, NULL, L_SET) < 0)
		{
		    seekerr:
			error ("$*Erro em \"seek\" ou \"skip\"");
		}
	}

	if (seekn)
	{
		if (nmout != NOSTR && if_rmt (nmout))
		{
			error ("$Não é possível \"seek\" em \"/dev/rmt?\"\n");
		}
		else
		{
			if (llseek (fdout, (loff_t)outblsz * seekn, NULL, L_SET) < 0)
				goto seekerr;
		}
	}

	ptout = bufout;

	/*
	 *	Loop de leitura e verificação de validade
	 */
	for (EVER)
	{
		if (count >= 0  &&  (partial_r + full_r) >= count)
			{ escreve (); listat (1); }

		/*
		 *	Se for permitido erro, zera o buffer
		 */
		if (noerrflag)
			memset (bufin, 0, inblsz);

		/*
		 *	Leitura de um bloco
		 */
		if ((bytesin = read (fdin, bufin, inblsz)) == 0)
			{ escreve (); listat (1); }

		/*
		 *	Verifica se deu erro de leitura
		 */
		if (bytesin < 0)
 		{
			error ("*Erro de leitura no bloco %d", full_r + partial_r);

			if (!noerrflag)
				{ listat (0); exit (1); }

			bytesin = inblsz;

		}	/* end if erro de leitura */

		/*
		 *	Analisa o tamanho do bloco lido
		 */
		if (bytesin < inblsz)
		{
			partial_r++;

			if (syncflag)
				{ memset (bufin + bytesin, 0, inblsz - bytesin); bytesin = inblsz; }
		}
		else
		{
			full_r++;
		}

		/*
		 *	Processa swaps
		 */
		if (sflag)
			(*swap) ();

		/*
		 *	Processa conversões
		 */
		if (noconv)
			bytesout = bytesin;
		else
			(*convcar) ();

		/*
		 *	Se encheu um buffer, escreve
		 */
		if (bytesout >= outblsz)
			{ escreve (); ptout = bufout; }

	}	/* end  for (EVER) */

}	/* end main */

/*
 ****************************************************************
 *	Converte um número dado					*
 ****************************************************************
 */
long
getint (const char *str, int zero_is_good)
{
	long	 	n;
	const char	*ptr;

	n = strtol (str, &ptr, 0);

	switch (*ptr)
	{
	    case 'w':
	    case 'W':
		n *= 2;
		break;

	    case 'l':
	    case 'L':
		n *= 4;
		break;

	    case 'c':
	    case 'C':
		n *= 2 * 1024;
		break;

	    case 'k':
	    case 'K':
		n *= 1024;
		break;

	    case 'b':
	    case 'B':
		n *= BLSZ;
		break;

	    case '\0':
		break;
		
	    default:
		error ("$Sufixo de número inválido: '%c'", *ptr);

	}	/* end switch (*ptr) */

	if (n < 0 || n == 0 && !zero_is_good)
		error ("$Número inválido: \"%s\"", str);

	return (n);

}	/* end getint */

/*
 ****************************************************************
 *	Verifica se arquivo de entrada é fita			*
 ****************************************************************
 */
BOOL
if_rmt (const char *cp)
{
	if (strncmp (cp, "/dev/", 5) != 0)
		return (FALSE);

	cp += 5;

	if (*cp == 'n')
		cp++;

	if (strncmp (cp, "rmt", 3) == 0)
		return (TRUE);

	return (FALSE);

}	/* end if_rmt */

/*
 ****************************************************************
 *	Inverte cada par de bytes				*
 ****************************************************************
 */
void
swab0 (void)
{
	char		*cp;
	char		c;
	int		n;

	if (bytesin & 1)
		error ("$Com a opção \"swab0\", o tamanho da área deve ser par");

	for (cp = bufin, n = bytesin >> 1; n > 0; n--)
		{ c = *cp++; cp[-1] = cp[0]; *cp++ = c; }

}	/* end swab0 */

/*
 ****************************************************************
 *	Troca cada par de bytes					*
 ****************************************************************
 */
void
swab1 (void)
{
	ushort		*sp;
	ushort		s;
	int		n;

	if (bytesin & 3)
		error ("$Com a opção \"swab1\", o tamanho da área deve ser múltiplo de 4");

	for (sp = (ushort *)bufin, n = bytesin >> 2; n > 0; n--)
		{ s = *sp++; sp[-1] = sp[0]; *sp++ = s; }

}	/* end swab1 */

/*
 ****************************************************************
 *	Inverte uma palavra (4 bytes)				*
 ****************************************************************
 */
void
swab2 (void)
{
	char		*cp;
	char		c1, c2;
	int		n;

	if (bytesin & 3)
	{
		error
		(	"$Com a opção \"swab2\", o tamanho da área "
			"deve ser múltiplo de 4\n"
		);
	}

	for (cp = bufin, n = bytesin >> 2; n > 0; n--)
		{ c1 = cp[0]; *cp++ = cp[3]; c2 = cp[0]; *cp++ = cp[1]; *cp++ = c2; *cp++ = c1; }

}	/* end swab2 */

/*
 ****************************************************************
 *	Copia direto para bufout, sem conversão de codigo	*
 ****************************************************************
 */
void
direto (void)
{
	const char	*cp;
	int		i;

	if (mflag || Mflag)
		convcase (bufin, bytesin);

	cp = bufin;

	for (i = bytesin; i > 0; i--)
	{
		*ptout++ = *cp++;

		if (++bytesout >= outblsz)
			{ escreve (); ptout = bufout; }
	}

}	/* end direto */

/*
 ****************************************************************
 *	Converte caracteres para codigo ascii			*
 ****************************************************************
 */
void
parascii (void)
{
	const char	*cp;
	int		i;
	static int	espacos;

	cp = bufin;

	/*
	 *	Verifica se tem buffer de conversão
	 */
	if (convbsz == 0)
	{
		for (i = bytesin; i > 0; i--)
			guarda (tabconv[*cp++]);
	}
	else
	{
		char	c;

		/*
		 *	Tem buffer de conversão:
		 *	  Guarda brancos, independentemente;
		 *	  Se não branco, guarda '\n';
		 */
		for (i = bytesin; i > 0; i--)
		{
			c = tabconv[*cp++];

			if (c == ' ')
			{
				espacos++;
			}
			else
			{
				while (espacos)
				{
					guarda (' '); espacos--;
				}
				guarda (c);
			}

			if (++contconv >= convbsz)
			{
				guarda ('\n'); espacos = contconv = 0;
			}

		}	/* end for */

	}	/* end if (convbsz) */

}	/* end parascii */

/*
 ****************************************************************
 *	Converte caracteres de codigo ascii p/ ibm ou edcdic	*
 ****************************************************************
 */
void
deascii (void)
{
	char		*cp;
	int		i;

	cp = bufin;

	/*
	 *	Se acento pedido, converte todas as letras
	 */
	if (mflag | Mflag)
		convcase (cp, bytesin);

	/*
	 *	Verifica se tem buffer de conversão
	 */
	if (convbsz == 0)
	{
		for (i = bytesin; i > 0; i--)
			guarda (tabconv[*cp++]);
	}
	else
	{
		/*
		 *	Tem buffer de conversão:
		 *	  Se '\n', completa com brancos;
		 *	  Verifica tamanho do buffer, e guarda;
		 */
		for (i = bytesin; i > 0; i--, cp++)
		{
			if (*cp == '\n')
			{
				while (contconv++ < convbsz)
					guarda (tabconv[' ']);

				contconv = 0;
				continue;
			}

			if (contconv > convbsz)
				continue;

			if (contconv++ == convbsz)
				trunc++;

			guarda (tabconv[*cp]);

		}	/* end for */

	}	/* end if (convbsz) */

}	/* end deascii */

/*
 ****************************************************************
 *	Converte letras para maiuscula e/ou minuscula		*
 ****************************************************************
 */
void
convcase (char *buf, int n)
{
	int		c;

	if   (Mflag) for (/* vazio */; n > 0; n--, buf++)
	{
		c = *buf;

		if (islower (c))
			*buf = toupper (c);
	}
	elif (mflag) for (/* vazio */; n > 0; n--, buf++)
	{
		c = *buf;

		if (isupper (c))
			*buf = tolower (c);
	}

}	/* end convcase */

/*
 ****************************************************************
 *	Guarda caracter no buffer de saida			*
 ****************************************************************
 */
void
guarda (int c)
{
	*ptout++ = c;

	/*
	 *	Se encheu buffer de saida, escreve
	 */
	if (++bytesout >= outblsz)
	{
		if (convcar == parascii && (mflag | Mflag))
			convcase (bufout, bytesout);

		escreve ();
		ptout = bufout;
	}

}	/* end guarda */

/*
 ****************************************************************
 *	Escreve buffer de saida em nmout			*
 ****************************************************************
 */
void
escreve (void)
{
	if (bytesout)
	{
		if (bytesout == outblsz)
			full_w++;
		else
			partial_w++;

		if (write (fdout, bufout, bytesout) != bytesout)
		{
			error ("*Erro de escrita no bloco %d", full_w + partial_w);

			if (!noerrflag)
				{ listat (0); exit (1); }
		}

		bytesout = 0;
	}

}	/* end escreve */

/*
 ****************************************************************
 *	Informa estado final dos registros de E/S manipulados	*
 ****************************************************************
 */
void
listat (int arg)
{
	if (vflag)
	{
		error ("\t\tCompletos\tParciais");
		error ("Lidos     %11u\t%8u",   full_r, partial_r);
		error ("Escritos  %11u\t%8u", full_w, partial_w);

		if (trunc)
			error ("\nREGISTROS TRUNCADOS: %u\n", trunc);
	}

	if (arg)
		exit (0);

}	/* end listat */

/*
 ****************************************************************
 *	Termino precoce do programa, causado por interrupção	*
 ****************************************************************
 */
void
on_signal (int signo, ...)
{
	error ("Programa interrompido");
	listat (0);
	exit (1);

}	/* end on_signal */

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
		"%s - Copia e converte arquivos\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [-v] [<arg>=<valor>] ...\n",
		pgname, pgversion, pgname
	);

	fprintf
	(	stderr,
		"\nOpções:"
		"\t-v:\tVerboso\n"
	);

	fprintf
	(	stderr,
		"\nArgumentos:\n"
		"\tif:	Nome do arquivo de entrada\n"
		"\tof:	Nome do arquivo de saída\n"
		"\tbs:	Tamanho do bloco\n"
		"\tcount:	Número de blocos a processar\n"
		"\tskip:	No. de blocos a pular na entrada "
			 "antes de iniciar a cópia\n"
		"\tseek:	No. de blocos a pular na saída\n"
		"\tconv:	Diversas conversões podem ser "
			 "especificadas\n"
		"\t....:	....\n"
	);

	fprintf
	(	stderr,
		"\nObs.:\tPara maiores detalhes, "
			 "ver o manual de referência do comando\n"
	);

	exit (2);

}	/* end help */
