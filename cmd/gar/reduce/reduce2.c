/*
 ****************************************************************
 *								*
 *			reduce2.c				*
 *								*
 *	Comprime um arquivo segundo o algoritmo LZW		*
 *								*
 *	Versão	2.3.00, de 27.08.90				*
 *		3.1.06, de 04.05.97				*
 *								*
 *	Módulo: GAR						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stat.h>
#include <stdlib.h>
#include <time.h>

#include "../h/common.h"
#include "../h/gar.h"
#include "../h/reduce.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
entry void	(*function) ();	/* Função a executar para cada bloco */

entry CRCFILE	outcrcfile;	/* Bloco para STDOUT */

entry CODEFILE	incodefile;	/* Bloco para CODEIN */

entry COLFILE	incolfile;	/* COLFILE da coleção */

entry off_t	col_size;	/* Tamanho do arquivo comprimido na coleção */ 

entry int	file_block;	/* No. do bloco do arquivo na coleção */ 

/*
 ****************************************************************
 *	Expande um arquivo segundo o algoritmo LZW		*
 ****************************************************************
 */
off_t
unreduce (void (*f) ())
{
	int		K, omega_K, old_omega_K, orig_omega_K;
	CODETB		*hp, *codetb;
	char		*sp;
	CRCFILE		*fp1 = &outcrcfile;
	CODEFILE	*fp0 = &incodefile;
	int		CODESZ;
#if (0)	/*************************************/
	static int	old_CODESZ = 0;
	static CODETB	*old_codetb;
#endif	/*************************************/
	char		STACK[7000];

	/*
	 *	Inicializa os parâmetros da expansão
	 */
	CODE_SOURCE = FIRST_CODE;

   /***	MAX_BIT_SZ = ......;	/* Atribuído em in.c */

   /***	crc_flag = ......;	/* Atribuído em in.c */

	MAX_CODE = BIT_SZ_TO_CODE (MAX_BIT_SZ);

	CODESZ = (1 << MAX_BIT_SZ); 

	bit_sz = MIN_BIT_SZ;

	max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);

	function = f;

	out_cnt = 0;

	incolfile.i_ptr = incolfile.i_bend;

	col_size = khead.k_size;

	file_block = 0;

	outcrcfile.i_ptr = outcrcfile.i_base;

	fp0->c_ptr = fp0->c_bend;	/* Abandona */

	if (Gflag)
	{
		fprintf
		(	stderr,
			"MAX_BIT_SZ = %d, CODESZ = %d, codetbsz = %d\n", 
			MAX_BIT_SZ, CODESZ, CODESZ * sizeof (CODETB)
		);
	}

	/*
	 *	Aloca memória para a tabela CODE
	 */
	if ((codetb = malloc (CODESZ * sizeof (CODETB))) == NOCODETB)
	{
		msg ("Não consegui alocar memória para a tabela CODE");
		quit (1);
	}

#if (0)	/*************************************/
	codetb = alloca (CODESZ * sizeof (CODETB));

	codetb[0].h_omega = 0;		/* Aloca a memória na STACK */
#endif	/*************************************/

#if (0)	/*************************************/
	if (CODESZ > old_CODESZ)
	{
		if (old_CODESZ != 0)
			free (old_codetb);

		if ((codetb = malloc (CODESZ * sizeof (CODETB))) == NOCODETB)
		{
			msg ("Não consegui alocar memória para a tabela CODE");
			quit (1);
		}

		old_codetb = codetb;	  old_CODESZ = CODESZ;
	}
	else
	{
		codetb = old_codetb;
	}
#endif	/*************************************/

	/*
	 *	Realiza a expansão
	 */
	K = old_omega_K = GETCODE ();

	if (old_omega_K == max_code)
	{
	   	free (codetb);
		return (out_cnt);
	}

	PUTCHAR (K);

	sp = STACK;

	while ((omega_K = GETCODE ()) != max_code)
	{
		/*
		 *	Verifica se é o código de controle
		 */
		if (omega_K == CONTROL_CODE)
		{
			fp0->c_ptr = fp0->c_bend;	/* Abandona */

			bit_sz = MIN_BIT_SZ;
			max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);

			if (Gflag)
			{
				fprintf
				(	stderr,
					"Modificando o código para %2d bits\n",
					bit_sz
				);
			}

			CODE_SOURCE = FIRST_CODE - 1;

			if ((omega_K = GETCODE ()) == max_code)
				break;
		}

		orig_omega_K = omega_K;

		/*
		 *	Caso especial (KwKwK)
		 */
		if (omega_K >= CODE_SOURCE)
		{
			if (omega_K >= MAX_CODE)
			{
				msg
				(	"***** ERRO: código inválido: "
					"(%d, %d)\n",
					omega_K, MAX_CODE
				);
				quit (1);
			}

			*sp++ = K;
			omega_K = old_omega_K;
		}

		/*
		 *	Decodifica e empilha os caracteres
		 */
		while (omega_K >= FIRST_CODE) 
		{
			hp = &codetb[omega_K];

			*sp++ = hp->h_K;
			omega_K = hp->h_omega;
		}

		K = omega_K;	*sp++ = K;

		/*
		 *	Emite os caracteres da pilha
		 */
		while (sp > STACK)
			PUTCHAR (*--sp);

		/*
		 *	Insere a cadeia omega_K nova na tabela
		 */
		if (CODE_SOURCE < MAX_CODE)
		{
			hp = &codetb[CODE_SOURCE++];

			hp->h_K = K;
			hp->h_omega = old_omega_K;
		}

		old_omega_K = orig_omega_K;

	}	/* end loop GETCODE */

	/*
	 *	Emite o último bloco (flush)
	 */
	writeblock (0, fp1);

   	free (codetb);

	return (out_cnt);

}	/* end unreduce */

