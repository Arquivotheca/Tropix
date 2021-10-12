/*
 ****************************************************************
 *								*
 *			pass2.c					*
 *								*
 *   Verifica os módulos necessários das bibliotecas regulares	*
 *								*
 *	Versão	1.0.0, de 14.04.87				*
 *		4.3.0, de 06.07.02				*
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
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <a.out.h>

#include "../h/ld.h"

/*
 ****************************************************************
 *	Definições locais					*
 ****************************************************************
 */
#define	LIBMAGIC	0x11DE784A	/* No. mágico (velocidade da luz) */

const char		libsym_suffix[] = "/.LIBSYM";

typedef struct libmod	LIBMOD;

typedef struct symtb	SYMTB;

struct libmod
{
	const char	*m_mod_nm;
	int		m_dev;
	int		m_ino;
	SYMTB		*m_symtb;
	LIBMOD		*m_next;
};

#define	NOLIBMOD	(LIBMOD *)NULL

struct symtb
{
	const char	*z_sym_nm;
	int		z_sym_len;
	char		z_type;
	long		z_value;
	SYMTB		*z_next;
};

#define	NOSYMTB	(SYMTB *)NULL

/*
 ******	Tabela de símbolos de todas as bibliotecas **************
 */
entry LIBMOD		*mod_first = NOLIBMOD, *mod_last = NOLIBMOD;

/*
 ****************************************************************
 *   Adiciona a tabela de símbolos de uma biblioteca regular	*
 ****************************************************************
 */
void
reg_lib_get_LIBSYM (const char *lib_nm)
{
	char		*libsym_nm;
	int		fd, ino = 0;
	STAT		s;
	char		*libsym_image;
	char		*cp, *begin_ptr, *end_ptr;
	const char	*mod_nm, *name;
	LIBMOD		*mp;
	SYMTB		*zp;
	SYMTB		*symtb_first, *symtb_last;

	/*
	 *	Tenta abrir .LIBSYM
	 */
	libsym_nm = alloca (strlen (lib_nm) + strlen (libsym_suffix) + 1);

	strcpy (libsym_nm, lib_nm); strcat (libsym_nm, libsym_suffix);

	if ((fd = open (libsym_nm, O_RDONLY)) < 0 || fstat (fd, &s) < 0)
	{
		if (errno == ENOENT)
			error ("@O diretório \"%s\" NÃO é o de uma biblioteca", lib_nm);
		else
			error ("*@Não consegui abrir \"%s\"", libsym_nm);

		return;
	}

	/*
	 *	Aloca memória e lê a biblioteca
	 */
	libsym_image = alloca (s.st_size + 1); (libsym_image + s.st_size)[0] = '\0';

	if ((read (fd, libsym_image, s.st_size)) != s.st_size)
		{ error ("*@Erro na leitura do arquivo \"%s\"", libsym_nm); close (fd); return; }

	close (fd);

	/*
	 *	Confere o número mágico
	 */
	if ((begin_ptr = strchr (libsym_image, '\n')) == NOSTR)
		error ("@Não encontrei o final da linha em \"%s\"", libsym_nm);

	*begin_ptr++  = '\0';

	if (atol (libsym_image) != LIBMAGIC)
		error ("@O arquivo \"%s\" não tem cabeçalho válido", libsym_nm);

	/*
	 *	Retira os módulos com os respectivos símbolos
	 */
	while (*begin_ptr != '\0')
	{
		if ((end_ptr = strchr (begin_ptr, '\n')) == NOSTR)
			error ("$Não encontrei o final da linha em \"%s\"", libsym_nm);

		*end_ptr++ = '\0';

		if ((mod_nm = strfield (begin_ptr, ',')) == NOSTR)
			error ("$Esperava o nome de um módulo em \"%s\"", libsym_nm);

		if ((cp = strfield (NOSTR, ':')) == NOSTR || (ino = atol (cp)) == 0)
			error ("$Esperava o número de um nó-índice em \"%s\"", libsym_nm);

		/*
		 *	Insere o módulo na lista
		 */
		if ((mp = malloc (sizeof (LIBMOD))) == NOLIBMOD)
			error (NOSTR);

		if ((mp->m_mod_nm = strdup (mod_nm)) == NOSTR)
			error (NOSTR);

		mp->m_dev   = s.st_dev;
		mp->m_ino   = ino;
	   /***	mp->m_symtb = NOSYMTB; ***/

		if (mod_first == NOLIBMOD)
			mod_first = mp;
		else
			mod_last->m_next = mp;

		mp->m_next = NOLIBMOD;
		mod_last   = mp;
#ifdef	DEBUG
		printf ("%s,%d: ", mod_nm, ino);
#endif	DEBUG
		/*
		 *	Retira os símbolos
		 */
		symtb_first = NOSYMTB; symtb_last = NOSYMTB;

		while ((name = strfield (NOSTR, ',')) != NOSTR)
		{
#ifdef	DEBUG
			printf ("%s,", name);
#endif	DEBUG
			/*
			 *	Insere o nome na lista
			 */
			if ((zp = malloc (sizeof (SYMTB))) == NOSYMTB)
				error (NOSTR);

			if ((zp->z_sym_nm = strdup (name)) == NOSTR)
				error (NOSTR);

			zp->z_sym_len = strlen (name);
		   /***	zp->z_type = ...; ***/
		   /***	zp->z_value = ...; ***/

			if (symtb_first == NOSYMTB)
				symtb_first = zp;
			else
				symtb_last->z_next = zp;

			zp->z_next = NOSYMTB;
			symtb_last = zp;

		}	/* end retirando os símbolos

		/*
		 *	Insere a lista de símbolos no módulo
		 */
		mp->m_symtb = symtb_first;
#ifdef	DEBUG
		printf ("\n");
#endif	DEBUG

		begin_ptr = end_ptr;
	}

}	/* end reg_lib_get_LIBSYM */

