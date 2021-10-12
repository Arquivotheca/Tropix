/*
 ****************************************************************
 *								*
 *			reduce0.c				*
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
#include <stat.h>
#include <signal.h>
#include <string.h>

#include "../h/reduce.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry long	in_cnt, out_cnt; /* Total de caracteres de entrada/saída */

entry int	in_fd;		/* Descritor do arquivo */

entry FILE	*out_fp;	/* FILE do arquivo */

entry int	code_area[NC];	/* Cabem sempre NC códigos até de 16 bits */

/*
 ****** Parâmetros de compressão ********************************
 */
entry int	MAX_BIT_SZ;	/* O algoritmo funciona no máximo com 16 bits */ 

entry int	MAX_BIT_USED;	/* Número de bits realmente usados */ 

entry int	MAX_CODE;	/* Nunca pode usar este código */

entry int	CODE_SOURCE;	/* Fonte de novos códigos */

entry int	TEST_POINT;	/* Próximo teste de performance */

entry int	bit_sz;		/* Tamanho atual */

entry int	max_code;	/* Último código do tamanho atual */

/*
 ****************************************************************
 *	Edita uma percentagem					*
 ****************************************************************
 */
char *
editpercent (int quoc)
{
	static char	buf[8];
	char		*sinal;

	if (quoc < 0)
		{ sinal = "-"; quoc = -quoc; }
	else
		sinal = "";


	sprintf (buf, "%s%d.%02d", sinal, quoc / 100, quoc % 100);
	
	return (buf);

}	/* end editpercent */

/*
 ****************************************************************
 *	Calcula uma percentagem					*
 ****************************************************************
 */
int
getpercent (long numer, long denom)
{
	int		quoc;

	/*
	 *	Tenta evitar overflow
	 */
	if (numer > (0x7FFFFFFF / 10000))
		quoc = numer / (denom / 10000);
	else
		quoc = 10000 * numer / denom;

	return (quoc);

}	/* end percent */
