/*
 ****************************************************************
 *								*
 *			key.c					*
 *								*
 *	Pequeno "driver" para o teclado				*
 *								*
 *	Versão	3.0.0, de 20.06.94				*
 *		4.5.0, de 17.06.03				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2003 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/scb.h"

#include "../h/kbd.h"

#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
#define CON_PL		1		/* Prioridade de interrupção */
#define splcon		spl1		/* Função de prioridade de interrupção */

#define KEY_TABLE_SZ	128		/* Apertando: 0-127, soltando: 128-255 */

#define	C(c)		(c & ~0x40)	/* Transforma em CTL */

/*
 ******	Tabelas de conversão ************************************
 */
const char		us_normal_key_table[KEY_TABLE_SZ],
			abnt_normal_key_table[KEY_TABLE_SZ],
			us_shifted_key_table[KEY_TABLE_SZ],
			abnt_shifted_key_table[KEY_TABLE_SZ],
			control_key_table[KEY_TABLE_SZ],
			function_key_table[KEY_TABLE_SZ][8];

const char		*normal_key_table  = us_normal_key_table,
			*shifted_key_table = us_shifted_key_table;

/*
 ******	Definições relativas ao estado **************************
 */
#define	SHIFT		0x01	/* "Shift" está apertado */
#define	CAP_LOCK	0x02	/* "Caplock" foi apertado */
#define	CONTROL		0x04	/* "Control" está apertado */
#define	ALT		0x08	/* "Alt" está apertado */
#define	NUM_LOCK	0x10	/* "Numlock" foi apertado */
#define	SCROLL_LOCK	0x20	/* "Scroll" foi apertado */

entry int	key_state;	/* Estado atual */

/*
 ******	Protótipos de funções ***********************************
 */
void		key_write_led (void);
void		key_set_leds (int);
void		key_send_cmd (int);
void		key_ready_wait (void);

/*
 ****************************************************************
 *	Inicializa as tabelas do teclado			*
 ****************************************************************
 */
void
key_init_table (void)
{
	if (scb.y_keyboard == 0)
	{
		normal_key_table  = us_normal_key_table;
		shifted_key_table = us_shifted_key_table;
	}
	else
	{
		normal_key_table  = abnt_normal_key_table;
		shifted_key_table = abnt_shifted_key_table;
	}

}	/* end key_init_table */

/*
 ****************************************************************
 *	Esvazia o FIFO de caracteres				*
 ****************************************************************
 */
void
flushchar (void)
{
	while (read_port (KBD_STATUS_PORT) & KBDS_KBD_BUFFER_FULL)
		read_port (KBD_DATA_PORT);

}	/* end flushchar */

/*
 ****************************************************************
 *	Verifica se tem um caractere disponível do teclado	*
 ****************************************************************
 */
int
haschar (void)
{
	return (read_port (KBD_STATUS_PORT) & KBDS_KBD_BUFFER_FULL);

}	/* end haschar */

/*
 ****************************************************************
 *	Obtém um caractere do teclado (por status)		*
 ****************************************************************
 */
int
getchar (void)
{
	int		c;

	/*
	 *	Obtém o número da tecla apertada ou solta
	 */
	for (EVER)
	{
		while (!(read_port (KBD_STATUS_PORT) & KBDS_KBD_BUFFER_FULL))
			/* vazio */;

		video_line = 0;      

		c = read_port (KBD_DATA_PORT);

		if ((c = key_code_to_char (c)) > 0)
			return (c);

	}	/* end for (EVER) */

}	/* end getchar */

/*
 ****************************************************************
 *	Converte um código lido do teclado em caracter ISO	*
 ****************************************************************
 */
