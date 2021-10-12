/*
 ****************************************************************
 *								*
 *			sub.c					*
 *								*
 *	Funções auxiliares					*
 *								*
 *	Versão	4.3.0, de 11.07.02				*
 *		4.3.0, de 16.08.02				*
 *								*
 *	Módulo: fsckt1						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/common.h>
#include <sys/sync.h>
#include <sys/stat.h>
#include <sys/t1.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <setjmp.h>

#include "../h/fsckt1.h"

/*
 ****************************************************************
 *	Obtém um INODE do disco					*
 ****************************************************************
 */
T1DINO *
inode_get (ino_t ino)
{
	bget (&inoblk, T1_INOtoADDR (ino));

	return ((T1DINO *)inoblk.b_area + T1_INOtoINDEX (ino));

}	/* end inode_get */

/*
 ****************************************************************
 *	Edita um tipo de bloco					*
 ****************************************************************
 */
const char *
edit_block_type (int type)
{
	switch (type)
	{
	    case T1_MAP_FREE:
		return ("FREE");

	    case T1_MAP_MAP:
		return ("MAP");

	    case T1_MAP_DATA:
		return ("DATA");

	    case T1_MAP_INDIR:
		return ("INDIR");

	    case T1_MAP_INODE_EMPTY:
		return ("INODE_EMPTY");

	    case T1_MAP_INODE_FULL:
		return ("INODE_FULL");

	    case T1_MAP_BAD:
		return ("BAD");

	    case T1_MAP_OUT:
		return ("OUT");

	    default:
		return ("????");
	};

}	/* end edit_block_type */

/*
 ****************************************************************
 *	Escreve as Informações do INODE				*
 ****************************************************************
 */
void
print_inode (ino_t ino, const T1DINO *np)
{
	char		*cp;
	TM		*tm;
	time_t		mtime;

	printf ("\nINODE %d:  ", ino);

	cp = modetostr (np->n_mode);
	printf ("%c  %s  ", *cp, cp + 14);

	printf ("%d  ", np->n_nlink);

	if ((cp = (char *)pwcache (np->n_uid)) != NOSTR)
		printf ("%s  ", cp);
	else
		printf ("usuário %d  ", np->n_uid);

	printf ("%d  ", np->n_size);

	mtime = np->n_mtime; tm = localtime (&mtime);

	printf
	(	"%02d.%02d.%02d\n",
		tm->tm_mday, tm->tm_mon + 1, tm->tm_year % 100
	);

}	/* end print_inode */

/*
 ****************************************************************
 *	Pergunta SIM/NÃO ao usuário				*
 ****************************************************************
 */
int
faskyesno (const char *msg)
{
	int		c;

	for (EVER)
	{
		printf ("%s? (n): ", msg);

		if (fflag < 0)
			{ printf ("NÃO\n\n"); return (-1); }

		if (read_only)
			{ printf ("NÃO (sistema montado)\n\n"); return (-1); }

		if (fflag > 0)
			{ printf ("SIM\n\n"); return (1); }

		if ((c = getchar ()) != '\n')
		{
			while (getchar () != '\n')
				/* vazio */;
		}

		printf ("\n");

		switch (c)
		{
		    case '\n':
			return (-1);

		    case 'S':
		    case 'Y':
			fflag = 1;
		    case 's':
		    case 'y':
			return (1);

		    case 'N':
			fflag = -1;
		    case 'n':
			return (-1);

		}	/* end switch */

	}	/* end for (EVER) */

}	/* end faskyesno */

/*
 ****************************************************************
 *	Função de HASH para INODEs				*
 ****************************************************************
 */
INO_HASH *
ino_hash (ino_t ino)
{
	INO_HASH	*ip;

	/*
	 *	Verifica se já tem lista de colisão
	 */
	link_place = &inode_hash_tb[ino & INO_HASH_MASK];

	if ((ip = *link_place) == NO_INO_HASH)
		return (NO_INO_HASH);

	/*
	 *	Percorre a lista de colisão
	 */
	for (EVER)
	{
		if (ip->i_ino == ino)
			return (ip);

		if (ip->i_next == NO_INO_HASH)
			{ link_place = &ip->i_next; return (NO_INO_HASH); }
		else
			ip = ip->i_next;
	}

}	/* end hash */
