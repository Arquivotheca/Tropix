/*
 ****************************************************************
 *								*
 *			key.c					*
 *								*
 *	Pequeno "driver" para o teclado				*
 *								*
 *	Versão	3.0.0, de 20.06.94				*
 *		3.0.1, de 05.10.96				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1994 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
extern const char normal_key_table[], 	/* Tabelas de conversão */
		  shifted_key_table[],
		  control_key_table[];

#define	C(c)	(c & ~0x40)		/* Transforma em CTL */

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
 ******	Portas e definições do teclado **************************
 */
#define	KEY_STATUS	0x64	/* Registro de estado */
#define	KEY_DATA	0x60	/* Registro de dados */

#define	KEY_AVAIL	0x01	/* Caracter disponível no teclado */
#define	KEY_BUSY	0x02	/* Reg. de entrada não pronto */
#define	KEY_SETLEDs	0xED	/* Prepara escrita nos LEDs */
#define KEY_ACK		0xFA	/* Acknowledge */
#define KEY_RESEND	0xFE	/* Tente novamente */

/*
 ******	Protótipos de funções ***********************************
 */
void		key_write_led (void);
void		key_send_cmd (int);
void		key_ready_wait (void);

/*
 ****************************************************************
 *	Verifica se tem um caractere disponível do teclado	*
 ****************************************************************
 */
int
haschar (void)
{
	return (read_port (KEY_STATUS) & KEY_AVAIL);

}	/* end haschar */

/*
 ****************************************************************
 *	Obtém um caractere do teclado				*
 ****************************************************************
 */
int
getchar (void)
{
	int		c;

	/*
	 *	Leu a tela presente
	 */
	video_line = 0;

	/*
	 *	Obtém o número da tecla apertada ou solta
	 */
    again:
	while (!(read_port (KEY_STATUS) & KEY_AVAIL))
		/* vazio */;

	c = read_port (KEY_DATA);

	/*
	 *	Verifica se a tecla foi apertada ou desapertada
	 */
	if (c < 128)
	{
		/*
		 *	A tecla foi apertada
		 */
		switch (c)
		{
		    case 29:		/* Control */
			key_state |= CONTROL;
			break;

		    case 56:		/* Alt */
			key_state |= ALT;
			break;

		    case 42:		/* Shift esquerdo */
		    case 54:		/* Shift direito */
			key_state |= SHIFT;
			key_write_led ();
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
			if (c >= 112)	/* A tabela tem 112 entradas */
				break;

			switch (key_state)
			{
			    case 0:			/* Nada apertado */
			    case ALT:
				c = normal_key_table[c];
				break;

			    case SHIFT:
			    case CAP_LOCK:
			    case CAP_LOCK|SHIFT:
				c = shifted_key_table[c];
				break;

			    case CONTROL:
				c = control_key_table[c];
				break;

			    case ALT|CONTROL:
				if (c == 83)		/* Del */
				{
					*(ushort *)0x472 = 0x1234;

					for (c = 100; c > 0; c--)
					{
						while (read_port (KEY_STATUS) & KEY_BUSY)
							/* vazio */;

						write_port (0xFE, KEY_STATUS);
					}

					reset_cpu ();
				}

				c = control_key_table[c];
				break;

			    default:			/* Ignora */
				c = 0;
				break;

			}	/* end switch */

			if (c != 0)	/* Entrada nula é ignorada */
				return (c);

			break;

		}	/* end switch (c) */
	}
	else	/* c >= 128 */
	{
		/*
		 *	A tecla foi desapertada
		 */
		switch (c)
		{
		    case 157:		/* Control */
			key_state &= ~CONTROL;
			break;

		    case 184:		/* Alt */
			key_state &= ~ALT;
			break;

		    case 170:		/* Shift esquerdo */
		    case 182:		/* Shift direito */
			key_state &= ~SHIFT;
			key_write_led ();
			break;

	       /*** case 186:		/* Caps lock	***/
	       /*** default:				***/
		   /***	break;			***/

		}	/* end switch (c) */

	}	/* end if (c < 128) */

	goto again;

}	/* end getchar */

/*
 ****************************************************************
 *	Atualiza os LEDs do teclado				*
 ****************************************************************
 */
void
key_write_led (void)
{
	int		value = 0;

	key_send_cmd (KEY_SETLEDs);

	if (key_state & SCROLL_LOCK)		/* Scroll */
		value |= 0x01;

	if (key_state & NUM_LOCK)		/* Num lock */
		value |= 0x02;

	if (key_state & (SHIFT|CAP_LOCK))	/* Caps */
		value |= 0x04;

	key_send_cmd (value);

}	/* end key_write_led */

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

	for (retry = 5; retry > 0; retry--)
	{
		key_ready_wait ();

		write_port (cmd, KEY_DATA);

		for (i = 100000; i > 0; i--)
		{
			if (read_port (KEY_STATUS) & KEY_AVAIL)
			{
				DELAY (10);

				if ((val = read_port (KEY_DATA)) == KEY_ACK)
					return;

				if (val == KEY_RESEND)
					break;
			}
		}
	}

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
		if ((read_port (KEY_STATUS) & KEY_BUSY) == 0)
			break;

		DELAY (10);
	}

}	/* end key_ready_wait */

/*
 ****************************************************************
 *	Tabela de caracteres minúsculos				*
 ****************************************************************
 */
