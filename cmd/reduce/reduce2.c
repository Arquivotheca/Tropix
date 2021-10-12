/*
 ****************************************************************
 *								*
 *			reduce2.c				*
 *								*
 *	Comprime um arquivo segundo o algoritmo LZW		*
 *								*
 *	Versão	2.3.0, de 27.08.90				*
 *		2.3.3, de 12.12.90				*
 *								*
 *	Módulo: reduce						*
 *		Utilitários Básicos				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
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

#include "reduce.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern int	exit_code;	/* Código de retorno */

extern int	rflag;		/* Não gera/confere CRCs */
extern int	uflag;		/* Uncompress */
extern int	fflag;		/* Force */
extern int	vflag;		/* Verbose */
extern int	tflag;		/* Tabela */
extern int	dflag;		/* Debug */
extern int	cflag;		/* Imprime os códigos */

extern long	in_cnt, out_cnt; /* Total de caracteres de entrada/saída */

extern char	*in_nm, *out_nm; /* Nomes dos arquivos */

extern int	code_area[NC];	/* Cabem sempre 8 códigos ainda de 16 bits */

extern R_HEADER	r_header;	/* O cabeçalho */

entry CRCFILE	outcrcfile;	/* Bloco para STDOUT */

entry CODEFILE	incodefile;	/* Bloco para CODEIN */

/*
 ****** Parâmetros de compressão ********************************
 */
extern int	MAX_BIT_SZ;	/* O algoritmo funciona no máximo com 16 bits */ 

extern int	MAX_BIT_USED;	/* Número de bits realmente usados */ 

extern int	MAX_CODE;	/* Nunca pode usar este código */

extern int	CODE_SOURCE;	/* Fonte de novos códigos */

extern int	TEST_POINT;	/* Próximo teste de performance */

extern int	bit_sz;		/* Tamanho atual */

extern int	max_code;	/* Último código do tamanho atual */

/*
 ****************************************************************
 *	Expande um arquivo segundo o algoritmo LZW		*
 ****************************************************************
 */
void
unreduce (void)
{
	register int	K, omega_K, old_omega_K, orig_omega_K;
	register CODETB	*hp, *codetb;
	register char	*sp;
	register CRCFILE *fp1 = &outcrcfile;
	register CODEFILE *fp0 = &incodefile;
	R_HEADER	header;
	STAT		s;
	int		CODESZ;
	char		STACK[7000];

	/*
	 *	Le e analisa o cabeçalho
	 */
	fread (&header, sizeof (R_HEADER), 1, stdin);

	if (header.h_magic != R_MAGIC)
	{
		fprintf
		(	stderr,
			"%s: O arquivo de entrada \"%s\" não está no formato "
			"comprimido\n",
			pgname, in_nm
		);
		exit (1);
	}

	if (header.h_crc != crc16 (&header, sizeof (R_HEADER) - CRCSZ))
	{
		fprintf
		(	stderr,
			"%s: Erro de CRC no cabeçalho de \"%s\"\n",
			pgname, in_nm
		);
		exit (1);
	}

	/*
	 *	Tenta obter algumas informações
	 */
	if (vflag)
	{
		fprintf
		(	stderr,
			"Arquivo comprimido em %.24s, %d bits, %s CRCs\n",
			btime (&header.h_time), header.h_bits,
			header.h_flags & R_CRC ? "com" : "sem"
		);

		if (fstat (STDIN, &s) >= 0 && s.st_size && header.h_size)
			printtotal (header.h_size, s.st_size, 0 /* sem bits */);

	} 	/* end vflag */

	/*
	 *	Inicializa os parâmetros da expansão
	 */
	CODE_SOURCE = FIRST_CODE;

	MAX_BIT_SZ = header.h_bits;

	if ((header.h_flags & R_CRC) == 0)
		rflag++;

	MAX_CODE = BIT_SZ_TO_CODE (MAX_BIT_SZ);

	CODESZ = (1 << MAX_BIT_SZ); 

	bit_sz = MIN_BIT_SZ;

	max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);

	/*
	 *	Aloca memória para a tabela CODE 
	 */
	if ((codetb = malloc (CODESZ * sizeof (CODETB))) == NOCODETB)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alocar memória para a tabela CODE\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Realiza a expansão
	 */
	K = old_omega_K = GETCODE ();

	if (old_omega_K == max_code)
		return;

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

			if (dflag)
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
				fprintf
				(	stderr,
					"%s: ***** ERRO: código inválido: "
					"(%d, %d)\n",
					pgname, omega_K, MAX_CODE
				);
				exit (1);
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

	if (dflag)
		fprintf (stderr, "Último código gerado = %d\n", CODE_SOURCE - 1);

	if (tflag)
		printtable (codetb, CODE_SOURCE - 1);

}	/* end unreduce */

/*
 ****************************************************************
 *	Le um bloco de códigos 					*
 ****************************************************************
 */
int
readcode (register CODEFILE *fp)
{
	register int	r;

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

		if (dflag)
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
		fprintf
		(	stderr,
			"%s: EOF prematuro de \"%s\"\n",
			pgname, in_nm
		);
		return (max_code);
	}

	fp->c_ptr = fp->c_base;

	fp->c_bend = fp->c_base + r;

	return (*fp->c_ptr++);

}	/* end readcode */

/*
 ****************************************************************
 *	Confere o CRC e escreve um bloco do arquivo 		*
 ****************************************************************
 */
void
writeblock (unsigned char c, register CRCFILE *fp)
{
	register int	n, rn;

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
			fprintf
			(	stderr,
				"%s: Não consegui alocar memória "
				"para a área de saída\n",
				pgname
			);
			exit (1);
		}

		fp->i_bend = fp->i_base + BLSZ;

		if (!rflag)
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
		if (!rflag)
		{
			rn  =  (int)*--fp->i_ptr;
			rn |= ((int)*--fp->i_ptr << 8);

			n -= CRCSZ;

			if (rn != crc16 (fp->i_base, n))
			{
				fprintf
				(	stderr,
					"%s: ***** Erro de CRC aos %d bytes\n",
					pgname, out_cnt
				);
			}
		}

	}

	/*
	 *   Escreve no arquivo
	 */
	if (n > 0)
		rn = write (STDOUT, fp->i_base, n);
	else
		rn = 0;

	fp->i_ptr = fp->i_base;

	*fp->i_ptr++ = c;

	/*
	 *	Erro na escrita
	 */
	if (rn != n)
	{
		fprintf
		(	stderr,
			"%s: Erro de escrita em \"%s\"\n",
			pgname, out_nm
		);
		perror (pgname);
		exit (1);
	}

	out_cnt += n;

}	/* end writeblock */
