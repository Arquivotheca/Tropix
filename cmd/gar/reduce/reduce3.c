/*
 ****************************************************************
 *								*
 *			reduce3.c				*
 *								*
 *	Comprime um arquivo segundo o algoritmo LZW		*
 *								*
 *	Versão	2.3.00, de 27.08.90				*
 *		3.1.06, de 04.05.97				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "../h/common.h"
#include "../h/reduce.h"

/*
 ****************************************************************
 *	Escreve os 8 códigos					*
 ****************************************************************
 */
void
flushcode (void)
{
	int		code0, code1;
	int		*wp = code_area;
	char		*cp;
	FILE		*fp = out_fp;

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
#ifdef	ICA
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
		cp += 2; putc (*cp++, fp); putc (*cp++, fp);
#else	PC
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
		putc (cp[1], fp); putc (cp[0], fp); cp += 4;
#endif	ICA/PC

		break;

	    default:
		fprintf (stderr, "%s: No. de bits inválido: %d\n", pgname, bit_sz);
		quit (1);

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
	int		char0, char1, char2, m;
	int		*wp = code_area;
	char		*cp;
	COLFILE		*fp = &incolfile;

	switch (bit_sz)
	{
	    case 9:
		m = 0x1FF;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 1) & m | (char1 >> 7);
		char0 = GETC (fp); *wp++ = (char1 << 2) & m | (char0 >> 6);
		char1 = GETC (fp); *wp++ = (char0 << 3) & m | (char1 >> 5);
		char0 = GETC (fp); *wp++ = (char1 << 4) & m | (char0 >> 4);
		char1 = GETC (fp); *wp++ = (char0 << 5) & m | (char1 >> 3);
		char0 = GETC (fp); *wp++ = (char1 << 6) & m | (char0 >> 2);
		char1 = GETC (fp); *wp++ = (char0 << 7) & m | (char1 >> 1);
		char0 = GETC (fp); *wp++ = (char1 << 8) & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 10:
		m = 0x3FF;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 2) & m | (char1 >> 6);
		char0 = GETC (fp); *wp++ = (char1 << 4) & m | (char0 >> 4);
		char1 = GETC (fp); *wp++ = (char0 << 6) & m | (char1 >> 2);
		char0 = GETC (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 2) & m | (char1 >> 6);
		char0 = GETC (fp); *wp++ = (char1 << 4) & m | (char0 >> 4);
		char1 = GETC (fp); *wp++ = (char0 << 6) & m | (char1 >> 2);
		char0 = GETC (fp); *wp++ = (char1 << 8) & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 11:
		m = 0x7FF;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 3)  & m | (char1 >> 5);
		char0 = GETC (fp); *wp++ = (char1 << 6)  & m | (char0 >> 2);
		char1 = GETC (fp);
		char2 = GETC (fp);
		   *wp++ = ((char0 << 9) | (char1 << 1)) & m | (char2 >> 7);
		char0 = GETC (fp); *wp++ = (char2 << 4)  & m | (char0 >> 4);
		char1 = GETC (fp); *wp++ = (char0 << 7)  & m | (char1 >> 1);
		char0 = GETC (fp);
		char2 = GETC (fp);
		  *wp++ = ((char1 << 10) | (char0 << 2)) & m | (char2 >> 6);
		char0 = GETC (fp); *wp++ = (char2 << 5)  & m | (char0 >> 3);
		char1 = GETC (fp); *wp++ = (char0 << 8)  & m | char1;

		if (char1 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 12:
		m = 0xFFF;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = GETC (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = GETC (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = GETC (fp); *wp++ = (char1 << 8) & m |  char0;
		char0 = GETC (fp);
		char1 = GETC (fp); *wp++ = (char0 << 4) & m | (char1 >> 4);
		char0 = GETC (fp); *wp++ = (char1 << 8) & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 13:
		m = 0x1FFF;
		char0 = GETC (fp);
		char1 = GETC (fp);  *wp++ = (char0 << 5)  & m | (char1 >> 3);
		char2 = GETC (fp);
		char0 = GETC (fp);
		   *wp++ = ((char1 << 10) | (char2 << 2)) & m | (char0 >> 6);
		char1 = GETC (fp);  *wp++ = (char0 << 7)  & m | (char1 >> 1);
		char2 = GETC (fp);
		char0 = GETC (fp);
		   *wp++ = ((char1 << 12) | (char2 << 4)) & m | (char0 >> 4);
		char1 = GETC (fp);
		char2 = GETC (fp);
		   *wp++ = ((char0 <<  9) | (char1 << 1)) & m | (char2 >> 7);
		char0 = GETC (fp);  *wp++ = (char2 << 6)  & m | (char0 >> 2);
		char1 = GETC (fp);
		char2 = GETC (fp);
		   *wp++ = ((char0 << 11) | (char1 << 3)) & m | (char2 >> 5);
		char0 = GETC (fp);  *wp++ = (char2 << 8)  & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 14:
		m = 0x3FFF;
		char0 = GETC (fp);
		char1 = GETC (fp);  *wp++ = (char0 << 6)  & m | (char1 >> 2);
		char2 = GETC (fp);
		char0 = GETC (fp);
		   *wp++ = ((char1 << 12) | (char2 << 4)) & m | (char0 >> 4);
		char1 = GETC (fp);
		char2 = GETC (fp);
		   *wp++ = ((char0 << 10) | (char1 << 2)) & m | (char2 >> 6);
		char0 = GETC (fp);  *wp++ = (char2 << 8)  & m |  char0;
		char0 = GETC (fp);
		char1 = GETC (fp);  *wp++ = (char0 << 6)  & m | (char1 >> 2);
		char2 = GETC (fp);
		char0 = GETC (fp);
		   *wp++ = ((char1 << 12) | (char2 << 4)) & m | (char0 >> 4);
		char1 = GETC (fp);
		char2 = GETC (fp);
		   *wp++ = ((char0 << 10) | (char1 << 2)) & m | (char2 >> 6);
		char0 = GETC (fp);  *wp++ = (char2 << 8)  & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 15:
		m = 0x7FFF;
		char0 = GETC (fp);
		char1 = GETC (fp);  *wp++ = (char0 << 7)  & m | (char1 >> 1);
		char2 = GETC (fp);
		char0 = GETC (fp);
		   *wp++ = ((char1 << 14) | (char2 << 6)) & m | (char0 >> 2);
		char1 = GETC (fp);
		char2 = GETC (fp);
		   *wp++ = ((char0 << 13) | (char1 << 5)) & m | (char2 >> 3);
		char0 = GETC (fp);
		char1 = GETC (fp);
		   *wp++ = ((char2 << 12) | (char0 << 4)) & m | (char1 >> 4);
		char2 = GETC (fp);
		char0 = GETC (fp);
		   *wp++ = ((char1 << 11) | (char2 << 3)) & m | (char0 >> 5);
		char1 = GETC (fp);
		char2 = GETC (fp);
		   *wp++ = ((char0 << 10) | (char1 << 2)) & m | (char2 >> 6);
		char0 = GETC (fp);
		char1 = GETC (fp);
		   *wp++ = ((char2 <<  9) | (char0 << 1)) & m | (char1 >> 7);
		char0 = GETC (fp);  *wp++ = (char1 << 8)  & m |  char0;

		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    case 16:
		cp = (char *)wp;
#ifdef	ICA
		cp += 2; *cp++ = GETC (fp); *cp++ = GETC (fp);
		cp += 2; *cp++ = GETC (fp); *cp++ = GETC (fp);
		cp += 2; *cp++ = GETC (fp); *cp++ = GETC (fp);
		cp += 2; *cp++ = GETC (fp); *cp++ = GETC (fp);
		cp += 2; *cp++ = GETC (fp); *cp++ = GETC (fp);
		cp += 2; *cp++ = GETC (fp); *cp++ = GETC (fp);
		cp += 2; *cp++ = GETC (fp); *cp++ = GETC (fp);
		cp += 2; *cp++ = GETC (fp); *cp++ = char0 = GETC (fp);
#else	PC
		cp[1] = GETC (fp); cp[0] = GETC (fp); cp += 4;
		cp[1] = GETC (fp); cp[0] = GETC (fp); cp += 4;
		cp[1] = GETC (fp); cp[0] = GETC (fp); cp += 4;
		cp[1] = GETC (fp); cp[0] = GETC (fp); cp += 4;
		cp[1] = GETC (fp); cp[0] = GETC (fp); cp += 4;
		cp[1] = GETC (fp); cp[0] = GETC (fp); cp += 4;
		cp[1] = GETC (fp); cp[0] = GETC (fp); cp += 4;
		cp[1] = GETC (fp); cp[0] = char0 = GETC (fp); cp += 4;
#endif	ICA/PC
		if (char0 < 0)	/* EOF */
			return (0);
		else
			break;

	    default:
		fprintf (stderr, "%s: No. de bits inválido: %d\n", pgname, bit_sz);
		quit (1);

	}	/* end switch */

	in_cnt += bit_sz;

	return (NC);

}	/* end fillcode */
