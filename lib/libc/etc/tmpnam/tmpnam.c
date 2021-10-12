/*
 ****************************************************************
 *								*
 *			tmpnam.c				*
 *								*
 *	Gera nomes para arquivos temporários			*
 *								*
 *	Versão	2.3.0, de 03.03.90				*
 *		3.0.0, de 26.06.95				*
 *								*
 *	Módulo: tmpnam						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/syscall.h>

#include <stdio.h>
#include <unistd.h>

/*
 ****************************************************************
 *	Gera nomes de arquivos temporários			*
 ****************************************************************
 */
char *
tmpnam (register char *str)
{
	register char	c;
	static char	nmvec[L_tmpnam];

	/*
	 *	Verifica se foi dado um ponteiro
	 */
	if (str == NOSTR)
		str = nmvec;

	/*
	 *	Formato: "/tmp/lddddd"
	 */
	sprintf (str, "/tmp/x%05d", getpid () % 100000 ); 

	for (c = 'a'; c <= 'z'; c++)
	{
		str[5] = c;

		if (access (str, F_OK) < 0)
			return (str);
	}

	return (NOSTR);

}	/* end tmpnam */
