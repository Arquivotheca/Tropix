/*
 ****************************************************************
 *								*
 *			reduce1.c				*
 *								*
 *	Comprime um arquivo segundo o algoritmo LZW		*
 *								*
 *	Versão	2.3.0, de 13.11.90				*
 *		3.0.0, de 14.05.95				*
 *								*
 *	Módulo: TCMPTO_C/TNET					*
 *		Utilitários de comunicações			*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <sys/types.h>
#include <sys/syscall.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../reduce.h"

/*
 ****************************************************************
 *	Variáveis e Definições globais				*
 ****************************************************************
 */
extern int	Bflag;		/* Debug */

#undef	CRC
#ifdef	CRC
extern int	crcflag;	/* Não gera/confere CRCs */
#endif	CRC

extern long	in_cnt, out_cnt; /* Total de caracteres de entrada/saída */

extern int	in_fd;		/* Descritor do arquivo */

extern int	code_area[NC];	/* Cabem sempre NC códigos até 16 bits */

entry CRCFILE	incrcfile;	/* Bloco para STDIN */

entry CODEFILE	outcodefile;	/* Bloco para CODEOUT */

entry COLFILE	outcolfile;	/* Bloco para TRANSMISSÃO */

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
off_t
reduce (int file_fd, char *reduce_area)
{
	register int	omega, K, i, bit_shift, HASHSZ;
	register HASHTB	*hp, *hashtb;
	register CRCFILE *fp0 = &incrcfile;
	register CODEFILE *fp1 = &outcodefile;
	static HASHTB	*static_hashtb;
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

	in_fd = file_fd;

	/*
	 *	Aloca memória para a tabela HASH 
	 */
	if ((hashtb = static_hashtb) == NOHASHTB)
		static_hashtb = malloc (HASHSZ * sizeof (HASHTB));

	if ((hashtb = static_hashtb) == NOHASHTB)
		error ("$Não consegui alocar memória para a tabela HASH");

	/*
	 *	Inicializa a tabela HASH
	 */
	for (hp = &hashtb[HASHSZ - 1]; hp >= hashtb; hp--)
		hp->h_omega = NOCODE;

	/*
	 *	Inicializa PUTCODE
	 */
	fp1->c_base = code_area;
	fp1->c_ptr  = fp1->c_base;
	fp1->c_bend = fp1->c_base + NC;

	/*
	 *	Inicializa PUTC
	 */
	outcolfile.i_base = reduce_area;
	outcolfile.i_ptr  = reduce_area;
	outcolfile.i_bend = reduce_area + BLSZ;

	/*
	 *	Inicializa GETCHAR
	 */
	fp0->i_ptr  = fp0->i_bend;

	out_cnt = 0;	in_cnt = 0;

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

	return (out_cnt);

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

	if (Bflag)
	{
		error
		(	"Ponto de análise: Entrada = %d bytes",
			in_cnt
		);

		error
		(	"Fator de compressão = %s %%, ",
			editpercent (comp_factor)
		);

		error
		(	"Fator de compressão anterior = %s %%",
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

		if (Bflag)
		{
			error
			(	"Modificando o código para %2d bits aos %d bytes",
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
		error ("$Não consegui alocar memória para a área de entrada");

	/*
	 *	Le um registro
	 */
	for (cnt = 0; cnt < BLSZ; cnt += r)
	{
		if ((r = read (in_fd, fp->i_base + cnt, BLSZ - cnt)) < 0)
			error ("$*Erro de leitura do arquivo");

		if (r == 0)
			break;
	}

	if (cnt == 0)
		return (EOF);

	in_cnt += cnt;

	/*
	 *	Acrescenta o CRC e prepara os ponteiros
	 */
	fp->i_ptr = fp->i_base;

	fp->i_bend = fp->i_base + cnt;

#ifdef	CRC
	if (!crcflag)
	{
		r = crc16 (fp->i_base, cnt);

		*fp->i_bend++ = r >> 8;
		*fp->i_bend++ = r;
	}
#endif	CRC

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

		if (Bflag)
		{
			error
			(	"Modificando o código para %2d bits aos %d bytes",
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

		if (Bflag)
		{
			error
			(	"Modificando o código para %2d bits aos %d bytes",
				bit_sz, in_cnt
			);
		}

	}	/* end aumentar o código */

	*fp1->c_ptr++ = max_code;

	flushcode ();

	send_seg (0, &outcolfile);

}	/* end write_max_code */
