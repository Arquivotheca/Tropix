/*
 ****************************************************************
 *								*
 *			nlist.c					*
 *								*
 *	Obtem trechos da tabela de símbolos um objeto		*
 *								*
 *	Versão	1.0.0, de 20.11.86				*
 *		4.0.0, de 22.08.01				*
 *								*
 *	Módulo: nlist						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <a.out.h>
#include <nlist.h>
#include <errno.h>

/*
 ****************************************************************
 *	Extrai valores da tabela de simbolos			*
 ****************************************************************
 */
int
nlist (const char *file, NLIST *nl)
{
	NLIST		*lp;
	int		fd;
	int		n_nl;
	HEADER		header;
	void		*image, *end_image;
	const SYM	*sp;

	/*
	 *	Zera a Tabela
	 */
	for (n_nl = 0, lp = nl; lp->n_name != NOSTR; lp++)
		{ n_nl++; lp->n_type  = 0; lp->n_value = 0; }

	/*
	 *	Abre e analisa o Arquivo
	 */
	errno = 0;

	if ((fd = open (file, O_RDONLY)) < 0)
		return (-1);

	if (read (fd, &header, sizeof (HEADER)) != sizeof (HEADER))
		{ close (fd); return (-2); }

	if (header.h_magic != FMAGIC && header.h_magic != NMAGIC)
		{ close (fd); return (-3); }

	if (header.h_ssize == 0)
		{ close (fd); return (-4); }

	/*
	 *	Le a Tabela de Simbolos do Programa
	 */
	lseek (fd, sizeof (HEADER) + header.h_tsize + header.h_dsize, L_SET);

	image = alloca (header.h_ssize); end_image = image + header.h_ssize;

	if (read (fd, image, header.h_ssize) != header.h_ssize)
		{ close (fd); return (-5); }

	close (fd);

	/*
	 *	Procura o(s) nome(s) lido(s) na lista
	 */
	for (sp = image; n_nl > 0 && sp < (SYM *)end_image; sp = SYM_NEXT_PTR (sp))
	{
		for (lp = nl; lp->n_name != NOSTR; lp++)
		{
			if (streq (lp->n_name, sp->s_name))
			{
				lp->n_type  = sp->s_type;
				lp->n_value = sp->s_value;

				n_nl--;
			}
		}

	}	/* end (percorrendo a tabela de símbolos do módulo */

	return (0);

}	/* end nlist */
