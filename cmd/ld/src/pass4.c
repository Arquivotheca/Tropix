/*
 ****************************************************************
 *								*
 *			pass4.c					*
 *								*
 *	Passo 4: Prepara os arquivos para a geração		*
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
 ****************************************************************
 *	Passo 4: Prepara os arquivos para a geração		*
 ****************************************************************
 */
void
pass4 (void)
{
	/*
	 *	Cria o módulo final
	 */
	if ((aout_fd = creat (obj_nm, REG_MODE)) < 0)
		error ("$*Não consegui criar \"%s\"", obj_nm);

	/*
	 *	Cria o temporário para o TEXT
	 */
	if (dotflag)
		text_nm = mktemp ("ldtXXXXXX");
	else
		text_nm = mktemp ("/tmp/ldtXXXXXX");

	if (text_nm == NOSTR)
		error ("$Não consegui obter um nome para o arquivo do TEXT");

	if ((text_fd = open (text_nm, O_RDWR|O_CREAT, REG_MODE)) < 0)
		error ("$*Não consegui criar \"%s\"", text_nm);

	if (unlink (text_nm) < 0)
		error ("$*Não consegui remover \"%s\"", text_nm);

	/*
	 *	Cria o temporário para o DATA
	 */
	if (dotflag)
		data_nm = mktemp ("lddXXXXXX");
	else
		data_nm = mktemp ("/tmp/lddXXXXXX");

	if (data_nm == NOSTR)
		error ("$Não consegui obter um nome para o arquivo do DATA");

	if ((data_fd = open (data_nm, O_RDWR|O_CREAT, REG_MODE)) < 0)
		error ("$*Não consegui criar \"%s\"", data_nm);

	if (unlink (data_nm) < 0)
		error ("$*Não consegui remover \"%s\"", data_nm);

	/*
	 *	Cria os temporários para RT e RD
	 */
	if (!rflag && !bflag)
		return;

	if ((rt_nm = mktemp ("/tmp/ldrtXXXXXX")) == NOSTR)
		error ("$Não consegui obter um nome para o arquivo da relocação do TEXT");

	if ((rt_fp = fopen (rt_nm, "w")) == NOFILE)
		error ("$*Não consegui criar \"%s\"", rt_nm);

	if ((rd_nm = mktemp ("/tmp/ldrdXXXXXX")) == NOSTR)
		error ("$Não consegui obter um nome para o arquivo da relocação do DATA");

	if ((rd_fp = fopen (rd_nm, "w")) == NOFILE)
		error ("$*Não consegui criar \"%s\"", rd_nm);

}	/* end pass4 */
