/*
 ****************************************************************
 *								*
 *			do_replace.c				*
 *								*
 *	Insere/substitui módulos na biblioteca			*
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
#include <unistd.h>
#include <string.h>
#include <stat.h>
#include <fcntl.h>
#include <a.out.h>
#include <errno.h>

#include "../h/mklib.h"

/*
 ****************************************************************
 *	Cria o arquivo de sinopse				*
 ****************************************************************
 */
int
do_replace (const char *argv[])
{
	MOD		*mp, *lp, *ap;
	const char	*mod_nm, **cpp;
	char		*mod_path, *nm_point;
	int		len, max_len, r, modif = 0, sort = 0;
	STAT		s;
	HEADER		h;

	/*
	 *	Pequena consistência
	 */
	if (argv[0] == NOSTR)
		error ("$A substituição deve ter nomes de <módulo>s");

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
	 *	Processa cada um dos argumentos (módulos)
	 */
	while ((mod_nm = *argv++) != NOSTR)
	{
		strcpy (nm_point, mod_nm);

		if (mod_copy (last_nm (mod_nm), /* => */ mod_path, &s, &h) < 0)
			continue;

		/*
		 *	Verifica se o módulo já está na biblioteca
		 */
		if ((mp = mod_search (mod_nm)) == NOMOD)
		{
			/* O módulo ainda NÃO pertence à biblioteca */

			if ((mp = malloc (sizeof (MOD))) == NOMOD)
				error (NOSTR);

			mp->m_mod_nm = mod_nm;
			mp->m_ino = s.st_ino;
		   /***	mp->m_sym = ...; ***/
		   /***	mp->m_n_sym = ...; ***/
		   /***	mp->m_next = ...; ***/

			/* Insere já no local correto da lista */

			for (ap = NOMOD, lp = mod_first; lp != NOMOD; ap = lp, lp = lp->m_next)
			{
				if (strcmp (lp->m_mod_nm, mp->m_mod_nm) > 0)
					break;
			}

			if (ap == NOMOD)
				mod_first = mp;
			else
				ap->m_next = mp;

			mp->m_next = lp;

			if (mod_insert (mod_nm, mp, &s, &h) < 0)
				continue;

			modif++; sort++;
		}
		else
		{
			/* O módulo já pertence à biblioteca */

			if ((r = mod_replace (mp, &s, &h, 'r')) > 0)
				modif += r;
		}

	}	/* end for (nomes de módulos) */

	free (hash_tb);

	if (sort)
		return (-modif);	/* < 0  =>  Ordena */
	else
		return (modif);

}	/* end do_replace */

/*
 ****************************************************************
 *	Instala todos os símbolos na tabela HASH		*
 ****************************************************************
 */
void
hash_all_sym (void)
{
	const MOD	*mp;
	const SYMTB	*old_zp;
	SYMTB		*zp, **link_point;

	/*
	 *	Aloca memória para a tabela HASH
	 */
	if (hash_tb == NULL)
		error ("$Tabela HASH não alocada");

	/*
	 *	Insere os símbolos
	 */
	for (mp = mod_first; mp != NOMOD; mp = mp->m_next)
	{
		for (zp = mp->m_sym; zp != NOSYMTB; zp = zp->z_sym_next)
		{
			if ((old_zp = hash (zp->z_sym_nm, zp->z_sym_len, &link_point)) != NOSYMTB)
			{
				error
				(
					"Símbolo \"%s\" duplicado: \"%s\" :: \"%s\"",
					zp->z_sym_nm,
					mp->m_mod_nm, old_zp->z_mod->m_mod_nm
				);
			}
			else
			{
				*link_point = zp; zp->z_link = NOSYMTB;
			}
		}
	}

}	/* end hash_all_sym */

/*
 ****************************************************************
 *	Copia o módulo para o diretório da biblioteca		*
 ****************************************************************
 */
