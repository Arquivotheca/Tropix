/*
 ****************************************************************
 *								*
 *			reduce3.c				*
 *								*
 *	Comprime um arquivo segundo o algoritmo LZW		*
 *								*
 *	Versão	2.3.0, de 27.08.90				*
 *		2.3.3, de 14.12.90				*
 *								*
 *	Módulo: reduce						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "reduce.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
#define	PRINT_CODES

extern int	exit_code;	/* Código de retorno */

extern int	rflag;		/* Não gera/confere CRCs */
extern int	uflag;		/* Uncompress */
extern int	fflag;		/* Force */
extern int	vflag;		/* Verbose */
extern int	tflag;		/* Tabela */
extern int	dflag;		/* Debug */
extern int	cflag;		/* Imprime os códigos */

extern int	code_area[NC];	/* Cabem sempre NC códigos até 16 bits */

extern long	in_cnt, out_cnt; /* Total de caracteres de entrada/saída */

extern char	*in_nm, *out_nm; /* Nomes dos arquivos */

extern int	CODE_SOURCE;	/* Fonte de novos códigos */

/*
 ****** Parâmetros de compressão ********************************
 */
extern int	bit_sz;		/* Tamanho atual */

/*
 ****************************************************************
 *	Escreve os 8 códigos					*
 ****************************************************************
 */
