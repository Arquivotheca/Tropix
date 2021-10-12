/*
 ****************************************************************
 *								*
 *			do_create.c				*
 *								*
 *	Cria o arquivo de sinopse				*
 *								*
 *	Versão	1.0.0, de 06.10.86				*
 *		4.2.0, de 01.10.01				*
 *								*
 *	Módulo: MKLIB						*
 *		Utilitários Especiais				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2001 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <ftw.h>
#include <unistd.h>
#include <a.out.h>
#include <errno.h>

#include "../h/mklib.h"

/*
 ****************************************************************
 *	Cria o arquivo de sinopse				*
 ****************************************************************
 */
int
do_create (const char *argv[])
{
	/*
	 *	Pequena consistência
	 */
	if (argv[0] != NOSTR)
		error ("$A criação não deve ter nomes de <módulo>s");

	/*
	 *	Aloca memória para a tabela HASH
	 */
	if ((hash_tb = calloc (HASHSZ, sizeof (SYMTB *))) == (SYMTB **)NULL)
		error (NOSTR);

	/*
	 *	Remove o arquivo de sinopse antigo
	 */
	if (unlink (lib_sinop_nm) < 0 && errno != ENOENT)
		error ("$*Não consegui remover \"%s\"", lib_sinop_nm);

	/*
	 *	Gera o novo arquivo de sinopse
	 */
	mod_first = mod_last = NOMOD;

	if (ftw (lib_dir_nm, ftw_create_function) < 0)
		error ("A função \"ftw\" devolveu um valor negativo");

	free (hash_tb);

	return (-1);	/* Ordena e escreve a sinopse */

}	/* end do_create */

/*
 ****************************************************************
 *	Insere um módulo nas tabelas				*
 ****************************************************************
 */
int
ftw_create_function (const char *mod_path, STAT *sp, int opt)
{
	const char	*mod_nm = mod_path;
	MOD		*mp;

	/*
	 *	Diretório não legível ou não pôde dar "stat"
	 */
	if (opt == FTW_NS || opt == FTW_DNR)
	{
		error ("*Não pude ler/obter o estado de \"%s\"", mod_nm);
		return (0);
	}

	/*
	 *	Prepara e verifica se aceita o nome
	 */
	if (patmatch (mod_path, ".!.*") || patmatch (mod_path, "*/.!.*"))
	{
		if (opt == FTW_D)
			return (1);	/* Pula toda subárvore */
		else
			return (0);
	}

	if (opt != FTW_F)
		return (0);

	if (!S_ISREG (sp->st_mode))
	{
		error ("O arquivo \"%s\" não é regular", mod_path);
		return (0);
	}

	if (lib_dir_nm_len == 1 && lib_dir_nm[0] == '.' && mod_nm[0] != '.')
		mod_nm -= 2;

	mod_nm += lib_dir_nm_len + 1;

	/*
	 *	Verifica se o dispositivo do arquivo é o mesmo 
	 *	da raiz da biblioteca
	 */
	if (sp->st_dev != lib_dev)
	{
		error
		(
			"O módulo \"%s\" NÃO está no mesmo dispositivo que \"%s\"",
			mod_path, lib_dir_nm
		);
	}

	/*
	 *	Insere o módulo na lista
	 */
	if ((mp = malloc (sizeof (MOD))) == NOMOD)
		error (NOSTR);

	if ((mp->m_mod_nm = strdup (mod_nm)) == NOSTR)
		error (NOSTR);

	mp->m_ino = sp->st_ino;

	if (mod_first == NOMOD)
		mod_first = mp;
	else
		mod_last->m_next = mp;

	mp->m_next = NOMOD;
	mod_last   = mp;

	/*
	 *	Insere os símbolos
	 */
	if (mod_insert (mod_nm, mp, sp, NOHEADER) < 0)
		return (0);

	return (0);

}	/* end ftw_create_function */

/*
 ****************************************************************
 *	Insere um módulo na tabela de módulos 			*
 ****************************************************************
 */
