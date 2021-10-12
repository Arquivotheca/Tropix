/*
 ****************************************************************
 *								*
 *			getinfo.c				*
 *								*
 *	Leitura de informações sobre terminais			*
 *								*
 *	Versão	2.0.0, de 21.04.87				*
 *		3.0.0, de 28.03.95				*
 *								*
 *	Modulo: getinfo						*
 *		libcurses/terminfo				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <errno.h>
#include <terminfo.h>

#include "../h/defs.h"

/*
 ****************************************************************
 *	Definição de constantes					*
 ****************************************************************
 */
#define	MAGIC	0x011B		/* Magic Number */

static const char TERMINFO[] =	"/usr/lib/terminfo";

/*
 ******	Protótipos de funções ***********************************
 */
static int	getshort (FILE *);

/*
 ****************************************************************
 *	Leitura	da "terminfo"					*
 ****************************************************************
 */
int
getinfo (const char *term, register INFO *ip)
{
	register int	i;
	register FILE	*fp;
	const char	*dir;
	int		namesz;		/* Tamanho da Seção Names */
	short		*offsets;	/* Área para a Seção Offsets */
	char		info_nm[512];	/* Nome do Arq. com Terminfo */

	/*
	 *	Se o nome não for dado, utiliza "$TERM"
	 */
	if (term == NOSTR && (term = getenv ("TERM")) == NOSTR)
	{
		fprintf
		(	stderr,
			"terminfo: Não consegui obter o nome do terminal\n"
		);
		return (0);
	}

	/*
	 *	Nome do diretório, se presente usa "$TERMINFO"
	 */
	if ((dir = getenv ("TERMINFO")) == NOSTR)
		strcpy (info_nm, TERMINFO);
	else
		strcpy (info_nm, dir);

	/*
	 *	Monta o nome completo: "dir/term"
	 */
	strcat (info_nm, "/");
	strcat (info_nm, term); 
	
	/*
	 *	Abre o arquivo com o banco de dados
	 */
	if ((fp = fopen (info_nm, "r")) == NOFILE)
	{
		fprintf
		(	stderr,
			"terminfo: Não consegui abrir \"%s\" (%s)\n",
			info_nm, strerror (errno)
		);
		return (0);
	}
#ifdef	DEBUG
	else
	{
		printf ("\nLendo \"%s\"\n", info_nm);
	}
#endif	DEBUG

	/*
	 *	Leitura do cabeçalho
	 */
	if (getshort (fp) != MAGIC)
	{
		fprintf
		(	stderr,
			"terminfo: A descrição do terminal \"%s\" "
			"contém um número mágico inválido\n",
			info_nm
		); 

		fclose (fp);
		return (0);
	}

	if
	(	(namesz		= getshort (fp)) < 0	||
		(ip->i_boolno	= getshort (fp)) < 0	||
		(ip->i_numno	= getshort (fp)) < 0 	||
		(ip->i_strno	= getshort (fp)) < 0 	||
		(ip->i_tabsz	= getshort (fp)) < 0 
	)
	{
		fprintf
		(	stderr,
			"terminfo: Descrição do terminal \"%s\" inconsistente\n",
			info_nm
		); 

		fclose (fp);
		return (0);
	}

#ifdef	DEBUG
	printf
	(	"HEADER: %d, %d, %d, %d, %d, %d\n", MAGIC,
		namesz, ip->i_boolno, ip->i_numno,
		ip->i_strno, ip->i_tabsz
	);
#endif	DEBUG


	/*
	 *	Aloca memória para booleans, numbers, strings e offsets
	 */
	if
	(	(ip->i_booleans	= malloc (ip->i_boolno)) == NOSTR ||
		(ip->i_numbers	= malloc (ip->i_numno * sizeof (short))) == (short *)NULL ||
		(ip->i_strings	= malloc (ip->i_strno * sizeof (char *))) == (char **)NULL ||
		(offsets	= malloc (ip->i_strno * sizeof (short))) == (short *)NULL ||
		(ip->i_table	= malloc (ip->i_tabsz)) == NOSTR ||
		(ip->i_names	= malloc (namesz)) == NOSTR
	)
	{
		fprintf
		(	stderr,
			"terminfo: Memória esgotada\n"
		); 

		fclose (fp);
		return (0);
	}

	/*
	 * Leitura das seções "names", "booleans", "numbers", "offsets" e "string"
	 */
#ifdef	DEBUG
	printf ("Início de names: %d\n", ftell (fp));
#endif	DEBUG

	fread (ip->i_names, namesz, sizeof (char), fp);

	if (namesz & 1)
		getc (fp);

#ifdef	DEBUG
	printf ("Início de Booleans: %d\n", ftell (fp));
#endif	DEBUG

	fread (ip->i_booleans, ip->i_boolno, sizeof (char), fp);

#ifdef	DEBUG
	printf ("Início de Numbers: %d\n", ftell (fp));
#endif	DEBUG

	fread (ip->i_numbers, ip->i_numno, sizeof (short), fp);

#ifdef	DEBUG
	printf ("Início de Offsets: %d\n", ftell (fp));
#endif	DEBUG

	fread (offsets, ip->i_strno, sizeof (short), fp);

#ifdef	DEBUG
	printf ("Início da String Table: %d\n", ftell (fp));
#endif	DEBUG

	fread (ip->i_table, ip->i_tabsz, sizeof (char), fp);

	/*
	 *	Encerra a leitura
	 */
	if (ferror (fp))
	{
		fprintf
		(	stderr,
			"terminfo: Erro na leitura de \"%s\" (%s)\n",
			info_nm, strerror (errno)
		);

		fclose (fp);
		return (0);
	}

	fclose (fp);

	/*
	 *	Conversão offsets => i_strings
	 */
	for (i = 0; i < ip->i_strno; i++)
	{
		if (offsets[i] >= 0)
			ip->i_strings[i] = ip->i_table + offsets[i];
		else
			ip->i_strings[i] = NOSTR;
	}

	/*
	 *	Libera offsets
	 */
	free (offsets);

	return (1);

}	/* end getinfo */

/*
 ****************************************************************
 *	Leitura de um "short"					*
 ****************************************************************
 */
static int
getshort (FILE *fp)
{
	short		n;

	if (fread (&n, sizeof (short), 1, fp) != 1)
		return (-1);

	return (n);

}	/* end getshort */