void
flushcode (void)
{
	register int	code0, code1;
	register int	*wp = code_area;
	register char	*cp;
	register FILE	*fp = stdout;

#ifdef	PRINT_CODES
	if (cflag)
	{
		fprintf
		(	stderr,
			"(%6d, %2d) %6d  %6d  %6d  %6d  %6d  %6d  %6d  %6d\n",
			CODE_SOURCE - 1,  bit_sz,
			wp[0], wp[1], wp[2], wp[3], wp[4], wp[5], wp[6], wp[7]
		);
	}
#endif	PRINT_CODES

	switch (bit_sz)
	{
	    case 9:
		code0 = *wp++;	putc (		     (code0 >> 1), fp);
		code1 = *wp++; 	putc ((code0 << 7) | (code1 >> 2), fp);
		code0 = *wp++; 	putc ((code1 << 6) | (code0 >> 3), fp);
		code1 = *wp++; 	putc ((code0 << 5) | (code1 >> 4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >> 5), fp);
		code1 = *wp++; 	putc ((code0 << 3) | (code1 >> 6), fp);
		code0 = *wp++; 	putc ((code1 << 2) | (code0 >> 7), fp);
		code1 = *wp++; 	putc ((code0 << 1) | (code1 >> 8), fp);
				putc ((code1),			   fp);

		break;

	    case 10:
		code0 = *wp++;	putc (		     (code0 >> 2), fp);
		code1 = *wp++; 	putc ((code0 << 6) | (code1 >> 4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >> 6), fp);
		code1 = *wp++; 	putc ((code0 << 2) | (code1 >> 8), fp);
				putc ((code1),			   fp);
		code0 = *wp++;	putc (		     (code0 >> 2), fp);
		code1 = *wp++; 	putc ((code0 << 6) | (code1 >> 4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >> 6), fp);
		code1 = *wp++; 	putc ((code0 << 2) | (code1 >> 8), fp);
				putc ((code1),			   fp);

		break;

	    case 11:
		code0 = *wp++;	putc (		     (code0 >>  3), fp);
		code1 = *wp++; 	putc ((code0 << 5) | (code1 >>  6), fp);
		code0 = *wp++; 	putc ((code1 << 2) | (code0 >>  9), fp);
				putc (		     (code0 >>  1), fp);
		code1 = *wp++; 	putc ((code0 << 7) | (code1 >>  4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >>  7), fp);
		code1 = *wp++; 	putc ((code0 << 1) | (code1 >> 10), fp);
				putc (		     (code1 >>  2), fp);
		code0 = *wp++; 	putc ((code1 << 6) | (code0 >>  5), fp);
		code1 = *wp++; 	putc ((code0 << 3) | (code1 >>  8), fp);
				putc ((code1),			    fp);

		break;

	    case 12:
		code0 = *wp++;	putc (		     (code0 >> 4), fp);
		code1 = *wp++; 	putc ((code0 << 4) | (code1 >> 8), fp);
				putc ((code1),			   fp);
		code0 = *wp++;	putc (		     (code0 >> 4), fp);
		code1 = *wp++; 	putc ((code0 << 4) | (code1 >> 8), fp);
				putc ((code1),			   fp);
		code0 = *wp++;	putc (		     (code0 >> 4), fp);
		code1 = *wp++; 	putc ((code0 << 4) | (code1 >> 8), fp);
				putc ((code1),			   fp);
		code0 = *wp++;	putc (		     (code0 >> 4), fp);
		code1 = *wp++; 	putc ((code0 << 4) | (code1 >> 8), fp);
				putc ((code1),			   fp);

		break;

	    case 13:
		code0 = *wp++;	putc (		     (code0 >>  5), fp);
		code1 = *wp++; 	putc ((code0 << 3) | (code1 >> 10), fp);
				putc (		     (code1 >>  2), fp);
		code0 = *wp++; 	putc ((code1 << 6) | (code0 >>  7), fp);
		code1 = *wp++; 	putc ((code0 << 1) | (code1 >> 12), fp);
				putc (		     (code1 >>  4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >>  9), fp);
				putc (		     (code0 >>  1), fp);
		code1 = *wp++; 	putc ((code0 << 7) | (code1 >>  6), fp);
		code0 = *wp++; 	putc ((code1 << 2) | (code0 >> 11), fp);
				putc (		     (code0 >>  3), fp);
		code1 = *wp++; 	putc ((code0 << 5) | (code1 >>  8), fp);
				putc ((code1),			    fp);

		break;

	    case 14:
		code0 = *wp++;	putc (		     (code0 >>  6), fp);
		code1 = *wp++; 	putc ((code0 << 2) | (code1 >> 12), fp);
				putc (		     (code1 >>  4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >> 10), fp);
				putc (		     (code0 >>  2), fp);
		code1 = *wp++; 	putc ((code0 << 6) | (code1 >>  8), fp);
				putc ((code1),			    fp);
		code0 = *wp++;	putc (		     (code0 >>  6), fp);
		code1 = *wp++; 	putc ((code0 << 2) | (code1 >> 12), fp);
				putc (		     (code1 >>  4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >> 10), fp);
				putc (		     (code0 >>  2), fp);
		code1 = *wp++; 	putc ((code0 << 6) | (code1 >>  8), fp);
				putc ((code1),			    fp);

		break;

	    case 15:
		code0 = *wp++;	putc (		     (code0 >>  7), fp);
		code1 = *wp++; 	putc ((code0 << 1) | (code1 >> 14), fp);
				putc (		     (code1 >>  6), fp);
		code0 = *wp++; 	putc ((code1 << 2) | (code0 >> 13), fp);
				putc (		     (code0 >>  5), fp);
		code1 = *wp++; 	putc ((code0 << 3) | (code1 >> 12), fp);
				putc (		     (code1 >>  4), fp);
		code0 = *wp++; 	putc ((code1 << 4) | (code0 >> 11), fp);
				putc (		     (code0 >>  3), fp);
		code1 = *wp++; 	putc ((code0 << 5) | (code1 >> 10), fp);
				putc (		     (code1 >>  2), fp);
		code0 = *wp++; 	putc ((code1 << 6) | (code0 >>  9), fp);
				putc (		     (code0 >>  1), fp);
		code1 = *wp++; 	putc ((code0 << 7) | (code1 >>  8), fp);
				putc ((code1),			    fp);

		break;

	    case 16:
		cp = (char *)wp;

		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);

		break;

	    default:
		fprintf (stderr, "%s: No. de bits inválido: %d\n", pgname, bit_sz);
		exit (1);

	}	/* end switch */

	out_cnt += bit_sz;

}	/* end flushcode */

