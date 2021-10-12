/*
 ****************************************************************
 *								*
 *			reduce1.c				*
 *								*
 *	Comprime um arquivo segundo o algoritmo LZW		*
 *								*
 *	Versão	2.3.0, de 27.08.90				*
 *		2.3.4, de 14.01.91				*
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

extern int	code_area[NC];	/* Cabem sempre NC códigos até 16 bits */

extern R_HEADER	r_header;	/* O cabeçalho */

entry CRCFILE	incrcfile;	/* Bloco para STDIN */

entry CODEFILE	outcodefile;	/* Bloco para CODEOUT */

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
 *	Comprime um arquivo segundo o algoritmo LZW		*
 ****************************************************************
 */
void
reduce (void)
{
	register int	omega, K, i, bit_shift, HASHSZ;
	register HASHTB	*hp, *hashtb;
	register CRCFILE *fp0 = &incrcfile;
	register CODEFILE *fp1 = &outcodefile;
	static int	HASHSZ_TB[] =
			{ 641, 1259, 2459, 4919, 9833, 19661, 39323, 78649 };

	/*
	 *	Inicializa os parâmetros para a compressão
	 */
	CODE_SOURCE = FIRST_CODE;

	TEST_POINT = TEST_INTERVAL;

	bit_sz = MIN_BIT_SZ;

	max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);

	MAX_CODE = BIT_SZ_TO_CODE (MAX_BIT_SZ);

	MAX_BIT_USED = MIN_BIT_SZ;
 
	HASHSZ = HASHSZ_TB[MAX_BIT_SZ - MIN_BIT_SZ];

	bit_shift = MAX_BIT_SZ - 8;

	if (dflag)
		fprintf (stderr, "MAX_BIT_SZ = %d\n", MAX_BIT_SZ);

	/*
	 *	Aloca memória para a tabela HASH 
	 */
	if ((hashtb = malloc (HASHSZ * sizeof (HASHTB))) == NOHASHTB)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alocar memória para a tabela HASH\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Inicializa a tabela HASH
	 */
	for (hp = &hashtb[HASHSZ - 1]; hp >= hashtb; hp--)
		hp->h_omega = NOCODE;

	/*
	 *	Iniciliza PUTCODE
	 */
	fp1->c_base = code_area;
	fp1->c_ptr  = fp1->c_base;
	fp1->c_bend = fp1->c_base + NC;

	/*
	 *	Escreve o cabeçalho
	 */
	r_header.h_magic = R_MAGIC;
   /***	r_header.h_size  = .....;	***/
   	r_header.h_time  = time ((time_t *)NULL);
	r_header.h_bits  = MAX_BIT_SZ;
	r_header.h_flags = (rflag ? 0 : R_CRC);
	r_header.h_crc   = crc16 (&r_header, sizeof (R_HEADER) - CRCSZ);

	fwrite (&r_header, sizeof (R_HEADER), 1, stdout);

	out_cnt = sizeof (R_HEADER);

	/*
	 *	Realiza a compressão
	 */
	omega = GETCHAR ();

	while ((K = GETCHAR ()) >= 0)	/* != EOF */
	{
		/*
		 *	Calcula a primeira tentativa HASH
		 */
		i = (K << bit_shift) ^ omega;

		hp = &hashtb[i];

		/*
		 *	Achou na tabela
		 */
		if (hp->h_omega == omega && hp->h_K == K)
			{ omega = hp->h_omega_K; continue; }

		/*
		 *	O código ainda não se encontra na tabela
		 */
		if (hp->h_omega == NOCODE)
		{
		    not_found:
			PUTCODE (omega);

			if (CODE_SOURCE < MAX_CODE)
			{
				hp->h_omega = omega;
				hp->h_K = K;
				hp->h_omega_K = CODE_SOURCE++;
			}
			elif   (in_cnt >= TEST_POINT)
			{
				analysis (hashtb, HASHSZ);
			}

		 	omega = K;

			continue;
		}
	
		/*
		 *	Colisão (supõe que a tabela nunca esteja cheia)
		 */
		if (i == 0)
			i = 1;
		else
			i = HASHSZ - i;

		for (EVER)
		{
			if ((int)(hp -= i) < (int)hashtb)
				hp += HASHSZ;

			if (hp->h_omega == omega && hp->h_K == K)
				{ omega = hp->h_omega_K; break; }

			if (hp->h_omega == NOCODE)
				goto not_found;

		}	/* end loop de colisão */

	}	/* end loop GETCHAR () */

	/*
	 *	Coloca o último código, final e dá flush
	 */
	PUTCODE (omega);

	write_max_code ();

	/*
	 *	Se a saída não for um PIPE, atualiza o cabeçalho
	 */
	if (fseek (stdout, 0, SEEK_SET) >= 0)
	{
	   	r_header.h_size = in_cnt;
		r_header.h_bits = MAX_BIT_USED;
		r_header.h_crc   = crc16 (&r_header, sizeof (R_HEADER) - CRCSZ);

		fwrite (&r_header, sizeof (R_HEADER), 1, stdout);
	}

	/*
	 *	Verifica se ocorreu um erro de saída
	 */
	if   (ferror (stdout))
	{
		fprintf
		(	stderr,
			"%s: Erro de escrita em \"%s\"\n",
			pgname, out_nm
		);
		exit_code = 1;
	}
	elif (out_cnt > in_cnt)
	{
		fprintf
		(	stderr,
			"%s: O arquivo foi EXPANDIDO ao invés de ser comprimido!\n",
			pgname
		);
		exit_code = 2;
	}

	if (vflag)
		printtotal (in_cnt, out_cnt, 1 /* com bits */);

}	/* end reduce */

