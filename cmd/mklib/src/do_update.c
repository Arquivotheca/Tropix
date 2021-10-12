/*
 ****************************************************************
 *								*
 *			update.c				*
 *								*
 *	Atualiza módulos da biblioteca				*
 *								*
 *	Versão	1.0.0, de 06.10.86				*
 *		4.0.0, de 20.08.01				*
 *								*
 *	Módulo: MKLIB						*
 *		Utilitários Especiais				*
 *		Categoria D					*
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
#include <dirent.h>
#include <a.out.h>
#include <fcntl.h>
#include <stat.h>
#include <ftw.h>

#include "../h/mklib.h"

/*
 ****************************************************************
 *	Atualiza módulos da biblioteca				*
 ****************************************************************
 */
int
do_update (const char *argv[])
{
	MOD		*mp;
	DIR		*dir_fp;
	const DIRENT	*dp;
	int		modif = 0;
	char		*nm_point, *mod_path;
	const char	**cpp;
	int		r = 0, len, max_len = 0;
	STAT		new_s, old_s;
	HEADER		h;

	/*
	 *	Pequena consistência
	 */
	if (argv[0] != NOSTR)
		error ("$A atualização não deve ter nomes de <módulo>s");

	/*
	 *	Lê a sinopse e insere todos os símbolos na tabela HASH
	 */
	read_sinop_file (1);

	if ((hash_tb = calloc (HASHSZ, sizeof (SYMTB *))) == NULL)
		error (NOSTR);

	hash_all_sym ();

	/*
	 *	Prepara o prefixo dos nomes dos módulos
	 */
	for (cpp = argv, max_len = 0; *cpp != NOSTR; cpp++)
	{
		if (max_len < (len = strlen (*cpp)))
			max_len = len;
	}

	mod_path = alloca (lib_dir_nm_len + 1 + max_len);

	strcpy (mod_path, lib_dir_nm);

	strcat (mod_path, "/");	nm_point = strend (mod_path);

	/*
	 *	Abre o diretório corrente
	 */
	if ((dir_fp = opendir (".")) == NODIR)
		error ("$*Não consegui abrir o diretório corrente");

	/*
	 *	Le o diretório
	 */
	while ((dp = readdir (dir_fp)) != NODIRENT)
	{
		/*
		 *	Ignora ".", ".." e ".old"
		 */
		if (dp->d_name[0] ==  '.')
			continue;

		/*
		 *	Verifica se é objeto
		 */
		if (strcmp (strend (dp->d_name) - 2, ".o") != 0)
			continue;

		/*
		 *	Busca em LIBMOD apenas pela ultima componente
		 */
		if ((mp = last_nm_mod_search (dp->d_name)) == NOMOD)
		{
			error ("O módulo \"%s\" não pertence à biblioteca", dp->d_name);
			continue;
		}

		/*
		 *	O modulo pertence à biblioteca. Obtem os estados
		 */
		if (stat (dp->d_name, &new_s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", dp->d_name);
			continue;
		}

		strcpy (nm_point, mp->m_mod_nm);

		if (instat (lib_dev, mp->m_ino, &old_s) < 0)
		{
			error ("*Não consegui obter o estado de \"%s\"", mod_path);
			continue;
		}

		/*
		 *	Compara as datas
		 */
		if (new_s.st_mtime <= old_s.st_mtime)
			continue;

		/*
		 *	Consulta o usuário
		 */
		if   (!fflag)
		{
			fprintf (stderr, "Atualiza \"%s\"? (n): ", mp->m_mod_nm);

			if (askyesno () <= 0)
				continue;
		}

		/*
		 *	Atualiza
		 */
		if (mod_copy (dp->d_name, /* => */ mod_path, &new_s, &h) < 0)
			continue;

		if ((r = mod_replace (mp, &new_s, &h, 'u')) > 0)
			modif += r;

	}	/* end for (lendo o diretório) */

	closedir (dir_fp);

	free (hash_tb);

	return (modif);		/* Escreve (Não precisa ordenar) */

}	/* end update */