int
mod_copy (const char *mod_nm, /* => */ const char *mod_path, STAT *sp, HEADER *hp)
{
	int		fd_in, fd_out, n;
	char		*cp;
	int		r = -1;
	STAT		s;
	char		area[BLSZ];

	/*
	 *	Abre o arquivo original
	 */
	if ((fd_in = open (mod_nm, O_RDONLY)) < 0 || fstat (fd_in, sp) < 0)
	{
		error ("*Não consegui abrir \"%s\"", mod_nm);
		return (-1);
	}

	if (!S_ISREG (sp->st_mode))
	{
		error ("O arquivo \"%s\" não é regular", mod_nm);
		goto bad1;
	}

	/*
	 *	Le o cabeçalho e verifica se é um módulo objeto
	 */
	if (read (fd_in, hp, sizeof (HEADER)) != sizeof (HEADER))
	{
		error ("*Erro na leitura do cabeçalho de \"%s\"", mod_nm);
		goto bad1;
	}

	if (hp->h_magic != FMAGIC)
	{
		error ("O arquivo \"%s\" não é um módulo objeto", mod_nm);
		goto bad1;
	}

	/*
	 *	Verifica se precisa criar um diretório
	 */
	if ((cp = strrchr (mod_path, '/')) != NOSTR)
	{
		*cp = '\0';

		if (stat (mod_path, &s) < 0 && mkdir (mod_path, 0755) < 0)
			error ("$*NÃO consegui criar o diretório \"%s\"", mod_path);

		*cp = '/';
	}

	/*
	 *	Cria o arquivo correspondente na biblioteca
	 */
	if ((fd_out = creat (mod_path, 0644)) < 0)
	{
		error ("*Não pude criar \"%s\"", mod_path);
		goto bad1;
	}

	/*
	 *	Escreve o cabecalho
	 */
	if (write (fd_out, hp, sizeof (HEADER)) != sizeof (HEADER))
	{
		error ("*Erro ao escrever o cabeçalho de \"%s\"", mod_path);
		goto bad2;
	}

	/*
	 *	Realiza a copia
	 */
	while ((n = read (fd_in, area, BLSZ)) > 0)
	{
		if (write (fd_out, area, n) != n)
		{
			error ("*Erro de escrita em \"%s\"", mod_path);
			goto bad2;
		}
	}

	if (n < 0)
	{
		error ("*Erro de leitura de \"%s\"", mod_nm);
		goto bad2;
	}

	fstat (fd_out, sp);

	r = 0;		/* Não houve erros */

	/*
	 *	Fecha os arquivos
	 */
    bad2:
	close (fd_out);
    bad1:
	close (fd_in);

	return (r);

}	/* end mod_copy */

/*
 ****************************************************************
 *	Substitui um módulo na biblioteca			*
 ****************************************************************
 */
int
mod_replace (MOD *mp, STAT *sp, HEADER *hp, int cmd_nm)
{
	int		fd, modif = 0;
	const char	*mod_nm;
	SYM		*sym_tb;
	MOD		*new_mp;
	SYMTB		*zp, *new_zp;
	SYMTB		**link_point;

	/*
	 *	Abre o arquivo (novo), já na biblioteca
	 */
	mod_nm = /*** last_nm ***/ mp->m_mod_nm;

	if ((fd = inopen (sp->st_dev, sp->st_ino)) < 0)
	{
		error ("*Não consegui abrir \"%s\"", mod_nm);
		return (-1);
	}

	/*
	 *	Aloca memória e le a tabela de símbolos
	 */
	sym_tb = alloca (hp->h_ssize);

	lseek (fd, sizeof (HEADER) + hp->h_tsize + hp->h_dsize, L_SET);

	if (hp->h_ssize && read (fd, sym_tb, hp->h_ssize) != hp->h_ssize)
	{
		error ("*Erro na leitura da tabela de símbolos de \"%s\"", mod_nm);
		close (fd);	return (-1);
	}

	close (fd);

	/*
	 *	Processa a tabela de símbolos do novo módulo
	 */
	new_mp = alloca (sizeof (MOD));

	mk_mod_sym
	(
		mod_nm,
		new_mp,
		sym_tb,
		(SYM *)((int)sym_tb + hp->h_ssize),
		0			/* HASHOFF */
	);

	/*
	 *	Confere os tamanhos e símbolos
	 */
	if (new_mp->m_n_sym != mp->m_n_sym)
	{
		modif++;
	}
	else
	{
		sort_mod_sym (new_mp);

		for
		(	zp = mp->m_sym, new_zp = new_mp->m_sym;
			zp != NOSYMTB;
			zp = zp->z_sym_next, new_zp = new_zp->z_sym_next
		)
		{
			if (strcmp (zp->z_sym_nm, new_zp->z_sym_nm))
				{ modif++; break; }
		}
	}


	if (vflag)
		printf ("%c - %s\n", cmd_nm, mod_nm);

	if (modif == 0)
		return (0);

	/*
	 *	Atualiza LIBSYM
	 */
	for (new_zp = new_mp->m_sym; new_zp != NOSYMTB; new_zp = new_zp->z_sym_next)
	{
		if ((zp = hash (new_zp->z_sym_nm, new_zp->z_sym_len, &link_point)) != NOSYMTB)
		{
			if (zp->z_mod != mp)
			{
				error
				(
					"Símbolo \"%s\" duplicado: \"%s\" :: \"%s\"",
					zp->z_sym_nm, mod_nm, zp->z_mod->m_mod_nm
				);
			}
		}
		else
		{
			*link_point = new_zp; new_zp->z_link = NOSYMTB;
		}
	}

   /***	mp->m_mod_nm	= ...; ***/
   /***	mp->m_ino	= ...; ***/
	mp->m_sym	= new_mp->m_sym;
	mp->m_n_sym	= new_mp->m_n_sym;
   /***	mp->m_next	= ...; ***/

	return (1);

}	/* end mod_replace */