/*
 ****************************************************************
 *	Analisa a performance da compressão			*
 ****************************************************************
 */
int
analysis (register HASHTB *hashtb, int HASHSZ)
{
	register HASHTB	*hp;
	register int	comp_factor;
	register CODEFILE *fp1 = &outcodefile;
	static int	old_comp_factor = 0;

	/*
	 *	Calcula o novo fator de compressão
	 */
	TEST_POINT = in_cnt + TEST_INTERVAL;

	comp_factor = getpercent (in_cnt - out_cnt, in_cnt);

	if (dflag)
	{
		fprintf
		(	stderr,
			"Ponto de análise: Entrada = %d bytes\n",
			in_cnt
		);

		fprintf
		(	stderr,
			"Fator de compressão = %s %%, ",
			editpercent (comp_factor)
		);

		fprintf
		(	stderr,
			"Fator de compressão anterior = %s %%\n",
			editpercent (old_comp_factor)
		);
	}

	/*
	 *	Compara os fatores
	 */
	if (comp_factor > old_comp_factor)
	{
		/*
		 *	Está melhorando - não mexe
		 */
		old_comp_factor = comp_factor;

		return (0);
	}
	else
	{
		/*
		 *	Piorou - recomeça de MIN_BIT_SZ bits
		 */
		for (hp = &hashtb[HASHSZ - 1]; hp >= hashtb; hp--)
			hp->h_omega = NOCODE;

		old_comp_factor = 0;

		CODE_SOURCE = FIRST_CODE;
		PUTCODE (CONTROL_CODE);

		flushcode ();
		fp1->c_ptr = fp1->c_base;

		bit_sz = MIN_BIT_SZ;
		max_code = BIT_SZ_TO_CODE (MIN_BIT_SZ);

		if (dflag)
		{
			fprintf
			(	stderr,
				"Modificando o código para %2d bits aos %d bytes\n",
				bit_sz, in_cnt
			);
		}

		return (-1);
	}

}	/* end analysis */

/*
 ****************************************************************
 *	Le um bloco do arquivo e acrescenta CRC			*
 ****************************************************************
 */
