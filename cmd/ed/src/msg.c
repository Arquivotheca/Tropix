/*
 ****************************************************************
 *								*
 *			msg.c					*
 *								*
 *	Definição das Mensagens ao Usuário			*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
 *		3.0.0, de 26.04.97				*
 *								*
 *	Modulo: ED						*
 *		Editores de Texto				*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include <stdio.h>

#include "../h/ed.h"
#include "../h/proto.h"
#include "../h/extern.h"

/*
 ******	Mensagens ***********************************************
 */
const char	M_ok[] =	"Ok";
const char	M_nspace[] =	"Não há memória para \"%s\"";
const char	M_wrerror[] =	"Erro na escrita";
const char	M_rderror[] =	"Erro na leitura";
const char	M_permission[] = "Permissão negada";
const char	M_nfound[] =	"Não encontrei \"%s\"";
const char	M_directory[] =	"\"%s\" é um diretório";
const char	M_iregular[] =	"\"%s\" não é um arquivo regular";
const char	M_object[] =	"\"%s\" é um arquivo objeto";
const char	M_empty[] =	"\"%s\" está vazio";
const char	M_rdonly[] =	"\"%s\" só tem permissão para leitura";
const char	M_exists[] =	"\"%s\" já existe";
const char	M_saved[] =	"\"%s\" já está atualizado";
const char	M_newfile[] =	"\"%s\": arquivo novo";
const char	M_size[] =	"\"%s\": %d linhas, %d caracteres";