/*
 ****************************************************************
 *	Le um bloco de códigos 					*
 ****************************************************************
 */
int
readcode (CODEFILE *fp)
{
	int		r;

	/*
	 *	Verifica se é a primeira utilização da GETCODE
	 */
	if (fp->c_base == NOCODEP)
		fp->c_base = code_area;

	/*
	 *	Verifica se deve mudar o tamanho do código
	 */
	if (CODE_SOURCE + NC > max_code  &&  bit_sz < MAX_BIT_SZ)
	{
		bit_sz++;

		max_code = BIT_SZ_TO_CODE (bit_sz);

		if (Gflag)
		{
			fprintf
			(	stderr,
				"Modificando o código para %2d bits\n",
				bit_sz
			);
		}
	}

	/*
	 *	Le um registro
	 */
	if ((r = fillcode ()) <= 0)
	{
		msg ("EOF prematuro do arquivo compactado");
		return (max_code);
	}

	fp->c_ptr = fp->c_base;

	fp->c_bend = fp->c_base + r;

	return (*fp->c_ptr++);

}	/* end readcode */

/*
 ****************************************************************
 *	Le um bloco da coleção e confere o CRC			*
 ****************************************************************
 */
int
readcol (COLFILE *fp)
{
	int		cnt;
	char		crc[8];

	/*
	 *	Verifica se é a primeira utilização da GETC
	 */
	if (fp->i_base == NOSTR && (fp->i_base = malloc (BLSZ + CRCSZ)) == NOSTR)
	{
		msg ("Não consegui alocar memória para a área de entrada");
		quit (1);
	}

	/*
	 *	Le um bloco
	 */
	cnt = MIN (col_size, BLSZ);

	bread (fp->i_base, cnt);

	col_size -= cnt;

	bread (crc, sizeof (crc) );

	if (atoi (crc) != crc16 (fp->i_base, cnt) )
	{
		msg ("Erro de crc no bloco %d", file_block);
	}

	file_block++;

   /***	in_cnt += cnt; ***/

	/*
	 *	Prepara os ponteiros
	 */
	fp->i_ptr = fp->i_base;

	fp->i_bend = fp->i_base + cnt;

	return (*fp->i_ptr++);

}	/* end readcol */

/*
 ****************************************************************
 *	Confere o CRC e escreve um bloco do arquivo 		*
 ****************************************************************
 */
void
writeblock (unsigned char c, CRCFILE *fp)
{
	int		n, rn;

	/*
	 *	Verifica se é a primeira utilização da putc
	 */
	if (fp->i_base == NOSTR)
	{
		/*
		 *	A primeira utilização da putc
		 */
		if ((fp->i_base = malloc (BLSZ + CRCSZ)) == NOSTR)
		{
			msg ("Não consegui alocar memória para a área de saída");
			quit (1);
		}

		fp->i_bend = fp->i_base + BLSZ;

		if (!crcflag)
			fp->i_bend += CRCSZ;

		fp->i_ptr = fp->i_base;

		n = 0;
	}
	else
	{
		n = fp->i_ptr - fp->i_base;

		/*
		 *	Verifica o CRC
		 */
		if (!crcflag)
		{
			rn  =  (int)*--fp->i_ptr;
			rn |= ((int)*--fp->i_ptr << 8);

			n -= CRCSZ;

			if (rn != crc16 (fp->i_base, n))
				msg ("***** Erro de CRC aos %d bytes\n", out_cnt);
		}

	}

	/*
	 *   Processa o bloco
	 */
	if (n > 0)
		(*function) (fp->i_base, n);

	fp->i_ptr = fp->i_base;

	*fp->i_ptr++ = c;

	out_cnt += n;

}	/* end writeblock */
