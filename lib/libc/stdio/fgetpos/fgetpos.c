/*
 ****************************************************************
 *								*
 *			fgetpos.c				*
 *								*
 *	Obtém/restaura a posição corrente de um arquivo		*
 *								*
 *	Versão	2.3.0, de 03.03.90				*
 *		3.0.0, de 08.07.95				*
 *								*
 *	Módulo: fgetpos						*
 *		libc/stdio					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

/*
 ****************************************************************
 *	Obtém a posição corrente de um arquivo			*
 ****************************************************************
 */
int
fgetpos (FILE *fp, fpos_t *pos)
{
	long int	offset;

	if ((offset = ftell (fp)) == -1)
		return (-1);

	*pos = offset;

	return (0);

}	/* end fgetpos */

/*
 ****************************************************************
 *	Restaura a posição corrente de um arquivo		*
 ****************************************************************
 */
int
fsetpos (FILE *fp, const fpos_t *pos)
{
	return (fseek (fp, *pos, SEEK_SET));

}	/* end fgetpos */