/*
 ****************************************************************
 *	Le os 8 códigos						*
 ****************************************************************
 */
int
fillcode (void)
{
	register int	char0, char1, char2, m;
	register int	*wp = code_area;
	register char	*cp;
	register FILE	*fp = stdin;

	switch (bit_sz)
	{
	    case 9:
		m = 0x1FF;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 1) & m | (char1 >> 7);
		char0 = getc (fp); *wp++ = (char1 << 2) & m | (char0 >> 6);
		char1 = getc (fp); *wp++ = (char0 << 3) & m | (char1 >> 5);
		char0 = getc (fp); *wp++ = (char1 << 4) & m | (char0 >> 4);
		char1 = getc (fp); *wp++ = (char0 << 5) & m | (char1 >> 3);
		char0 = getc (fp); *wp++ = (char1 << 6) & m | (char0 >> 2);
		char1 = getc (fp); *wp++ = (char0 << 7) & m | (char1 >> 1);
		char0 = getc (fp); *wp++ = (char1 << 8) & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 10:
		m = 0x3FF;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 2) & m | (char1 >> 6);
		char0 = getc (fp); *wp++ = (char1 << 4) & m | (char0 >> 4);
		char1 = getc (fp); *wp++ = (char0 << 6) & m | (char1 >> 2);
		char0 = getc (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 2) & m | (char1 >> 6);
		char0 = getc (fp); *wp++ = (char1 << 4) & m | (char0 >> 4);
		char1 = getc (fp); *wp++ = (char0 << 6) & m | (char1 >> 2);
		char0 = getc (fp); *wp++ = (char1 << 8) & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 11:
		m = 0x7FF;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 3)  & m | (char1 >> 5);
		char0 = getc (fp); *wp++ = (char1 << 6)  & m | (char0 >> 2);
		char1 = getc (fp);
		char2 = getc (fp);
		   *wp++ = ((char0 << 9) | (char1 << 1)) & m | (char2 >> 7);
		char0 = getc (fp); *wp++ = (char2 << 4)  & m | (char0 >> 4);
		char1 = getc (fp); *wp++ = (char0 << 7)  & m | (char1 >> 1);
		char0 = getc (fp);
		char2 = getc (fp);
		  *wp++ = ((char1 << 10) | (char0 << 2)) & m | (char2 >> 6);
		char0 = getc (fp); *wp++ = (char2 << 5)  & m | (char0 >> 3);
		char1 = getc (fp); *wp++ = (char0 << 8)  & m | char1;

		if (char1 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 12:
		m = 0xFFF;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = getc (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = getc (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = getc (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = getc (fp);
		char1 = getc (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = getc (fp); *wp++ = (char1 << 8) & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 13:
		m = 0x1FFF;
		char0 = getc (fp);
		char1 = getc (fp);  *wp++ = (char0 << 5)  & m | (char1 >> 3);
		char2 = getc (fp);
		char0 = getc (fp);
		   *wp++ = ((char1 << 10) | (char2 << 2)) & m | (char0 >> 6);
		char1 = getc (fp);  *wp++ = (char0 << 7)  & m | (char1 >> 1);
		char2 = getc (fp);
		char0 = getc (fp);
		   *wp++ = ((char1 << 12) | (char2 << 4)) & m | (char0 >> 4);
		char1 = getc (fp);
		char2 = getc (fp);
		   *wp++ = ((char0 <<  9) | (char1 << 1)) & m | (char2 >> 7);
		char0 = getc (fp);  *wp++ = (char2 << 6)  & m | (char0 >> 2);
		char1 = getc (fp);
		char2 = getc (fp);
		   *wp++ = ((char0 << 11) | (char1 << 3)) & m | (char2 >> 5);
		char0 = getc (fp);  *wp++ = (char2 << 8)  & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 14:
		m = 0x3FFF;
		char0 = getc (fp);
		char1 = getc (fp);  *wp++ = (char0 << 6)  & m | (char1 >> 2);
		char2 = getc (fp);
		char0 = getc (fp);
		   *wp++ = ((char1 << 12) | (char2 << 4)) & m | (char0 >> 4);
		char1 = getc (fp);
		char2 = getc (fp);
		   *wp++ = ((char0 << 10) | (char1 << 2)) & m | (char2 >> 6);
		char0 = getc (fp);  *wp++ = (char2 << 8)  & m |  char0;
		char0 = getc (fp);
		char1 = getc (fp);  *wp++ = (char0 << 6)  & m | (char1 >> 2);
		char2 = getc (fp);
		char0 = getc (fp);
		   *wp++ = ((char1 << 12) | (char2 << 4)) & m | (char0 >> 4);
		char1 = getc (fp);
		char2 = getc (fp);
		   *wp++ = ((char0 << 10) | (char1 << 2)) & m | (char2 >> 6);
		char0 = getc (fp);  *wp++ = (char2 << 8)  & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 15:
		m = 0x7FFF;
		char0 = getc (fp);
		char1 = getc (fp);  *wp++ = (char0 << 7)  & m | (char1 >> 1);
		char2 = getc (fp);
		char0 = getc (fp);
		   *wp++ = ((char1 << 14) | (char2 << 6)) & m | (char0 >> 2);
		char1 = getc (fp);
		char2 = getc (fp);
		   *wp++ = ((char0 << 13) | (char1 << 5)) & m | (char2 >> 3);
		char0 = getc (fp);
		char1 = getc (fp);
		   *wp++ = ((char2 << 12) | (char0 << 4)) & m | (char1 >> 4);
		char2 = getc (fp);
		char0 = getc (fp);
		   *wp++ = ((char1 << 11) | (char2 << 3)) & m | (char0 >> 5);
		char1 = getc (fp);
		char2 = getc (fp);
		   *wp++ = ((char0 << 10) | (char1 << 2)) & m | (char2 >> 6);
		char0 = getc (fp);
		char1 = getc (fp);
		   *wp++ = ((char2 <<  9) | (char0 << 1)) & m | (char1 >> 7);
		char0 = getc (fp);  *wp++ = (char1 << 8)  & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 16:
		cp = (char *)wp;

		cp += 2; *cp++ = getc (fp); *cp++ = getc (fp);
		cp += 2; *cp++ = getc (fp); *cp++ = getc (fp);
		cp += 2; *cp++ = getc (fp); *cp++ = getc (fp);
		cp += 2; *cp++ = getc (fp); *cp++ = getc (fp);
		cp += 2; *cp++ = getc (fp); *cp++ = getc (fp);
		cp += 2; *cp++ = getc (fp); *cp++ = getc (fp);
		cp += 2; *cp++ = getc (fp); *cp++ = getc (fp);
		cp += 2; *cp++ = getc (fp); *cp++ = char0 = getc (fp);

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    default:
		fprintf (stderr, "%s: No. de bits inválido: %d\n", pgname, bit_sz);
		exit (1);

	}	/* end switch */

	in_cnt += bit_sz;

#ifdef	PRINT_CODES
	if (cflag)
	{
		wp = code_area;
		fprintf
		(	stderr,
			"(%6d, %2d) %6d  %6d  %6d  %6d  %6d  %6d  %6d  %6d\n",
			CODE_SOURCE + NC, bit_sz,
			wp[0], wp[1], wp[2], wp[3], wp[4], wp[5], wp[6], wp[7]
		);
	}
#endif	PRINT_CODES

	return (NC);

}	/* end fillcode */