/*
 ****************************************************************
 *   Verifica os módulos necessários das bibliotecas regulares	*
 ****************************************************************
 */
void
pass2 (void)
{
	const MODULE	*mod_mp;
	LIBMOD		*mp, *next_mp;
	SYMTB		*zp, *next_zp;
	GSYM		*gp;
	int		fd;
	HEADER		h;
	char		load;
#undef	SYM_PASSEs
#ifdef	SYM_PASSEs
	int		pass = 1;
#endif	SYM_PASSEs

	/*
	 *	Percorre todos os símbolos de todos os módulos de bibliotecas
	 */
	do
	{
	    load = 0;
#ifdef	SYM_PASSEs
	    printf ("sym_pass = %d\n", pass++);
#endif	SYM_PASSEs
	    for (mp = mod_first; mp != NOLIBMOD; mp = mp->m_next)
	    {
		    for (zp = mp->m_symtb; zp != NOSYMTB; zp = zp->z_next)
		    {
			/*
			 *	Verifica se o símbolo nos interessa
			 */
			if ((gp = hash (zp->z_sym_nm, zp->z_sym_len)) == NOGSYM || !IS_UNDEF (&gp->s))
				continue;

			/*
			 *	Sim, nos interessa; lê o cabeçalho do módulo
			 */
			if ((fd = inopen (mp->m_dev, mp->m_ino)) < 0)
				{ error ("*@Não consegui abrir \"%s\"", mp->m_mod_nm); continue; }

			if (read (fd, &h, sizeof (HEADER)) != sizeof (HEADER))
			{
				error ("*@Erro na leitura do cabeçalho de \"%s\"", mp->m_mod_nm);
				close (fd); continue;
			}

			if (h.h_magic != FMAGIC)
			{
				error ("@O arquivo \"%s\" não é um módulo objeto", mp->m_mod_nm);
				close (fd); continue;
			}

			mod_mp = put_module (mp->m_mod_nm, FMAGIC, mp->m_dev, mp->m_ino, &h);
#ifndef	SYM_PASSEs
			if (vflag)
#endif	SYM_PASSEs
				printf ("\tMódulo \"%s\"\n", mp->m_mod_nm);

			install_mod_symbols (mod_mp, fd, &h, 'L' /* Biblioteca regular */);

			close (fd); load = 1;

			break;	/* Vai para o próximo módulo */

		    }	/* end for (símbolos deste módulo) */

	    }	/* end for (lista de módulos) */

	}	while (load);

	/*
	 *	Libera a memória das LIBSYMs
	 */
	for (mp = mod_first; mp != NOLIBMOD; mp = next_mp)
	{
		for (zp = mp->m_symtb; zp != NOSYMTB; zp = next_zp)
		{
			free ((void *)zp->z_sym_nm);

			next_zp = zp->z_next; free (zp);
		}

		free ((void *)mp->m_mod_nm);

		next_mp = mp->m_next; free (mp);
	}

}	/* end pass2 */