int
key_code_to_char (int code)
{
	int		c = -1;
	static int	alt_char, alt_base, alt_digit;

	/*
	 *	Retorna -1 se é para ignorar o código
	 *	(possivelmente alterou o estado)
	 */

	/*
	 *	Verifica se a tecla foi apertada ou desapertada
	 */
	if (code < KEY_TABLE_SZ)		/* Apertada */
	{
		switch (code)
		{
		    case 29:		/* Control */
			key_state |= CONTROL;
			break;

		    case 56:		/* Alt */
			if (key_state & ALT)
				break;

			key_state |= ALT;
			alt_char = 0; alt_base = 0;
			break;

		    case 42:		/* Shift esquerdo */
		    case 54:		/* Shift direito */
			key_state |= SHIFT;
#undef	WRITE_LED_WITH_SHIFT
#ifdef	WRITE_LED_WITH_SHIFT
			key_write_led ();
#endif	WRITE_LED_WITH_SHIFT
			break;

		    case 58:		/* Caps lock */
			key_state ^= CAP_LOCK;
			key_write_led ();
			break;

		    case 69:		/* Num lock */
			key_state ^= NUM_LOCK;
			key_write_led ();
			break;

		    case 70:		/* Scroll lock */
			key_state ^= SCROLL_LOCK;
			key_write_led ();
			break;

		    default:
			switch (key_state)
			{
			    case 0:			/* Nada apertado */
				c = normal_key_table[code];
				break;

			    case SHIFT:
			    case CAP_LOCK:
			    case CAP_LOCK|SHIFT:
				c = shifted_key_table[code];
				break;

			    case CONTROL:
				if ((c = control_key_table[code]) == 0)
					c = normal_key_table[code];
				break;

			    case ALT:	/* Processa seqüências para gerar chars */
				alt_digit = normal_key_table[code];

				if (alt_base == 0)
				{
					if   (alt_digit == '0')
						alt_base = 8;
					elif (alt_digit == 'x')
						{ alt_base = 16; alt_digit = '0'; }
					else
						alt_base = 10;
				}

				if (alt_digit > '9')
					alt_digit -= 'a' - 10;
				else
					alt_digit -= '0';

				alt_char = alt_char * alt_base + alt_digit;

				c = -1;			/* Ainda não emite */
				break;

			    case ALT|CONTROL:
				if (code == 83)		/* Del */
				{
					*(ushort *)(SYS_ADDR+0x472) = 0x1234;

					for (c = 100; c > 0; c--)
					{
						while (read_port (KBD_STATUS_PORT) & KBDS_INPUT_BUFFER_FULL)
							/* vazio */;

						write_port (0xFE, KBD_STATUS_PORT);
					}

					reset_cpu ();
				}
				break;

			    default:			/* Ignora */
				c = -1;
				break;

			}	/* end switch */

			return (c);

		}	/* end switch (c) */
	}
	else	/* code >= KEY_TABLE_SZ */	/* Desapertada */
	{
		switch (code)
		{
		    case 157:		/* Control */
			key_state &= ~CONTROL;
			break;

		    case 184:		/* Alt */
			key_state &= ~ALT;

			if (alt_base != 0)
				return (alt_char & 0xFF);

			break;

		    case 170:		/* Shift esquerdo */
		    case 182:		/* Shift direito */
			key_state &= ~SHIFT;
#ifdef	WRITE_LED_WITH_SHIFT
			key_write_led ();
#endif	WRITE_LED_WITH_SHIFT
			break;

	       /*** case 186:		/* Caps lock	***/
	       /*** default:				***/
		   /***	break;			***/

		}	/* end switch (code) */

	}	/* end if (code < KEY_TABLE_SZ) */

	return (-1);

}	/* end key_code_to_char */

/*
 ****************************************************************
 *	Atualiza os LEDs do teclado				*
 ****************************************************************
 */
void
key_write_led (void)
{
	int		pl, value = 0;

#if (0)	/*******************************************************/
	pl = splcon ();
#endif	/*******************************************************/
	pl = splx (irq_pl_vec[CON_PL]);

	key_send_cmd (KBDC_SET_LEDS);

	if (key_state & SCROLL_LOCK)		/* Scroll */
		value |= 0x01;

	if (key_state & NUM_LOCK)		/* Num lock */
		value |= 0x02;

#ifdef	WRITE_LED_WITH_SHIFT
	if (key_state & (SHIFT|CAP_LOCK))	/* Caps */
		value |= 0x04;
#else
	if (key_state & CAP_LOCK)		/* Caps */
		value |= 0x04;
#endif	WRITE_LED_WITH_SHIFT

	DELAY (10);				/* É necessário ? */

	key_send_cmd (value);

	splx (pl);

}	/* end key_write_led */

/*
 ****************************************************************
 *	Atualiza os LEDs do teclado (no X-Windows)		*
 ****************************************************************
 */
void
key_set_leds (int value)
{
	int		pl;

#if (0)	/*******************************************************/
	pl = splcon ();
#endif	/*******************************************************/
	pl = splx (irq_pl_vec[CON_PL]);

	key_send_cmd (KBDC_SET_LEDS);

	key_send_cmd (value);

	splx (pl);

}	/* end key_set_leds */

