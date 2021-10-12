/*
 ****************************************************************
 *								*
 *			<sys/region.h>				*
 *								*
 *	Tabelas de regiões de memória				*
 *								*
 *	Versão	3.0.0, de 07.09.94				*
 *		4.6.0, de 22.08.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *		/usr/include/sys				*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *								*
 ****************************************************************
 */

#define	REGION_H		/* Para definir os protótipos */

/*
 ******	Formato das estruturas **********************************
 */
struct	regionl
{
	char	rgl_type;	/* Tipo da região */
	char	rgl_prot;	/* Tipo de proteção da região */

	pg_t	rgl_vaddr;	/* Endereço virtual em páginas na memória */
	REGIONG	*rgl_regiong;	/* Ponteiro para a região global */
};

struct	regiong
{
	char	rgg_name[16];	/* Nome da região */
	char	rgg_type;	/* Tipo da região */
	char	rgg_flags;	/* Características da região */
	LOCK	rgg_regionlock;	/* SPINLOCK de uso exclusivo da entrada */
	char	rgg_pgtb_sz;	/* Tamanho da tabela de páginas da região */

	pg_t	rgg_size;	/* Tamanho da região em páginas */
	pg_t	rgg_paddr;	/* Endereço real em páginas na memória */

	pg_t	rgg_pgtb;	/* Tabela de páginas da região */
	int	rgg_count;	/* No. de proc. usando esta região (procreg) */
	REGIONG	*rgg_next;	/* Próx. reg. assoc. mesmo i-node (shmemreg) */
};

/*
 ******	Código das regiões **************************************
 */
enum
{
	RG_NONE,	/* Região inexistente */
	RG_TEXT,	/* Região de texto compartilhado */
	RG_DATA,	/* Região de data (compart. entre threads) */
	RG_STACK,	/* Região de stack nunca compartilhada */
	RG_PHYS,	/* Região de PHYS */
	RG_SHMEM	/* Região de memória compartilhada */
};

#define RG_NAMES { "NONE", "TEXT", "DATA", "STACK", "PHYS", "SHMEM" }

/*
 ******	Indicadores para as operações com regiões ***************
 */
enum
{
	RG_CLR	   = 1,		/* Região deve ser zerada */
	RG_NOMMU   = 2,		/* Não carrega/descarrega MMU */

	RG_PHYSDUP = 0,		/* Região deve ser duplicada físicamente */
	RG_LOGDUP  = 4		/* Região deve ser duplicada logicamente */
};

/*
 ******	Indicadores para as regiões globais *********************
 */
enum
{
	RGG_METX   = 0x04	/* Manter cópia do processo em memória */
};

#define	RGG_FLAG_BITS	"\x03" "METX"

/*
 ******	Indicadores de criação de processos *********************
 */
enum { FORK, THREAD };
