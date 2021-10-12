/*
 ****************************************************************
 *								*
 *			t1bio.c					*
 *								*
 *	Funções para leitura do sistema de arquivos T1		*
 *								*
 *	Versão	4.3.0, de 14.08.02				*
 *		4.6.0, de 21.10.04				*
 *								*
 *	Módulo: Boot2						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <common.h>
#include <sync.h>

#include <t1.h>
#include <stat.h>
#include <disktb.h>
#include <bhead.h>
#include <devmajor.h>

#include "../h/common.h"
#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
T1DINO		*t1_iget (const DISKTB *pp, ino_t inode);

/*
 ****************************************************************
 *	Procura o Arquivo Desejado				*
 ****************************************************************
 */
T1DINO *
t1_iname (const DISKTB *pp, const char *path)
{
	ino_t		inode;
	long		lbn;
	daddr_t		daddr;
	T1DIR		*dp;
	T1DINO		*np = NOT1DINO;
	char		*cp;
	char		area[BL4SZ];
	char		path_vec[128];

	/*
	 *	converte o caminho (para facilitar a comparação)
	 */
	while (*path == '/')
		path++;

	strcpy (path_vec, path);

	for (cp = path_vec; *cp != '\0'; cp++)
	{
		if   (*cp == '/')
			*cp = '\0';
	}

	*++cp = '\0';	/* NULO extra */

	/*
	 *	Percorre os diretórios, procurando o arquivo pedido
	 */
	for (cp = path_vec, inode = T1_ROOT_INO; *cp != '\0'; cp = strend (cp) + 1)
	{
		if ((np = t1_iget (pp, inode)) == NOT1DINO)
			{ printf ("\nINODE %d inexistente\n", inode); return (NOT1DINO); }

		for (lbn = 0; (daddr = daddrvec[lbn]) != 0; lbn++)
		{
			bread (pp, T1_BL4toBL (daddr), area, BL4SZ);

			if (vflag)
				printf ("Li o bloco %d do diretório\n", daddr);

			for (dp = (T1DIR *)area; dp < (T1DIR *)&area[BL4SZ]; dp = T1DIR_SZ_PTR (dp))
			{
				if (dp->d_ino == 0)
					continue;

				if (vflag)
					printf (" (%s :: %s)", cp, dp->d_name);

				if (streq (cp, dp->d_name))
					goto next;

			}	/* end for (um bloco de um INODE) */

		}	/* end for (blocos de um INODE) */

		{ printf ("\nArquivo inexistente\n"); return (NOT1DINO); }

		/*
		 *	Próximo INODE
		 */
	    next:
		if (vflag)
			printf ("\n");

		inode = dp->d_ino;

	}	/* end for (INODEs) */

	/*
	 *	Já sai da malha com o INODE do arquivo desejado
	 */
	if ((np = t1_iget (pp, inode)) == NOT1DINO)
		{ printf ("\nINODE %d inexistente\n", inode); return (NOT1DINO); }

	if (vflag)
		printf ("\ninode = %d, mode = %4X\n", inode, np->n_mode);

	return (np);

}	/* end t1_iname */

/*
 ****************************************************************
 *	Obtém um INODE de um arquivo				*
 ****************************************************************
 */
T1DINO *
t1_iget (const DISKTB *pp, ino_t inode)
{
	T1DINO		*np;
	char		area[BL4SZ];
	static T1DINO	dino;

	bread (pp, T1_BL4toBL (T1_INOtoADDR (inode)), area, BL4SZ);

	np = (T1DINO *)area + T1_INOtoINDEX (inode);

	memmove (&dino, np, sizeof (T1DINO));

	memmove (&daddrvec[0], np->n_addr, (T1_NADDR - 3) * sizeof (daddr_t));

	/*
	 *	Le o bloco simplesmente indireto: Isto suporta até (4 MB + 64 KB)
	 */
	if (np->n_addr[T1_NADDR - 3])
		bread (pp, T1_BL4toBL (np->n_addr[T1_NADDR - 3]), &daddrvec[T1_NADDR - 3], BL4SZ);

	return (&dino);

}	/* end t1_iget */
