/*
 ****************************************************************
 *								*
 *			pass6.c					*
 *								*
 *	Verifica os símbolos das bibliotecas compartilhadas	*
 *								*
 *	Versão	1.0.0, de 23.10.86				*
 *		4.3.0, de 10.06.02				*
 *								*
 *	Módulo: ld						*
 *		Utilitários básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <a.out.h>

#include "../h/ld.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */

/*
 ******	Protótipos de funções ***********************************
 */
void		check_shared_library_symbol_table (const MODULE *mp);

/*
 ****************************************************************
 *	Verifica os símbolos das bibliotecas compartilhadas	*
 ****************************************************************
 */
void
pass6 (void)
{
	const MODULE	*mp;
	const GSYM	*gp;

	/*
	 *	Verifica se os símbolos estão nas bibliotecas compartilhadas
	 */
	if (external_reference_cnt > 0 && shared_lib_given)
	{
		for (mp = modtb_list.m_next; mp != NOMODULE; mp = mp->m_next)
		{
			if (mp->m_type != SMAGIC)
				continue;

			check_shared_library_symbol_table (mp);
		}
	}

	/*
	 *	Verifica se não há símbolos indefinidos
	 */
	if (!rflag && !bflag)
	{
		int		undef = 0;

		for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
		{
			if (!IS_UNDEF_OR_COMMON (&gp->s))
				continue;

			if (undef++ == 0)
				fprintf (stderr, "Símbolos indefinidos:\n");

			fprintf (stderr, "   %s\n", gp->s.s_name);

			error_standard_count++;
		}
	}

#ifdef	DEBUG
	/*
	 *	Imprime a lista de referências
	 */
	{
		const GSYM	*gp;
		int		index;

		if (vflag) for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
		{
			if (gp->s_ref_len == 0)
				continue;

			printf ("%31.31s: ", gp->s.s_name);

			for (index = 0; index < gp->s_ref_len; index++)
				printf (" %P", gp->s_ref_vec[index]);

			printf ("\n");
		}

	}
#endif	DEBUG

}	/* end pass6 */

/*
 ****************************************************************
 *	Resolve os símbolos das bibliotecas compartilhadas	*
 ****************************************************************
 */
void
check_shared_library_symbol_table (const MODULE *mp)
{
	int		fd;
	HEADER		h;
	const SYM	*sp;
	void		*image;
	const SYM	*end_image;
	GSYM		*gp;

	/*
	 *	Abre a biblioteca
	 */
	if ((fd = inopen (mp->m_dev, mp->m_ino)) < 0)
		{ error ("*@Não consegui abrir \"%s\"", mp->m_nm); return; }

	if (read (fd, &h, sizeof (HEADER)) != sizeof (HEADER))
		{ error ("*@Erro na leitura do cabeçalho de \"%s\"", mp->m_nm); goto bad; }

	/*
	 *	Le a tabela de símbolos
	 */
	if (h.h_ssize == 0)
		{ error ("Biblioteca compartilhada \"%s\" sem símbolos?", mp->m_nm); goto bad; }

	image = alloca (h.h_ssize);

	lseek (fd, h.h_tsize + h.h_dsize, SEEK_CUR);

	if (read (fd, image, h.h_ssize) != h.h_ssize)
		{ error ("*@Erro na leitura da tabela de símbolos de \"%s\"", mp->m_nm); goto bad; }

	end_image = image + h.h_ssize;

	/*
	 *	Analisa os símbolos
	 */
	for (sp = image; sp < end_image; sp = SYM_NEXT_PTR (sp))
	{
		if ((sp->s_type & EXTERN) == 0)
			continue;

		/*
		 *	Procura o símbolo na tabela global
		 */
		if ((gp = hash (sp->s_name, sp->s_nm_len)) == NOGSYM)
			continue;

		if (!IS_UNDEF_OR_COMMON (&gp->s))
			continue;

		gp->s.s_type = sp->s_type;

	}	/* end for (símbolos da biblioteca compartilhada) */

	if (sp != end_image)
		error ("Defasamento na tabela de símbolos da biblioteca \"%s\"", mp->m_nm);

	/*
	 *	Terminou
	 */
    bad:
	close (fd);

}	/* end check_shared_library_symbol_table */