const char	normal_key_table[] =
{
	 0,	'\e',	'1',	'2',	/*   0 -   3 */
	'3',	'4',	'5',	'6',	/*   4 -   7 */
	'7',	'8',	'9',	'0',	/*   8 -  11 */
	'-',	'=',	0x7F,	'\t',	/*  12 -  15 */
	'q',	'w',	'e',	'r',	/*  16 -  19 */
	't',	'y',	'u',	'i',	/*  20 -  23 */
	'o',	'p',	'[',	']',	/*  24 -  27 */
	'\r',	 0,	'a',	's',	/*  28 -  31 */
	'd',	'f',	'g',	'h',	/*  32 -  35 */
	'j',	'k',	'l',	';',	/*  36 -  39 */
	'\'',	'`',	 0,	'\\',	/*  40 -  43 */
	'z',	'x',	'c',	'v',	/*  44 -  47 */
	'b',	'n',	'm',	',',	/*  48 -  51 */
	'.',	'/',	 0,	 0,	/*  52 -  55 */
	 0,	' ',	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	 0,	/*  68 -  71 */
	 0,	 0,	 0,	 0,	/*  72 -  75 */
	 0,	 0,	 0,	 0,	/*  76 -  79 */
	 0,	 0,	 0,	 0x7F,	/*  80 -  83 */
	 0,	 0,	'<',	 0,	/*  84 -  87 */
	 0,	 0,	 0,	 0,	/*  88 -  91 */
	 0,	 0,	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0	/* 108 - 111 */
};

/*
 ****************************************************************
 *	Tabela de caracteres maiúsculos				*
 ****************************************************************
 */
const char	shifted_key_table[] =
{
	 0,	'\e',	'!',	'@',	/*   0 -   3 */
	'#',	'$',	'%',	'^',	/*   4 -   7 */
	'&',	'*',	'(',	')',	/*   8 -  11 */
	'_',	'+',	0x7F,	'\t',	/*  12 -  15 */
	'Q',	'W',	'E',	'R',	/*  16 -  19 */
	'T',	'Y',	'U',	'I',	/*  20 -  23 */
	'O',	'P',	'{',	'}',	/*  24 -  27 */
	'\n',	 0,	'A',	'S',	/*  28 -  31 */
	'D',	'F',	'G',	'H',	/*  32 -  35 */
	'J',	'K',	'L',	':',	/*  36 -  39 */
	'"',	'~',	 0,	'|',	/*  40 -  43 */
	'Z',	'X',	'C',	'V',	/*  44 -  47 */
	'B',	'N',	'M',	'<',	/*  48 -  51 */
	'>',	'?',	 0,	 0,	/*  52 -  55 */
	 0,	' ',	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	 0,	/*  68 -  71 */
	 0,	 0,	 0,	 0,	/*  72 -  75 */
	 0,	 0,	 0,	 0,	/*  76 -  79 */
	 0,	 0,	 0,	 0,	/*  80 -  83 */
	 0,	 0,	'>',	 0,	/*  84 -  87 */
	 0,	 0,	 0,	 0,	/*  88 -  91 */
	 0,	 0,	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0	/* 108 - 111 */
};

/*
 ****************************************************************
 *	Tabela de caracteres de controle			*
 ****************************************************************
 */
const char	control_key_table[] =
{
	 0,	'\e',	'1',	'2',	/*   0 -   3 */
	'3',	'4',	'5',	'6',	/*   4 -   7 */
	'7',	'8',	'9',	'0',	/*   8 -  11 */
	'-',	'=',	0x7F,	'\t',	/*  12 -  15 */
      C('Q'), C('W'), C('E'), C('R'),	/*  16 -  19 */
      C('T'), C('Y'), C('U'), C('I'),	/*  20 -  23 */
      C('O'), C('P'),	'[',	']',	/*  24 -  27 */
	'\n',	 0,   C('A'), C('S'),	/*  28 -  31 */
      C('D'), C('F'), C('G'), C('H'),	/*  32 -  35 */
      C('J'), C('K'), C('L'),	':',	/*  36 -  39 */
	'\'',	'`',	 0,	'\\',	/*  40 -  43 */
      C('Z'), C('X'), C('C'), C('V'),	/*  44 -  47 */
      C('B'), C('N'), C('M'),	',',	/*  48 -  51 */
	'.',	'/',	 0,	 0,	/*  52 -  55 */
	 0,	' ',	 0,	 0,	/*  56 -  59 */
	 0,	 0,	 0,	 0,	/*  60 -  63 */
	 0,	 0,	 0,	 0,	/*  64 -  67 */
	 0,	 0,	 0,	 0,	/*  68 -  71 */
	 0,	 0,	 0,	 0,	/*  72 -  75 */
	 0,	 0,	 0,	 0,	/*  76 -  79 */
	 0,	 0,	 0,	 0,	/*  80 -  83 */
	 0,	 0,	'<',	 0,	/*  84 -  87 */
	 0,	 0,	 0,	 0,	/*  88 -  91 */
	 0,	 0,	 0,	 0,	/*  92 -  95 */
	 0,	 0,	 0,	 0,	/*  96 -  99 */
	 0,	 0,	 0,	 0,	/* 100 - 103 */
	 0,	 0,	 0,	 0,	/* 104 - 107 */
	 0,	 0,	 0,	 0	/* 108 - 111 */
};
