/*
 ****************************************************************
 *								*
 *			prsym.c					*
 *								*
 *	Imprime a tabela de símbolos e tabela de relocação de	*
 *	um módulo objeto					*
 *								*
 *	Versão	1.0.0, de 30.04.86				*
 *		4.2.0, de 08.05.02				*
 *								*
 *	Módulo: ld & as						*
 *		Utilitários Básicos				*
 *		Categoria D					*
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
#include <string.h>
#include <a.out.h>
#include <fcntl.h>
#include <errno.h>

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
const char	pgversion[] = "Versão: 4.2.0, de 08.05.02";

#define	elif	else if

int		vflag;		/* Verbose */

/*
 ******	Protótipos de funções ***********************************
 */
void		prsym (const char *);
void		prreloc (void);
void		help (void);

/*
 ****************************************************************
 *	Programa de imprimir os símbolos e relocação		*
 ****************************************************************
 */
int
main (int argc, const char *argv[])
{
	int		opt, fd, rel_size, ref_size;
	const char	*file_nm = NOSTR;
	HEADER		h;
	const SYM	*sp;
	const RELOC	*rp;
	void		*symtb_image, *rel_image = NOVOID, *ref_image = NOVOID;
	const SYM	*end_symtb_image;
	const SYM	**vp;
	const RELOC	*end_rel_image = NOVOID;
	const EXTREF	*erp, *end_ref_image = NOVOID;
	const SYM	**vector_image = (const SYM **)NULL;
	int		vector_elem = 0;
	int		text_reloc_entries, reloc_entry_no;
	int		*ref_count_vec;
	int		index, var_total = 0, ref_total = 0;
	char		time_str[80];

	/*
	 *	Analisa as opções.
	 */
	while ((opt = getopt (argc, argv, "vH")) != EOF)
	{
		switch (opt)
		{
		    case 'v':			/* Verbose */
			vflag++;
			break;

		    case 'H':			/* Help */
			help ();

		    default:			/* Erro */
			fprintf (stderr, "\n");
			help ();
		}
	}

	argv += optind;
	argc -= optind;

	/*
	 *	Examina os argumentos
	 */
	if   (argc == 0)
		file_nm = "a.out";
	elif (argc == 1)
		file_nm = argv[0];
	else
		help ();

	if ((fd = open (file_nm, O_RDONLY)) < 0)
		error ("$*Não consegui abrir \"%s\"", file_nm);

	/*
	 *	Le o cabeçalho
	 */
	if (read (fd, &h, sizeof (HEADER)) != sizeof (HEADER))
		error ("$*Erro na leitura do cabeçalho de \"%s\"", file_nm);

	if (h.h_magic != FMAGIC && h.h_magic != NMAGIC && h.h_magic != SMAGIC)
		error ("$\"%s\" NÃO é um módulo objeto", file_nm);

	/*
	 *	Le a tabela de símbolos
	 */
	if (h.h_ssize == 0)
		error ("$Módulo objeto \"%s\" sem símbolos?", file_nm);

	symtb_image = alloca (h.h_ssize);

	lseek (fd, h.h_tsize + h.h_dsize, SEEK_CUR);

	if (read (fd, symtb_image, h.h_ssize) != h.h_ssize)
		error ("$*Erro na leitura da tabela de símbolos de \"%s\"", file_nm);

	end_symtb_image = symtb_image + h.h_ssize;

	/*
	 *	Prepara o vetor de ponteiros para a SYMTB
	 */
	for (sp = symtb_image; sp < end_symtb_image; sp = SYM_NEXT_PTR (sp))
		vector_elem++;

	vector_image = alloca (vector_elem * sizeof (SYM *));

	ref_count_vec = alloca (vector_elem * sizeof (int));

	memset (ref_count_vec, 0, vector_elem * sizeof (int));

	/*
	 *	Imprime a Tabela de símbolos
	 */
	printf ("----------------------------- SYMTB --------------------------------\n");
	printf ("TYPE  LOCAL LABEL EXTERN REF --VALUE--   --------------- ID ------------\n\n");

	for
	(	vp = vector_image, sp = symtb_image;
		sp < end_symtb_image;
		vp++, sp = SYM_NEXT_PTR (sp)
	)
	{
		vp[0] = sp;

		if ((sp->s_type & EXTERN) == 0)
			continue;

		switch (sp->s_type & ~EXTERN)
		{
		    case UNDEF:
			printf ("UNDEF");
			break;

		    case ABS:
			printf ("ABS  ");
			break;

		    case TEXT:
			printf ("TEXT ");
			break;

		    case DATA:
			printf ("DATA ");
			break;

		    case BSS:
			printf ("BSS  ");
			break;

		    default:
			printf ("???? ");
		}

		printf
		(	"   %c     %c     %c     %c  %P", 
			sp->s_flags & S_LOCAL ?  'L' : ' ',
			sp->s_flags & S_LBL ?    'R' : ' ',
			sp->s_flags & S_EXTERN ? 'E' : ' ',
			sp->s_flags & S_REF ?	 '*' : ' ',
			sp->s_value
		);

		printf ("   %s\n", sp->s_name);
	}

	/*
	 *	Le a relocação
	 */
#if (0)	/*******************************************************/
	if ((rel_size = h.h_rtsize + h.h_rdsize) == 0)
		exit (0);
#endif	/*******************************************************/

	if ((rel_size = h.h_rtsize + h.h_rdsize) != 0)
	{
		rel_image = alloca (rel_size);

	   /***	lseek (fd, ..., ...); ***/

		if (read (fd, rel_image, rel_size) != rel_size)
			error ("$*Erro na leitura da tabela de símbolos de \"%s\"", file_nm);

		end_rel_image = rel_image + rel_size;

#if (0)	/*******************************************************/
		close (fd);
#endif	/*******************************************************/
	}

	/*
	 *	Imprime a relocação
	 */
	text_reloc_entries = h.h_rtsize / sizeof (RELOC); reloc_entry_no = -1;

	printf ("\n\n");
	printf ("---------------------- RELOC --------------------------\n");
	printf ("SEGMENT SIZE -OFFSET--  --------------ID---------------\n\n");

	for (rp = rel_image; rp < end_rel_image; rp++)
	{
		if (++reloc_entry_no == text_reloc_entries)
			printf ("\n");

		switch (rp->r_flags & RSEGMASK)
		{
		    case RTEXT:
			printf ("TEXT  ");
			break;

		    case RDATA:
			printf ("DATA  ");
			break;

		    case RBSS:
			printf ("BSS   ");
			break;

		    case REXT:
			printf ("EXTERN");
			break;

		    case REXTREL:
			printf ("EXTREL");
			break;

		    default:
			printf ("????  ");
		}

		printf ("  ");

		switch (rp->r_flags & RSZMASK)
		{
		    case RBYTE:
			printf ("BYTE");
			break;

		    case RWORD:
			printf ("WORD");
			break;

		    case RLONG:
			printf ("LONG");
			break;

		    default:
			printf ("????");
		}

		if (reloc_entry_no >= text_reloc_entries)
			printf (" %P", rp->r_pos + h.h_tsize);
		else
			printf (" %P", rp->r_pos);

		if ((rp->r_flags & RSEGMASK) == REXT || (rp->r_flags & RSEGMASK) == REXTREL)
		{
			index = rp->r_symbol;

			if ((unsigned)index >= vector_elem)
			{
				printf ("  [Símbolo Errado]");
			}
			else
			{
				sp = vector_image[index];
				printf ("  %s", sp->s_name);

				ref_count_vec[index]++;
			}
		}
		elif (rp->r_symbol != 0)
		{
			printf (" RSYMBOL errado");
		}

		if (rp->r_flags & ~(RSEGMASK|RSZMASK))
			printf (" R_FLAGS com bits inválidos: %04X", rp->r_flags);

		putc ('\n', stdout);

	}

	/*
	 *	Processa as referências externas
	 */
printf ("refsize = %d\n", h.h_refsize);
	if ((ref_size = h.h_refsize) != 0)
	{
		ref_image = alloca (ref_size);

	   /***	lseek (fd, ..., ...); ***/

		if (read (fd, ref_image, ref_size) != ref_size)
			error ("$*Erro na leitura da tabela de símbolos de \"%s\"", file_nm);

		end_ref_image = ref_image + ref_size;
	}

	for (erp = ref_image; erp < end_ref_image; /* abaixo */)
	{
		const off_t	*op;

		printf ("%14.14s: ", erp->r_name);

		op = EXTREF_REFPTR (erp);

		for (index = 0; index < erp->r_ref_len; index++)
			printf (" %P", *op++);

		printf ("\n");

		erp = (EXTREF *)op;
	}
	

	/*
	 *	Imprime a estatística
	 */
	printf ("\n\n");
	printf ("--REF-- --------------ID---------------\n\n");

	for (index = 0; index < vector_elem; index++)
	{
		if (ref_count_vec[index] == 0)
			continue;

		var_total++; ref_total += ref_count_vec[index];

		sp = vector_image[index];

		printf ("%7d %s\n", ref_count_vec[index], sp->s_name);
	}

	printf ("\nvar_total = %d, ref_total = %d\n", var_total, ref_total);

#if (0)	/*******************************************************/
	/*
	 *	Mede o tempo do hash
	 */
	clockstamp ();

	for (index = 0; index < vector_elem; index++)
	{
		if (ref_count_vec[index] == 0)
			continue;

		sp = vector_image[index];

		for (fd = 0; fd < 1000000; fd++)
			strhash (sp->s_name, sp->s_nm_len, 1009);
	}

	printf ("%s\n", clocktostr (time_str));
#endif	/*******************************************************/

	close (fd);

	return (0);

}	/* end main */

/*
 ****************************************************************
 *	Resumo de utilização do programa			*
 ****************************************************************
 */
void
help (void)
{
	fprintf
	(	stderr,
		"%s - imprime as tabelas de símbolos e relocação de um módulo objeto\n"
		"\n%s\n"
		"\nSintaxe:\n"
		"\t%s [<objeto>]\n",
		pgname, pgversion, pgname
	);
	fprintf
	(	stderr,
		"\nObs.:\tse <objeto> não for dado, será usado \"a.out\"\n"
	);

	exit (2);

}	/* end help */
