/*
 ****************************************************************
 *								*
 *			<mkasmtb.h>				*
 *								*
 *	Estrutura para a geração direta de objetos simbólicos	*
 *								*
 *	Versão	1.0.0, de 25.04.86				*
 *		3.0.0, de 23.02.95				*
 *								*
 *	Módulo: mkasmtb.h					*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Modo do módulo gerado ***********************************
 */
typedef	enum
{
	RO,		/* Readonly global */
	RW,		/* Read/write global */
	ROL,		/* Readonly local */
	RWL		/* Read/write local */

}	ROW;

/*
 ******	Estrutura para a descrição de cada entrada **************
 */
typedef	struct
{
	int	o_size;		/* Tamanho do membro */
	int	o_offset;	/* Posição do membro na entrada */
	int	o_count;	/* No. de exemplares deste membro */
	char	*o_rel_nm;	/* Nome da variável relocável (NOSTR se abs.) */
	void	*o_rel_addr;	/* Endereço da variável relocadora */

}	OBJENT;

#define NOOBJENT	(OBJENT *)NULL

/*
 ******	Protótipos **********************************************
 */
extern int	mkasmtb (const char *, const char *, const void *, long, ROW, int, const OBJENT *);
