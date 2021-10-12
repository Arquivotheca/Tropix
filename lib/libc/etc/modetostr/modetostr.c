/*
 ****************************************************************
 *								*
 *			modetostr.c				*
 *								*
 *	Converte o modo de um arquivo em uma cadeia		*
 *								*
 *	Versão	1.1.0, de 19.06.87				*
 *		4.2.0, de 15.04.02				*
 *								*
 *	Módulo:	modetostr					*
 *		libc/etc					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include	<sys/types.h>

#include	<stdlib.h>
#include	<stat.h>

/*
 ****************************************************************
 *	Modos de proteção					*
 ****************************************************************
 */
typedef struct
{
	long	m_mode;		/* Indicador do modo */
	int	m_char;		/* Caracter de representação */

}	MODE;

static const MODE modevec0[] =
{
	S_7,			'7',
	S_6,			'6',
	S_5,			'5',
	S_4,			'4',
	S_3,			'3',
	S_2,			'2',
	S_1,			'1',
	S_IMETX,		'm',

	S_ISVTX,		't',
	S_ISUID,		's',
	S_ISGID,		'g',

	0
};

static const MODE modevec1[] =
{
	S_IREAD,		'r',
	S_IWRITE,		'w',
	S_IEXEC,		'x',
	S_IREAD  >> 3,		'r',
	S_IWRITE >> 3,		'w',
	S_IEXEC  >> 3,		'x',
	S_IREAD  >> 6,		'r',
	S_IWRITE >> 6,		'w',
	S_IEXEC  >> 6,		'x',

	0
};

/*
 ****************************************************************
 *	Variaveis Globais					*
 ****************************************************************
 */
static char	modestr[24];	/* Cadeia cujo ponteiro é devolvido */

/*
 ****************************************************************
 *	Converte o modo dado para a cadeia correspondente	*
 ****************************************************************
 */
char *
modetostr (register long mode)
{
	register char		*cp = modestr;
	register const MODE	*mp;

	/*
	 *	Tipo do arquivo
	 */
	switch (mode & S_IFMT)
	{
	    case S_IFIFO:	/* Fifo */
		*cp++ = 'F';
		break;

	    case S_IFCHR:	/* Caracter Especial */
		*cp++ = 'C';
		break;

	    case S_IFDIR:	/* Diretorio */
		*cp++ = 'D';
		break;

	    case S_IFBLK:	/* Bloco especial */
		*cp++ = 'B';
		break;

	    case S_IFREG:	/* Arquivo regular */
		*cp++ = 'R';
		break;

	    case S_IFLNK:	/* Elo simbólico */
		*cp++ = 'L';
		break;

	    default:
		*cp++ = '?';	/* Tipo invalido */
		break;

	}	/* end switch (mode & S_IFMT) */

	*cp++ = ' ';

	/*
	 *	Põe os indicadores (parte alta)
	 */
	for (mp = modevec0; mp->m_mode; mp++)
		*cp++ = (mp->m_mode & mode) ? mp->m_char : '-';

	*cp++ = ' ';

	/*
	 *	Põe os indicadores (parte baixa)
	 */
	for (mp = modevec1; mp->m_mode; mp++)
		*cp++ = (mp->m_mode & mode) ? mp->m_char : '-';

	return (modestr);

}	/* end mode */