/*
 ****************************************************************
 *	Envia um comando para o teclado				*
 ****************************************************************
 */
void
key_send_cmd (int cmd)
{
	int		retry, i;
	char		val;

	for (retry = 100; retry > 0; retry--)
	{
		key_ready_wait ();
DELAY (10);

		write_port (cmd, KBD_DATA_PORT);
DELAY (10);

		for (i = 100000; i > 0; i--)
		{
			if (read_port (KBD_STATUS_PORT) & KBDS_KBD_BUFFER_FULL)
			{
				DELAY (10);

				if ((val = read_port (KBD_DATA_PORT)) == KBD_ACK)
					return;

				if (val == KBD_RESEND)
					break;
			}
		}
	}

#ifdef	MSG
	printf ("key_send_cmd: TIMEOUT\n");
#endif	MSG

}	/* end key_send_cmd */

/*
 ****************************************************************
 *	Espera até que o teclado esteja pronto para comandos	*
 ****************************************************************
 */
void
key_ready_wait (void)
{
	int		i;

	for (i = 1000; i > 0; i--)
	{
		if ((read_port (KBD_STATUS_PORT) & KBDS_INPUT_BUFFER_FULL) == 0)
			break;

		DELAY (10);
	}

#ifdef	MSG
	if (i <= 0)
		printf ("key_ready_wait: TIMEOUT\n");
#endif	MSG

}	/* end key_ready_wait */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	Obtém um <^Q> ou <nl>					*
 ****************************************************************
 */
void
wait_for_ctl_Q (void)
{
	int		c;

	for (EVER)
	{
		if (!haschar ())
			continue;

		if ((c = getchar ()) == 0x11 || c == '\n' || c == '\r')
			break;
	}

	video_line = 0;

}	/* end wait_for_ctl_Q */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Tabela de caracteres minúsculos				*
 ****************************************************************
 */
const char	us_normal_key_table[] =
{
	 0,	'\e',	'1',	'2',	/*   0 -   3 */
	'3',	'4',	'5',	'6',	/*   4 -   7 */
	'7',	'8',	'9',	'0',	/*   8 -  11 */
	'-',	'=',	'\b',	'\t',	/*  12 -  15 */
	'q',	'w',	'e',	'r',	/*  16 -  19 */
	't',	'y',	'u',	'i',	/*  20 -  23 */
	'o',	'p',	'[',	']',	/*  24 -  27 */
	'\r',	 0,	'a',	's',	/*  28 -  31 */
	'd',	'f',	'g',	'h',	/*  32 -  35 */
	'j',	'k',	'l',	';',	/*  36 -  39 */
	'\'',	'`',	 0,	'\\',	/*  40 -  43 */
	'z',	'x',	'c',	'v',	/*  44 -  47 */
	'b',	'n',	'm',	',',	/*  48 -  51 */
	'.',	'/',	 0,	'*',	/*  52 -  55 */
	 0,	' ',	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	 0,	/*  68 -  71 */
	 0,	 0,	'-',	 0,	/*  72 -  75 */
	'5',	 0,	'+',	 0,	/*  76 -  79 */
	 0,	 0,	 0,	 0x7F,	/*  80 -  83 */
	 0,	 0,	'<',	 0,	/*  84 -  87 */
	 0,	 0,	 0,	 0,	/*  88 -  91 */
	 0,	 0,	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0,	/* 108 - 111 */
	 0,	 0,	 0,	 0,	/* 112 - 115 */
	 0,	 0,	 0,	 0,	/* 116 - 119 */
	 0,	 0,	 0,	 0,	/* 120 - 123 */
	 0,	 0,	 0,	 0	/* 124 - 127 */
};

/*
 ****************************************************************
 *	Tabela de caracteres minúsculos	do ABNT2		*
 ****************************************************************
 */