int
readblock (register CRCFILE *fp)
{
	register int	cnt, r;

	/*
	 *	Verifica se é a primeira utilização da GETC
	 */
	if (fp->i_base == NOSTR && (fp->i_base = malloc (BLSZ + CRCSZ)) == NOSTR)
	{
		fprintf
		(	stderr,
			"%s: Não consegui alocar memória para a área de entrada\n",
			pgname
		);
		exit (1);
	}

	/*
	 *	Le um registro
	 */
	for (cnt = 0; cnt < BLSZ; cnt += r)
	{
		if ((r = read (STDIN, fp->i_base + cnt, BLSZ - cnt)) < 0)
		{
			fprintf
			(	stderr,
				"%s: Erro de leitura de \"%s\"\n",
				pgname, in_nm
			);
			perror (pgname);
			exit (1);
		}

		if (r == 0)
			break;
	}

	if (cnt == 0)
		return (EOF);

	/*
	 *	Verifica se a entrada é por acaso a saída de um reduce anterior
	 */
	if (in_cnt == 0 && ((R_HEADER *)fp->i_base)->h_magic == R_MAGIC)
	{
		fprintf
		(	stderr,
			"%s: \"%s\" já está no formato comprimido. "
			"Recomprime? (n): ",
			pgname, in_nm
		);

		fflush (stderr);

		if (askyesno () <= 0)
			exit (1);
	}

	in_cnt += cnt;

	/*
	 *	Acrescenta o CRC e prepara os ponteiros
	 */
	fp->i_ptr = fp->i_base;

	fp->i_bend = fp->i_base + cnt;

	if (!rflag)
	{
		r = crc16 (fp->i_base, cnt);

		*fp->i_bend++ = r >> 8;
		*fp->i_bend++ = r;
	}

	return (*fp->i_ptr++);

}	/* end readblock */

/*
 ****************************************************************
 *	Emite um código						*
 ****************************************************************
 */
void
writecode (int code, register CODEFILE *fp)
{
	/*
	 *	Verifica se está na hora de mudar o tamanho do código
	 */
	if (CODE_SOURCE - 1 > max_code  &&  bit_sz < MAX_BIT_SZ)
	{
		bit_sz++;

		max_code = BIT_SZ_TO_CODE (bit_sz);

		if (bit_sz > MAX_BIT_USED)
			MAX_BIT_USED = bit_sz;

		if (dflag)
		{
			fprintf
			(	stderr,
				"Modificando o código para %2d bits aos %d bytes\n",
				bit_sz, in_cnt
			);
		}

	}	/* end aumentar o código */

	/*
	 *	Esvazia a área e prepara para nova área
	 */
	flushcode ();

	fp->c_ptr = fp->c_base;

	*fp->c_ptr++ = code;

}	/* end writecode */

/*
 ****************************************************************
 *	Emite o código de EOF (Não é simples)			*
 ****************************************************************
 */
void
write_max_code (void)
{
	register CODEFILE *fp1 = &outcodefile;

	/*
	 *	Arredondamento para sincronizar com a expansão
	 */
	CODE_SOURCE = ((CODE_SOURCE + (NC - 1)) / NC * NC) + 1;

	/*
	 *	Escreve o buffer, se estiver cheio
	 */
	if (fp1->c_ptr >= fp1->c_bend)
	{
		writecode (0, fp1);

		fp1->c_ptr = fp1->c_base;	/* retira o "0" */

		CODE_SOURCE += NC;
	}

	/*
	 *	Verifica se está na hora de mudar o tamanho do código
	 */
	if (CODE_SOURCE - 1 > max_code  &&  bit_sz < MAX_BIT_SZ)
	{
		bit_sz++;

		max_code = BIT_SZ_TO_CODE (bit_sz);

		if (bit_sz > MAX_BIT_USED)
			MAX_BIT_USED = bit_sz;

		if (dflag)
		{
			fprintf
			(	stderr,
				"Modificando o código para %2d bits aos %d bytes\n",
				bit_sz, in_cnt
			);
		}

	}	/* end aumentar o código */

	*fp1->c_ptr++ = max_code;

	flushcode ();

}	/* end write_max_code */
