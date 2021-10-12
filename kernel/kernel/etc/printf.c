/*
 ****************************************************************
 *								*
 *			printf.c				*
 *								*
 *	Rotinas de E/S para a Console				*
 *								*
 *	Versão	3.0.0, de 28.06.94				*
 *		4.9.0, de 02.10.06				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2006 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <stdarg.h>

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"
#include "../h/region.h"

#include "../h/a.out.h"
#include "../h/stat.h"
#include "../h/disktb.h"
#include "../h/tty.h"
#include "../h/signal.h"
#include "../h/uproc.h"

#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry char	*dmesg_area;		/* Área de mensagens */
entry char	*dmesg_ptr;
const char	*dmesg_end;

static char	printf_area[120];	/* Área para compor o texto */

/*
 ****** Protótipos de funções ***********************************
 */
int	strprintf (char *, int, const char *, int *);
void	printn (char **, int *, unsigned int, unsigned int, int, int);

/*
 ****************************************************************
 *	Impressão na console ("printf") simplificada		*
 ****************************************************************
 */
void
printf (const char *fmt, ...)
{
	const char	*cp;
	int		c;

	strprintf (printf_area, sizeof (printf_area), fmt, va_first (fmt));

	for (cp = printf_area; (c = *cp) != '\0'; cp++)
		putchar (c);

	video_clr_last_line ();

}	/* end printf */

/*
 ****************************************************************
 *	Impressão na console com lista variável de args		*
 ****************************************************************
 */
void
vprintf (const char *fmt, int *args)
{
	const char	*cp;
	int		c;

	strprintf (printf_area, sizeof (printf_area), fmt, args);

	for (cp = printf_area; (c = *cp) != '\0'; cp++)
		putchar (c);

	video_clr_last_line ();

}	/* end vprintf */

/*
 ****************************************************************
 *	Edição de textos 					*
 ****************************************************************
 */
int
snprintf (char *area, int size, const char *fmt, ...)
{
	return (strprintf (area, size, fmt, va_first (fmt)));

}	/* end snprintf */

/*
 ****************************************************************
 *	Edição de textos (menos elegante)			*
 ****************************************************************
 */
int
sprintf (char *area, const char *fmt, ...)
{
	return (strprintf (area, 120, fmt, va_first (fmt)));

}	/* end sprintf */

/*
 ****************************************************************
 *	Edição de textos 					*
 ****************************************************************
 */
int
strprintf (char *area, int size, const char *fmt, int *args)
{
	char		*str = area;
	int		c, w, n, sinal;
	char		zero;
	const char	*cp;
	ulong		arg;

	/*
	 *	Inicialização; reserva um byte para o '\0'
	 */
	if (size > 0)
		size--;

	/*
	 *	Malha principal
	 */
	for (EVER)
	{
		/*
		 *	Texto no formato
		 */
		while ((c = *fmt++) != '%')
		{
			if (c == '\0')
				{ *str = c; return (str - area); }

			if (size > 0)
				{ *str++ = c; size--; }
		}

		/*
		 *	Examina o próprio "%"
		 */
		if (*fmt == '%')
		{
			if (size > 0)
				{ *str++ = '%'; size--; }

			fmt++; continue;
		}

		/*
		 *	Retira a largura do campo e o operando
		 */
		if ((zero = *fmt) == '0')
			fmt++;

		for (w = 0; (c = *fmt++) >= '0' && c <= '9'; /* vazio */)
			w = w * 10 + c - '0';

		if (zero == '0')
			w = -w;

		arg = *args++; sinal = 0;

		/*
		 *	Analisa a conversão
		 */
		switch (c)
		{
			/*
			 *	Decimal
			 */
		    case 'd':
		    case 'D':
			if ((int)arg < 0)
				{ arg = -arg; sinal++; }
		    case 'u':
		    case 'U':
			printn (&str, &size, arg, 10, w, sinal);
			break;

			/*
			 *	Hexadecimal
			 */
		    case 'x':
			arg &= 0xFFFF;
		    case 'X':
			printn (&str, &size, arg, 16, w, 0);
			break;

			/*
			 *	Endereço (ponteiro)
			 */
		    case 'p':
		    case 'P':
			printn (&str, &size, arg >> 16,	16, 4, 0);

			if (size > 0)
				{ *str++ = ' '; size--; }

			printn (&str, &size, arg & 0xFFFF, 16, 4, 0);
			break;

			/*
			 *	Octal
			 */
		    case 'o':
			arg &= 0xFFFF;
		    case 'O':
			printn (&str, &size, arg, 8, w, 0);
			break;

			/*
			 *	Cadeia de caracteres
			 */
		    case 's':
			if (arg == 0)
				cp = "(NULO)";
			else
				cp = (const char *)arg;

		    case_s:
			for (/* acima */; (c = *cp); cp++, w--)
			{
				if (size > 0)
					{ *str++ = c; size--; }
			}

			while (w-- > 0)
			{
				if (size > 0)
					{ *str++ = ' '; size--; }
			}

			break;

			/*
			 *	Um caractere
			 */
		    case  'c':
			if (size > 0)
				{ *str++ = arg; size--; }
			break;

			/*
			 *	Seqüência de bits
			 */
		    case 'b':
		    case 'B':
			cp = (char *)*args++;

			printn (&str, &size, arg, 16, w, 0);

			if (!arg)
				break;

			for (c = 0; n = *cp++; /* vazio */)
			{
				if (arg & (1 << (n - 1)))
				{
					if (c)
					{
						if (size > 0)
							{ *str++ = ','; size--; }
					}
					else
					{
						if (size > 0)
							{ *str++ = ' '; size--; }

						if (size > 0)
							{ *str++ = '<'; size--; }
					}

					for (/* vazio */; (n = *cp) > ' '; cp++)
					{
						if (size > 0)
							{ *str++ = n; size--; }
					}

					c = 1;
				}
				else
				{
					for (/* vazio */; *cp > ' '; cp++)
						/* vazio */;
				}
			}

			if (c)
			{
				if (size > 0)
					{ *str++ = '>'; size--; }
			}

			break;

			/*
			 *	O nome de um dispositivo
			 */
		    case 'v':
		    {
			const DISKTB    *dp;

			if (arg == NODEV)
				{ cp = "NODEV"; goto case_s; }

			dp = &disktb[MINOR (arg)];

			if (/*** dp >= disktb && ***/ dp < next_disktb && dp->p_dev == arg)
				{ cp = dp->p_name; goto case_s; }

			printn (&str, &size, MAJOR (arg), 10, 0, 0);

			if (size > 0)
				{ *str++ = '/'; size--; }

			printn (&str, &size, MINOR (arg), 10, 0, 0);

			break;
		    }

			/*
			 *	O código de um tipo de sistema de arquivos
			 */
		    case 'f':
			if ((unsigned)arg < FS_LAST)
				{ cp = file_code_name_vec[arg]; goto case_s; }

			printn (&str, &size, arg, 10, 0, 0);

			break;

			/*
			 *	Uma mensagem de erro
			 */
		    case 'r':
			if (arg >= 0 && arg < sys_nerr)
				cp = sys_errlist[arg];
			else
				cp = "Erro Desconhecido";

			goto case_s;

			/*
			 *	Tipo de arquivo
			 */
		    case 'm':
			switch (arg & S_IFMT)
			{
			    case S_IFREG: cp = "REG";	break; 
			    case S_IFDIR: cp = "DIR";	break;
			    case S_IFIFO: cp = "FIFO";	break;
			    case S_IFBLK: cp = "BLK";	break;
			    case S_IFCHR: cp = "CHR";	break;
			    default:	  cp = "???";	break;
			}

			goto case_s;

			/*
			 *	Mensagem estilo "prpgnm": "função (marte, pgname)"
			 */
		    case 'g':
		    {
			const struct frameb { struct frameb *fp; unsigned long ret; } *p;
			const SYM       *sp;

			args--;			/* Não tem argumento */

			p = get_fp ();		/* Endereço de "printf", ... */
			p = p->fp;		/* Endereço de quem chamou "printf" */

			sp = getsyment (p->ret);

			for (cp = sp->s_name + 1; (c = *cp) && size > 0; cp++, size--)
				*str++ = c;

			if (size > 1)
				{ str[0] = ' '; str[1] = '('; str += 2; size -= 2; }

			for (cp = scb.y_uts.uts_nodename; (c = *cp) && size > 0; cp++, size--)
				*str++ = c;

			if (size > 1)
				{ str[0] = ','; str[1] = ' '; str += 2; size -= 2; }

			for (cp = u.u_pgname; (c = *cp) && size > 0; cp++, size--)
				*str++ = c;

			if (size > 0)
				{ *str++ = ')'; size--; }

			break;
		    }

		}	/* end switch */

	}	/* end for (EVER) */

}	/* end strprintf */

