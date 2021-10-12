/*
 ****************************************************************
 *								*
 *			h/txtio.h				*
 *								*
 *	Definições para Entrada e Saída de Texto		*
 *								*
 *	Versão	1.0.0, de 04.11.86				*
 *		3.0.0, de 26.04.97				*
 *								*
 *	Modulo: VI/EX						*
 *		Editores de Texto				*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Multiprocessado		*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#define	TXTIO_H

/*
 ******	Estado do arquivo ***************************************
 */
typedef	enum
{
	IO_OK,		/* E/S sem problemas */
	IO_NSPACE,	/* Não houve memória suficiente */
	IO_WRERROR,	/* Erro na escrita */
	IO_RDERROR,	/* Erro na leitura */
	IO_PERMISSION,	/* Permissão negada */
	IO_NFOUND,	/* Arquivo não encontrado */
	IO_DIRECTORY,	/* Arquivo é um diretório */
	IO_IREGULAR,	/* Arquivo irregular */
	IO_OBJECT,	/* Arquivo contem um programa objeto */
	IO_EMPTY,	/* Arquivo vazio */
	IO_RDONLY,	/* Arquivo com permissão apenas para leitura */
	IO_EXISTS,	/* Arquivo já existe */
	IO_SAVED	/* O Texto já foi salvo */

}	TXTIO;