const char	abnt_normal_key_table[KEY_TABLE_SZ] =
{
	 0,	'\e',	'1',	'2',	/*   0 -   3 */
	'3',	'4',	'5',	'6',	/*   4 -   7 */
	'7',	'8',	'9',	'0',	/*   8 -  11 */
	'-',	'=',	'\b',	'\t',	/*  12 -  15 */
	'q',	'w',	'e',	'r',	/*  16 -  19 */
	't',	'y',	'u',	'i',	/*  20 -  23 */
	'o',	'p',	'\'',	'[',	/*  24 -  27 */
	'\r',	 0,	'a',	's',	/*  28 -  31 */
	'd',	'f',	'g',	'h',	/*  32 -  35 */
	'j',	'k',	'l',	'ç',	/*  36 -  39 */
	'~',	'`',	 0,	']',	/*  40 -  43 */
	'z',	'x',	'c',	'v',	/*  44 -  47 */
	'b',	'n',	'm',	',',	/*  48 -  51 */
	'.',	';',	 0,	'*',	/*  52 -  55 */
	 0,	' ',	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	 0,	/*  68 -  71 */
	 0,	 0,	'-',	 0,	/*  72 -  75 */
	'5',	 0,	'+',	 0,	/*  76 -  79 */
	 0,	 0,	 0,	 0x7F,	/*  80 -  83 */
	 0,	 0,	'\\',	 0,	/*  84 -  87 */
	 0,	 0,	 0,	 0,	/*  88 -  91 */
	 0,	'/',	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0,	/* 108 - 111 */
	 0,	 0,	 0,	'/',	/* 112 - 115 */
	 0,	 0,	 0,	 0,	/* 116 - 119 */
	 0,	 0,	 0,	 0,	/* 120 - 123 */
	 0,	 0,	'.',	 0	/* 124 - 127 */
};

/*
 ****************************************************************
 *	Tabela de caracteres maiúsculos				*
 ****************************************************************
 */
const char	us_shifted_key_table[] =
{
	 0,	'\e',	'!',	'@',	/*   0 -   3 */
	'#',	'$',	'%',	'^',	/*   4 -   7 */
	'&',	'*',	'(',	')',	/*   8 -  11 */
	'_',	'+',	'\b',	'\t',	/*  12 -  15 */
	'Q',	'W',	'E',	'R',	/*  16 -  19 */
	'T',	'Y',	'U',	'I',	/*  20 -  23 */
	'O',	'P',	'{',	'}',	/*  24 -  27 */
	'\n',	 0,	'A',	'S',	/*  28 -  31 */
	'D',	'F',	'G',	'H',	/*  32 -  35 */
	'J',	'K',	'L',	':',	/*  36 -  39 */
	'"',	'~',	 0,	'|',	/*  40 -  43 */
	'Z',	'X',	'C',	'V',	/*  44 -  47 */
	'B',	'N',	'M',	'<',	/*  48 -  51 */
	'>',	'?',	 0,	'*',	/*  52 -  55 */
	 0,	' ',	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	'7',	/*  68 -  71 */
	'8',	'9',	'-',	'4',	/*  72 -  75 */
	'5',	'6',	'+',	'1',	/*  76 -  79 */
	'2',	'3',	'0',	'.',	/*  80 -  83 */
	 0,	 0,	'>',	 0,	/*  84 -  87 */
	 0,	 0,	 0,	 0,	/*  88 -  91 */
	 0,	 0,	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0,	/* 108 - 111 */
	 0,	 0,	 0,	 0,	/* 112 - 115 */
	 0,	 0,	 0,	 0,	/* 116 - 119 */
	 0,	 0,	 0,	 0,	/* 120 - 123 */
	 0,	 0,	 0,	 0	/* 124 - 127 */
};

/*
 ****************************************************************
 *	Tabela de caracteres maiúsculos	do ABNT2		*
 ****************************************************************
 */
const char	abnt_shifted_key_table[KEY_TABLE_SZ] =
{
	 0,	'\e',	'!',	'@',	/*   0 -   3 */
	'#',	'$',	'%',	'¨',	/*   4 -   7 */
	'&',	'*',	'(',	')',	/*   8 -  11 */
	'_',	'+',	'\b',	'\t',	/*  12 -  15 */
	'Q',	'W',	'E',	'R',	/*  16 -  19 */
	'T',	'Y',	'U',	'I',	/*  20 -  23 */
	'O',	'P',	'`',	'{',	/*  24 -  27 */
	'\n',	 0,	'A',	'S',	/*  28 -  31 */
	'D',	'F',	'G',	'H',	/*  32 -  35 */
	'J',	'K',	'L',	'Ç',	/*  36 -  39 */
	'^',	'"',	 0,	'}',	/*  40 -  43 */
	'Z',	'X',	'C',	'V',	/*  44 -  47 */
	'B',	'N',	'M',	'<',	/*  48 -  51 */
	'>',	':',	 0,	'*',	/*  52 -  55 */
	 0,	' ',	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	'7',	/*  68 -  71 */
	'8',	'9',	'-',	'4',	/*  72 -  75 */
	'5',	'6',	'+',	'1',	/*  76 -  79 */
	'2',	'3',	'0',	',',	/*  80 -  83 */
	 0,	 0,	'|',	 0,	/*  84 -  87 */
	 0,	'|',	 0,	 0,	/*  88 -  91 */
	 0,	 0,	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0,	/* 108 - 111 */
	 0,	 0,	 0,	'?',	/* 112 - 115 */
	 0,	 0,	 0,	 0,	/* 116 - 119 */
	 0,	 0,	 0,	 0,	/* 120 - 123 */
	 0,	 0,	'.',	 0	/* 124 - 127 */
};