/*
 ****************************************************************
 *	Imprime um Inteiro 					*
 ****************************************************************
 */
void
printn (char **str_ptr, int *size_ptr, unsigned n, unsigned base, int w, int sinal)
{
	char		*str, *cp, *end_area;
	int		size;
	char		area[32];
	int		i;
	char		c;

	/*
	 *	Inicializa os valores
	 */
	str = *str_ptr; size = *size_ptr;

	end_area = area + sizeof (area);

	/*
	 *	Converte o valor para a base dada
	 */
	for (cp = end_area; cp > area; /* abaixo */)
	{
		*--cp = "0123456789ABCDEF"[n%base];

		if ((n /= base) == 0)
			break;
	}

	/*
	 *	Coloca os brancos/zeros no início
	 */
	if (w != 0)
	{
		if   (w < 0)
			{ c = '0'; w = -w; }
		elif (base == 10)
			c = ' ';
		else
			c = '0';

		i = w - (end_area - cp);

		if (sinal)
			i--;

		for (/* acima */; i > 0; i--)
		{
			if (size > 0)
				{ *str++ = c; size--; }
		}
	}

	/*
	 *	Põe o sinal
	 */
	if (sinal)
	{
		if (size > 0)
			{ *str++ = '-'; size--; }
	}

	/*
	 *	Coloca os dígitos
	 */
	while (cp < end_area)
	{
		if (size > 0)
			{ *str++ = *cp++; size--; }
	}

	/*
	 *	Atualiza os novos valores
	 */
	*str_ptr = str; *size_ptr = size;

}	/* end printn */

/*
 ****************************************************************
 *	Escreve um caracter na console ou em uma CLIST		*
 ****************************************************************
 */
void
putchar (int c)
{
	TTY		*tp;

	if ((tp = kernel_pty_tp) != NOTTY && tp->t_state & ISOPEN)
	{
		SPINLOCK (&tp->t_outq.c_lock);

		cput (c, &tp->t_outq);

		if (c == '\n')
			cput ('\r', &tp->t_outq);

		SPINFREE (&tp->t_outq.c_lock);

		(*tp->t_oproc) (tp, 0);
	}
	else
	{
#ifdef	WRITEs_CLEARS_SCREEN_SAVER
		screen_saver_off ();
#endif	WRITEs_CLEARS_SCREEN_SAVER

		writechar (c);

		if (c == '\n')
			writechar ('\r');
	}

	/*
	 *	Guarda a mensagem, também em DMESG
	 */
	if (dmesg_ptr >= dmesg_end)
		dmesg_ptr = dmesg_area;

	*dmesg_ptr++ = c;

}	/* end putchar */
