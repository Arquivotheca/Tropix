/*
 ****************************************************************
 *								*
 *			<sys/shlib.h>				*
 *								*
 *	Estrutura da tabela de bibliotecas compartilhadas	*
 *								*
 *	Versão	3.2.3, de 02.12.99				*
 *		4.3.0, de 12.06.02				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	SHLIB_H			/* Para definir os protótipos */

/*
 ******	Tabela de bibliotecas compartilhadas ********************
 */
typedef struct
{
	char	sh_name[16];	/* Nome da biblioteca */
	time_t	sh_time;	/* Data da biblioteca */

	ushort	sh_dep_mask;	/* Dependências desta biblioteca */
	ushort	sh_hash_seq;	/* Ordem na inserção HASH */

	long	sh_tsize;	/* Tamanho do TEXT */
	long	sh_dsize;	/* Tamanho do DATA */
	long	sh_bsize;	/* Tamanho do BSS */
	long	sh_ssize;	/* Tamanho da SYMTB */

	pg_t	sh_tvaddr;	/* Endereço virtual do TEXT */
	pg_t	sh_dvaddr;	/* Endereço virtual do DATA */

	ulong	sh_vaddr_mask;	/* Indica as páginas virtuais usadas */

	REGIONG	*sh_region;	/* Região global do TEXT + DATA */

	/* Estatísticas */

	int	sh_sym_count;	/* No. de símbolos */
	int	sh_hash_col;	/* Colisões HASH */

}	SHLIB;

#define NSHLIB	4	/* Número máximo de bibliotecas compartihadas */

#define NOSHLIB	(SHLIB *)NULL

extern SHLIB	shlib_tb[];

/*
 ******	Protótipos de funções ***********************************
 */
enum { SHLIB_LOAD, SHLIB_UNLOAD, SHLIB_TABLE };	/* As operações */

extern int	shlib (int op, ...);
