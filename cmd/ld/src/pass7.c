/*
 ****************************************************************
 *								*
 *			pass7.c					*
 *								*
 *   Passo 7: Coda: Escreve as diversas seções e o cabeçalho	*
 *								*
 *	Versão	1.0.0, de 23.10.86				*
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <a.out.h>

#include "../h/ld.h"

/*
 ****************************************************************
 *	Definições globais					*
 ****************************************************************
 */
/*
 ****** Máscaras para criação de arquivos ***********************
 */
#define	REG_MODE	0644
#define	EXEC_MODE	0755

/*
 ******	Protótipos de funções ***********************************
 */
int		section_copy (const char *, int, char *, int size);

/*
 ****************************************************************
 *   Passo 7: Coda: Escreve as diversas seções e o cabeçalho	*
 ****************************************************************
 */
void
pass7 (void)
{
	const GSYM	*gp;
	int		n, mask, mode, count;
	HEADER		h;
	FILE		*fp = NOFILE;
	char		area[BUFSZ];

	/*
	 *	Prepara o cabeçalho
	 */
	memsetl (&h, 0, sizeof (HEADER) / sizeof (long));

	h.h_machine = 0x486;
	h.h_magic   = bflag ? SMAGIC : (nflag ? NMAGIC : FMAGIC);
   /***	h.h_version = 0;	***/
   /***	h.h_flags   = 0;	***/
	h.h_time    = time ((time_t *)NULL);
   /***	h.h_serial  = 0; 	***/
	h.h_tstart  = TEXT_ORG;
	h.h_dstart  = DATA_ORG;
   /***	h.h_entry   = ...;	***/
	h.h_tsize   = TEXT_SIZE;
	h.h_dsize   = DATA_SIZE;
	h.h_bsize   = COMMON_SIZE + BSS_SIZE;
	h.h_ssize   = sflag ? 0 : SYMTB_SIZE;
	h.h_rtsize  = TEXT_REL_SIZE;
	h.h_rdsize  = DATA_REL_SIZE;
   /***	h.h_lnosize = 0;	***/
   /***	h.h_dbsize  = 0;	***/
   /***	h.h_modsize = 0;	***/
   /***	h.h_refsize = ...;	***/

	/*
	 *	Calcula o tamanho da tabela de referências
	 */
	if (shared_lib_given) for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
	{
		if (gp->s_ref_len == 0)
			continue;

		h.h_refsize += EXTREF_SIZEOF (gp->s.s_nm_len) + gp->s_ref_len * sizeof (off_t);
	}

	/*
	 *	Analisa o Ponto de Entrada
	 */
	h.h_entry = TEXT_ORG;	/* Por omissão */

	if   (entry_nm != NOSTR)
	{
		if ((gp = hash (entry_nm, strlen (entry_nm))) == NOGSYM)
			error ("@Ponto de entrada \"%s\" não definido", entry_nm);
	}
	elif ((gp = hash ("start", sizeof ("start") - 1)) != NOGSYM)
	{
		entry_nm = "start";
	}

	if (gp != NOGSYM)
	{
		if (gp->s.s_type == (EXTERN|TEXT))
			h.h_entry = gp->s.s_value;
		else
			error ("@O ponto de entrada \"%s\" não é da seção TEXT ou não é global", entry_nm);
	}

	/*
	 *	Escreve o Cabeçalho
	 */
	if (write (aout_fd, &h, sizeof (HEADER)) != sizeof (HEADER))
		error ("*@Erro na escrita do cabeçalho de \"%s\"", obj_nm);

	/*
	 *	Copia o TEXT
	 */
	if (TEXT_SIZE != 0)
	{
		lseek (text_fd, 0, SEEK_SET);

		if ((count = section_copy ("do TEXT", text_fd, area, 0)) != TEXT_SIZE)
		{
			if (count > TEXT_SIZE)
				error ("@Tamanho inválido do arquivo temporário de texto");

			/*
			 *	Se o programa for reentrante, completa o TEXT com zeros no final
			 */
			if (nflag)
			{
				n = TEXT_SIZE - count; memset (area, '\0', n);

				if (write (aout_fd, area, n) != n)
					error ("*@Erro de escrita em \"%s\"", obj_nm);
			}
			else
			{
				error ("@Tamanho inválido do arquivo temporário de texto");
			}
		}
	}
	elif (close (text_fd) < 0)
	{
		error ("*Não consegui fechar o arquivo temporário de texto");
	}

	/*
	 *	Copia o DATA
	 */
	if (DATA_SIZE != 0)
	{
		lseek (data_fd, 0, SEEK_SET);

		section_copy ("do DATA", data_fd, area, DATA_SIZE);
	}
	elif (close (data_fd) < 0)
	{
		error ("*Não consegui fechar o arquivo temporário de data");
	}

	/*
	 *	Gera a tabela de símbolos global
	 */
	if (!sflag && SYMTB_SIZE != 0)
	{
		if ((fp = fdopen (aout_fd, "w")) == NOFILE)
			error ("$*Não consegui abrir novamente o arquivo \"%s\"", obj_nm);

		for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
		{
			if (fwrite (&gp->s, SYM_SIZEOF (gp->s.s_nm_len), 1, fp) != 1)
				error ("$*Erro na escrita da tabela de símbolos de \"%s\"", obj_nm);
		}

		fflush (fp);	/* Não pode fechar */
	}

	/*
	 *	Relocações ou Referências externas
	 */
	if   (rflag || bflag)
	{
		/*
		 *	Copia RT
		 */
		fclose (rt_fp);

		if (TEXT_REL_SIZE != 0)
			section_copy ("de relocação do texto", open (rt_nm, O_RDONLY), area, TEXT_REL_SIZE);

		if (unlink (rt_nm) < 0)
			error ("*@Não consegui remover \"%s\"", rt_nm);

		/*
		 *	Copia RD
		 */
		fclose (rd_fp);

		if (DATA_REL_SIZE != 0)
			section_copy ("de relocação do data", open (rd_nm, O_RDONLY), area, DATA_REL_SIZE);

		if (unlink (rd_nm) < 0)
			error ("*@Não consegui remover \"%s\"", rd_nm);

	}
	elif (shared_lib_given)
	{
		if (fp == NOFILE && (fp = fdopen (aout_fd, "w")) == NOFILE)
			error ("$*Não consegui abrir novamente o arquivo \"%s\"", obj_nm);

		for (gp = gsym_list.s_next; gp != NOGSYM; gp = gp->s_next)
		{
			if (gp->s_ref_len == 0)
				continue;

			fwrite (&gp->s_ref_len,  sizeof (ushort), 1, fp);
			fwrite (&gp->s.s_nm_len, sizeof (ushort), 1, fp);
			fwrite (&gp->s.s_name, SYM_NM_SZ (gp->s.s_nm_len), 1, fp);
			fwrite (gp->s_ref_vec, gp->s_ref_len * sizeof (off_t), 1, fp);
		}

		fflush (fp);	/* Não pode fechar */
	}

	/*
	 *	Se não houve erros até agora, é executável
	 */
	mask = umask (0); umask (mask);

	if (error_standard_count == 0 && !rflag && !bflag)
		mode = EXEC_MODE;
	else
		mode = REG_MODE;

	if (chmod (obj_nm, mode & ~mask) < 0)
		error ("*Não consegui trocar a proteção de \"%s\"", obj_nm);

}	/* end pass7 */

/*
 ****************************************************************
 *	Copia o arquivo para o final do objeto			*
 ****************************************************************
 */
int
section_copy (const char *nm, int fd, char *area, int size)
{
	int		n, count = 0;

	/*
	 *	Confere o "fd"
	 */
	if (fd < 0)
		error ("$*Não consegui abrir o arquivo temporário %s", nm);

	/*
	 *	Copia o arquivo
	 */
	while ((n = read (fd, area, BUFSZ)) > 0)
	{
		if (write (aout_fd, area, n) != n)
			error ("$*Erro de escrita em \"%s\"", obj_nm);

		count += n;
	}

	if (n < 0)
		error ("$*Erro de leitura do arquivo temporário %s", nm);

	/*
	 *	Fecha o Temporário
	 */
	if (close (fd) < 0)
		error ("*Não consegui fechar o arquivo temporário %s", nm);

	if (size != 0 && count != size)
		error ("$Tamanho inválido do arquivo temporário %s (%d :: %d)", nm, count, size);

	return (count);

}	/* end section_copy */
