/*
 ****************************************************************
 *								*
 *			<ctype.h>				*
 *								*
 *	Classificação dos caracteres				*
 *								*
 *	Versão	1.0.0, de 13.11.86				*
 *		3.0.0, de 03.01.95				*
 *								*
 *	Módulo: ctype.h						*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1994 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Variáveis externas **************************************
 */
extern const char	_ctype[];

/*
 ******	Categorias de caracteres ********************************
 */
#define	_U	0x01		/* Letra maiúscula */
#define _L	0x02		/* Letra minúscula */
#define	_D	0x04		/* Dígito decimal */
#define _X	0x08		/* Dígito hexadecimal */
#define	_S	0x10		/* Espaço */
#define _C	0x20		/* Controle */

/*
 ******	Macros de classificação *********************************
 */
#define	isalpha(c)	(_ctype[c] & (_U|_L))
#define	isupper(c)	(_ctype[c] & _U)
#define	islower(c)	(_ctype[c] & _L)
#define	isdigit(c)	(_ctype[c] & _D)
#define	isxdigit(c)	(_ctype[c] & _X)
#define	isalnum(c)	(_ctype[c] & (_U|_L|_D))
#define	isspace(c)	(_ctype[c] & _S)
#define	ispunct(c)	((_ctype[c] & (_C|_L|_U|_D)) == 0)
#define	iscntrl(c)	(_ctype[c] & _C)
#define	isprint(c)	((_ctype[c] & _C) == 0)
#define	isgraph(c)	((_ctype[c] & (_C|_S)) == 0)

#define	isascii(c)	((unsigned)(c) < 128)
#define	isiso(c)	((unsigned)(c) < 256)

/*
 ******	Macros de conversão *************************************
 */
#define	_tolower(c)	((c) |  0x20)
#define	_toupper(c)	((c) & ~0x20)
#define	tolower(c)	(isupper (c) ? ((c) |  0x20) : (c))
#define	toupper(c)	(islower (c) ? ((c) & ~0x20) : (c))
#define	toascii(c)	((c) & 0x7F)
#define	toiso(c)	((c) & 0xFF)