/*
 ****************************************************************
 *	Tabela de caracteres de contrôle			*
 ****************************************************************
 */
const char	control_key_table[] =
{
	 0,	 0,	 0,	 0,	/*   0 -   3 */
	 0,	 0,	 0,	 0,	/*   4 -   7 */
	 0,	 0,	 0,	 0,	/*   8 -  11 */
	 0,	 0,	 0,	 0,	/*  12 -  15 */
      C('Q'), C('W'), C('E'), C('R'),	/*  16 -  19 */
      C('T'), C('Y'), C('U'), C('I'),	/*  20 -  23 */
      C('O'), C('P'),	 0,	 0,	/*  24 -  27 */
	 0,	 0,   C('A'), C('S'),	/*  28 -  31 */
      C('D'), C('F'), C('G'), C('H'),	/*  32 -  35 */
      C('J'), C('K'), C('L'),	 0,	/*  36 -  39 */
	 0,	 0,	 0,	 0,	/*  40 -  43 */
      C('Z'), C('X'), C('C'), C('V'),	/*  44 -  47 */
      C('B'), C('N'), C('M'),	 0,	/*  48 -  51 */
	 0,	 0,	 0,	 0,	/*  52 -  55 */
	 0,	 0,	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	 0,	/*  68 -  71 */
	 0,	 0,	 0,	 0,	/*  72 -  75 */
	 0,	 0,	 0,	 0,	/*  76 -  79 */
	 0,	 0,	 0,	 0,	/*  80 -  83 */
	 0,	 0,	 0,	 0,	/*  84 -  87 */
	 0,	 0,	 0,	 0,	/*  88 -  91 */
	 0,	 0,	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0,	/* 108 - 111 */
	 0,	 0,	 0,	 0,	/* 112 - 115 */
	 0,	 0,	 0,	 0,	/* 116 - 119 */
	 0,	 0,	 0,	 0,	/* 120 - 123 */
	 0,	 0,	 0,	 0	/* 124 - 127 */
};

/*
 ****************************************************************
 *	Tabela de teclas multicaracteres			*
 ****************************************************************
 */
