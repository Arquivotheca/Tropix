/*
 ****************************************************************
 *								*
 *			initab.c				*
 *								*
 *	Leitura do arquivo de inicialização do sistema		*
 *								*
 *	Versão	4.6.0, de 15.07.04				*
 *		4.6.0, de 15.07.04				*
 *								*
 *	Módulo: initab						*
 *		libc/etc					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <init.h>

/*
 ****************************************************************
 *	Variáveis globais					*
 ****************************************************************
 */
#define	INITAB_FILE	"/etc/initab"

static FILE	*init_fp;		/* Fp do Arquivo */
static int	init_slot = -1;		/* O índice da entrada no arquivo */

/*
 ****************************************************************
 *	Prepara para a Leitura de INITAB_FILE			*
 ****************************************************************
 */
void
setinitent (void)
{
	if (init_fp == NOFILE)
		init_fp = fopen (INITAB_FILE, "r");

	if (init_fp == NOFILE)
		return;

	init_slot = -1;

	rewind (init_fp);

}	/* end setinitent */

/*
 ****************************************************************
 *	Le uma entrada e converte 				*
 ****************************************************************
 */
INITAB *
getinitent (void)
{
	char		c;
	char		*cp;
	static INITAB	*initp;
	static char	*init_area;

	/*
	 *	Prólogo
	 */
	if (init_area == NOSTR && (init_area = malloc (120)) == NOSTR)
		return (NOINITAB);

	if (initp == NOINITAB && (initp = malloc (sizeof (INITAB))) == NOINITAB)
		return (NOINITAB);

	if (init_fp == NOFILE)
		setinitent ();

	if (init_fp == NOFILE)
		return (NOINITAB);

	do
	{
		if (fngets (init_area, 120, init_fp) == NOSTR)
			return (NOINITAB);
	}
	while ((c = init_area[0]) == '\0' || c == '#');

	/*
	 *	Inicialmente, zera os campos
	 */
	initp->it_state	= ' ';
	initp->it_name	= "";
	initp->it_term	= "";
	initp->it_local	= "";

	/*
	 *	Em seguida, prepara os campos
	 */
	initp->it_slot = ++init_slot;

	if ((cp = strfield (init_area, ':')) != NOSTR)
		initp->it_state = cp[0];

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		initp->it_name = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		initp->it_term = cp;

	if ((cp = strfield (NOSTR, ':')) != NOSTR)
		initp->it_local = cp;

	return (initp);

}	/* end getinitent */

/*
 ****************************************************************
 *	Encerra a leitura do arquivo "/etc/passwd"		*
 ****************************************************************
 */
void
endinitent (void)
{
	fclose (init_fp);

	init_slot = -1;

	init_fp = NOFILE;

}	/* end endinitent */

/*
 ****************************************************************
 *	Procura uma entrada dado o nome da linha		*
 ****************************************************************
 */
INITAB *
getinitnam (const char *name)
{
	INITAB		*initp, *save_initp = NOINITAB;

	/*
	 *	Em primeiro lugar, procura nas linhas tradicionais
	 */
	setinitent ();

	while ((initp = getinitent ()) != NOINITAB)
	{
		save_initp = initp;

		if (streq (initp->it_name, name))
			return (initp);
	}

	/*
	 *	NÃO achou; verifica se é um pseudo terminal
	 */
	if ((initp = save_initp) == NOINITAB)
		return (NOINITAB);

	if (strlen (name) != 6)
		return (NOINITAB);

	if (strncmp (name, "ptyc", 4) != 0)
		return (NOINITAB);

	if (!isdigit (name[4]) || !isdigit (name[5]))
		return (NOINITAB);

	/*
	 *	Finalmente, simula uma entrada
	 */
	initp->it_slot	= init_slot + atol (&name[4]) + 1;
	initp->it_state	= '+';
	initp->it_name	= name;
	initp->it_term	= "vt100,iso";
	initp->it_local	= "Janela local";

	return (initp);

}	/* end getinitnam */