int
mod_insert (const char *mod_nm, MOD *mp, const STAT *sp, HEADER *hp)
{
	int		fd;
	HEADER		h;
	SYM		*sym_tb;

	/*
	 *	Abre o arquivo
	 */
	if ((fd = inopen (sp->st_dev, sp->st_ino)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", mod_nm);
		return (-1);
	}

	/*
	 *	Verifica se o HEADER já está disponível
	 */
	if (hp == NOHEADER)
	{
		hp = &h;

		/*
		 *	Verifica se o módulo é objeto
		 */
		if (read (fd, hp, sizeof (HEADER)) != sizeof (HEADER))
		{
			error ("*Erro na leitura do cabeçalho de \"%s\"", mod_nm);
			close (fd); 	return (-1);
		}

		if (hp->h_magic != FMAGIC)
		{
			error ("O arquivo \"%s\" não é um módulo objeto", mod_nm);
			close (fd); 	return (-1);
		}
	}

	/*
	 *	Aloca memória e le a tabela de símbolos
	 */
	sym_tb = alloca (hp->h_ssize);

	lseek (fd, sizeof (HEADER) + hp->h_tsize + hp->h_dsize, L_SET);

	if (hp->h_ssize && read (fd, sym_tb, hp->h_ssize) != hp->h_ssize)
	{
		error ("*Erro na leitura da tabela de símbolos de \"%s\"", mod_nm);
		close (fd); 	return (-1);
	}

	close (fd);

	/*
	 *	Processa os símbolos do arquivo
	 */
	if (vflag)
		printf ("a - %s\n", mod_nm);

	mk_mod_sym
	(
		mod_nm,
		mp,
		sym_tb,
		(SYM *)((int)sym_tb + hp->h_ssize),
		1				/* HASHON */
	);

	return (0);

}	/* end mod_insert */

/*
 ****************************************************************
 *	Processa a Tabela de símbolos de um módulo		*
 ****************************************************************
 */
int
mk_mod_sym
(
	const char	*mod_nm,
	MOD		*mp,
	SYM		*sym_tb,
	SYM		*end_sym_tb,
	int		hash_flag
)
{
	const SYM	*sp;
	SYMTB		*zp, **link_point;
	SYMTB		*first_sym = NOSYMTB, *last_sym = NOSYMTB;
	char		type;

	/*
	 *	Percorre toda a tabela de símbolos
	 */
	mp->m_n_sym = 0;

	for (sp = sym_tb; sp < end_sym_tb; sp = SYM_NEXT_PTR (sp))
	{
		/*
		 *	Analisa o símbolo
		 */
		if ((sp->s_type & EXTERN) == 0)
			continue;

		type = sp->s_type & ~EXTERN;

		if (type < TEXT || type > BSS)
			continue;

		/*
		 *	Verifica se a Hash está ativa: testa duplicações
		 */
		if (hash_flag)
		{
			if ((zp = hash (sp->s_name, sp->s_nm_len, &link_point)) != NOSYMTB)
			{
				if (vflag)
					fprintf (stderr, "\t");

				error
				(
					"Símbolo \"%s\" duplicado: "
					"\"%s\" :: \"%s\"",
					sp->s_name,
					mod_nm,
					zp->z_mod->m_mod_nm
				);

				continue;
			}

		}	/* end if (HASH on) */

		/*
		 *	Cria a estrutura do símbolo
		 */
		if ((zp = malloc (sizeof (SYMTB))) == NOSYMTB)
			error (NOSTR);

		if (hash_flag)
			{ *link_point = zp; zp->z_link = NOSYMTB; }

		if ((zp->z_sym_nm = strdup (sp->s_name)) == NOSTR)
			error (NOSTR);

		zp->z_sym_len = sp->s_nm_len,
		zp->z_mod     = mp;

		if (first_sym == NOSYMTB)
			first_sym = zp;
		else
			last_sym->z_sym_next = zp;

		last_sym = zp;
		zp->z_sym_next = NOSYMTB;

	   /***	zp->z_link = NOSYMTB; ***/

		mp->m_n_sym++;

	}	/* end percorrendo a tabela de símbolos do módulo */

	mp->m_sym = first_sym;

	if (sp != end_sym_tb)
		error ("Defasamento na tabela de símbolos de \"%s\"", mod_nm);

	return (mp->m_n_sym);

}	/* end mk_mod_sym */