const char	function_key_table[][8] =
{
	{ '\0' },			/*   0 */
	{ '\0' },			/*   1 */
	{ '\0' },			/*   2 */
	{ '\0' },			/*   3 */
	{ '\0' },			/*   4 */
	{ '\0' },			/*   5 */
	{ '\0' },			/*   6 */
	{ '\0' },			/*   7 */
	{ '\0' },			/*   8 */
	{ '\0' },			/*   9 */
	{ '\0' },			/*  10 */
	{ '\0' },			/*  11 */
	{ '\0' },			/*  12 */
	{ '\0' },			/*  13 */
	{ '\0' },			/*  14 */
	{ '\0' },			/*  15 */
	{ '\0' },			/*  16 */
	{ '\0' },			/*  17 */
	{ '\0' },			/*  18 */
	{ '\0' },			/*  19 */
	{ '\0' },			/*  20 */
	{ '\0' },			/*  21 */
	{ '\0' },			/*  22 */
	{ '\0' },			/*  23 */
	{ '\0' },			/*  24 */
	{ '\0' },			/*  25 */
	{ '\0' },			/*  26 */
	{ '\0' },			/*  27 */
	{ '\0' },			/*  28 */
	{ '\0' },			/*  29 */
	{ '\0' },			/*  30 */
	{ '\0' },			/*  31 */
	{ '\0' },			/*  32 */
	{ '\0' },			/*  33 */
	{ '\0' },			/*  34 */
	{ '\0' },			/*  35 */
	{ '\0' },			/*  36 */
	{ '\0' },			/*  37 */
	{ '\0' },			/*  38 */
	{ '\0' },			/*  39 */
	{ '\0' },			/*  40 */
	{ '\0' },			/*  41 */
	{ '\0' },			/*  42 */
	{ '\0' },			/*  43 */
	{ '\0' },			/*  44 */
	{ '\0' },			/*  45 */
	{ '\0' },			/*  46 */
	{ '\0' },			/*  47 */
	{ '\0' },			/*  48 */
	{ '\0' },			/*  19 */
	{ '\0' },			/*  50 */
	{ '\0' },			/*  51 */
	{ '\0' },			/*  52 */
	{ '\0' },			/*  53 */
	{ '\0' },			/*  54 */
	{ '\0' },			/*  55 */
	{ '\0' },			/*  56 */
	{ '\0' },			/*  57 */
	{ '\0' },			/*  58 */
	{ '\e', '[', '1', '1', '~' },	/*  59  F1 */
	{ '\e', '[', '1', '2', '~' },	/*  60  F2 */
	{ '\e', '[', '1', '3', '~' },	/*  61  F3 */
	{ '\e', '[', '1', '4', '~' },	/*  62  F4 */
	{ '\e', '[', '1', '5', '~' },	/*  63  F5 */
	{ '\e', '[', '1', '7', '~' },	/*  64  F6 */
	{ '\e', '[', '1', '8', '~' },	/*  65  F7 */
	{ '\e', '[', '1', '9', '~' },	/*  66  F8 */
	{ '\e', '[', '2', '0', '~' },	/*  67  F9 */
	{ '\e', '[', '2', '1', '~' },	/*  68 F10 */
	{ '\0' },			/*  69 */
	{ '\0' },			/*  70 */
	{ '\e', '[', '7', '~' },	/*  71 Home */
	{ '\e', '[', 'A' },		/*  72 Arrom up */
	{ '\e', '[', '5', '~' },	/*  73 Page up */
	{ '\0' },			/*  74 */
	{ '\e', '[', 'D' },		/*  75 Arrow left */
	{ '\0' },			/*  76 */
	{ '\e', '[', 'C' },		/*  77 Arrow right */
	{ '\0' },			/*  78 */
	{ '\e', '[', '8', '~' },	/*  79 End */
	{ '\e', '[', 'B' },		/*  80 Arrow down */
	{ '\e', '[', '6', '~' },	/*  81 Page down */
	{ '\e', '[', '2', '~' },	/*  82 Insert */
	{ '\0' },			/*  83 */
	{ '\0' },			/*  84 */
	{ '\0' },			/*  85 */
	{ '\0' },			/*  86 */
	{ '\e', '[', '2', '3', '~' },	/*  87 F11 */
	{ '\e', '[', '2', '4', '~' },	/*  88 F12 */
	{ '\0' },			/*  89 */
	{ '\0' },			/*  90 */
	{ '\0' },			/*  91 */
	{ '\0' },			/*  92 */
	{ '\0' },			/*  93 */
	{ '\0' },			/*  94 */
	{ '\0' },			/*  95 */
	{ '\0' },			/*  96 */
	{ '\0' },			/*  97 */
	{ '\0' },			/*  98 */
	{ '\0' },			/*  99 */
	{ '\0' },			/* 100 */
	{ '\0' },			/* 101 */
	{ '\0' },			/* 102 */
	{ '\0' },			/* 103 */
	{ '\0' },			/* 104 */
	{ '\0' },			/* 105 */
	{ '\0' },			/* 106 */
	{ '\0' },			/* 107 */
	{ '\0' },			/* 108 */
	{ '\0' },			/* 109 */
	{ '\0' },			/* 100 */
	{ '\0' },			/* 111 */
	{ '\0' },			/* 112 */
	{ '\0' },			/* 113 */
	{ '\0' },			/* 114 */
	{ '\0' },			/* 115 */
	{ '\0' },			/* 116 */
	{ '\0' },			/* 117 */
	{ '\0' },			/* 118 */
	{ '\0' },			/* 119 */
	{ '\0' },			/* 120 */
	{ '\0' },			/* 121 */
	{ '\0' },			/* 122 */
	{ '\0' },			/* 123 */
	{ '\0' },			/* 124 */
	{ '\0' },			/* 125 */
	{ '\0' },			/* 126 */
	{ '\0' }			/* 127 */
};
