/*
 ****************************************************************
 *								*
 *			sinop_io.c				*
 *								*
 *	Entrada/saída do arquivo de sinopse			*
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
#include <stat.h>
#include <fcntl.h>
#include <signal.h>
#include <a.out.h>
#include <errno.h>

#include "../h/mklib.h"

/*
 ******	Variáveis globais ***************************************
 */
entry int		n_lib_mod;	/* No. de módulos */

entry int		n_lib_sym;	/* No. de símbolos */


/*
 ****************************************************************
 *	Le o arquivo de sinopse					*
 ****************************************************************
 */
void
read_sinop_file (int create)
{
	int		fd, ino = 0;
	STAT		s;
	char		*cp, *begin_ptr, *end_ptr;
	char		*libsym_image;
	const char	*mod_nm, *name;
	MOD		*mp;
	SYMTB		*zp;
	SYMTB		*first_sym, *last_sym;

	/*
	 *	Abre o arquivo de sinopse
	 */
	if ((fd = open (lib_sinop_nm, O_RDONLY)) < 0 || fstat (fd, &s) < 0)
	{
		if (errno == ENOENT && create)
			return;

		error ("$*Não consegui abrir \"%s\"", lib_sinop_nm);
	}

	if (!S_ISREG (s.st_mode))
		error ("$*O arquivo \"%s\" não é regular", lib_sinop_nm);

	/*
	 *	Aloca memória e lê a biblioteca
	 */
	libsym_image = alloca (s.st_size + 1); (libsym_image + s.st_size)[0] = '\0';

	if ((read (fd, libsym_image, s.st_size)) != s.st_size)
		error ("$*Erro na leitura do arquivo \"%s\"", lib_sinop_nm);

	close (fd);

	/*
	 *	Confere o número mágico
	 */
	if ((begin_ptr = strchr (libsym_image, '\n')) == NOSTR)
		error ("$Não encontrei o final da linha em \"%s\"", lib_sinop_nm);

	*begin_ptr++  = '\0';

	if (atol (libsym_image) != LIBMAGIC)
		error ("$O arquivo \"%s\" não tem cabeçalho válido", lib_sinop_nm);

	/*
	 *	Retira os módulos com os respectivos símbolos
	 */
	mod_first = mod_last = NOMOD;

	while (*begin_ptr != '\0')
	{
		if ((end_ptr = strchr (begin_ptr, '\n')) == NOSTR)
			error ("$Não encontrei o final da linha em \"%s\"", lib_sinop_nm);

		*end_ptr++ = '\0';

		if ((mod_nm = strfield (begin_ptr, ',')) == NOSTR)
			error ("$Esperava o nome de um módulo em \"%s\"", lib_sinop_nm);

		if ((cp = strfield (NOSTR, ':')) == NOSTR || (ino = atol (cp)) == 0)
			error ("$Esperava o número de um nó-índice em \"%s\"", lib_sinop_nm);

		/*
		 *	Insere o módulo na lista
		 */
		if ((mp = malloc (sizeof (MOD))) == NOMOD)
			error (NOSTR);

		if ((mp->m_mod_nm = strdup (mod_nm)) == NOSTR)
			error (NOSTR);

		mp->m_ino = ino;
	   /***	mp->m_sym = ...; ***/
		mp->m_n_sym = 0;

		if (mod_first == NOMOD)
			mod_first = mp;
		else
			mod_last->m_next = mp;

		mp->m_next = NOMOD;
		mod_last   = mp;

		n_lib_mod++;

		/*
		 *	Retira os símbolos
		 */
		first_sym = last_sym = NOSYMTB;

		while ((name = strfield (NOSTR, ',')) != NOSTR)
		{
			/*
			 *	Cria a estrutura do símbolo
			 */
			if ((zp = malloc (sizeof (SYMTB))) == NOSYMTB)
				error (NOSTR);

			if ((zp->z_sym_nm = strdup (name)) == NOSTR)
				error (NOSTR);

			zp->z_sym_len = strlen (name);
			zp->z_mod     = mp;

			if (first_sym == NOSYMTB)
				first_sym = zp;
			else
				last_sym->z_sym_next = zp;

			last_sym = zp;
			zp->z_sym_next = NOSYMTB;

		   /***	zp->z_link = NOSYMTB; ***/

			mp->m_n_sym++;
			n_lib_sym++;

		}	/* Retira os símbolos */

		mp->m_sym = first_sym;

		begin_ptr = end_ptr;
	}
	
}	/* end read_sinop_file */

/*
 ****************************************************************
 *	Escreve o arquivo .LIBSYM				*
 ****************************************************************
 */
void
write_sinop_file (void)
{
	int		fd;
	FILE		*fp;
	const MOD	*mp;
	const SYMTB	*zp;

	/*
	 *	Não aceita mais sinais
	 */
	signal (SIGINT,  SIG_IGN);
	signal (SIGQUIT, SIG_IGN);

	/*
	 *	Cria o arquivo de sinopse
	 */
	if ((fd = creat (lib_sinop_nm, 0644)) < 0)
		error ("$*Não consegui criar \"%s\"", lib_sinop_nm);

	if (chmod (lib_sinop_nm, 0644) < 0)
		error ("$*Não consegui trocar a proteção de \"%s\"", lib_sinop_nm);

	if ((fp = fdopen (fd, "w")) == NOFILE)
		error ("$*Não consegui abrir \"%s\"", lib_sinop_nm);

	/*
	 *	Escreve o cabecalho
	 */
	fprintf (fp, "%d\n", LIBMAGIC);

	/*
	 *	Escreve os módulos com os respectivos símbolos
	 */
	for (mp = mod_first; mp != NOMOD; mp = mp->m_next)
	{
		char	first = 1;

		fprintf (fp, "%s,%d:", mp->m_mod_nm, mp->m_ino);

		for (zp = mp->m_sym; zp != NOSYMTB; zp = zp->z_sym_next)
		{
			if (first)
				first = 0;
			else
				fprintf (fp, ",");

			fprintf (fp, "%s", zp->z_sym_nm);
		}

		fprintf (fp, "\n");
	}

	fclose (fp);

#ifdef	DEBUG
	if (vflag > 1)
	{
		printf
		(	"Total: %d símbolos em %d módulos\n", 
			next_lib_sym - lib_sym,
			next_lib_mod - lib_mod
		);
	}
#endif	DEBUG
	
}	/* end writelibsym */
