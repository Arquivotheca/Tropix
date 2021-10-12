/*
 ****************************************************************
 *								*
 *			<malloc.h>				*
 *								*
 *	Definições para a alocação dinâmica de memória		*
 *								*
 *	Versão	1.0.0, de 03.09.86				*
 *		3.0.0, de 06.07.95				*
 *								*
 *	Módulo: malloc.h					*
 *		/usr/include					*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 1995 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

/*
 ******	Definicão de macros *************************************
 */
#define	_LONGROUND(n)	(((n) + 3) & ~3)

#define	smalloc(n)	(_sm_base = _sm_next,				\
			(_sm_next += _LONGROUND (n)) <= _sm_end ?	\
				_sm_base : _smalloc (n))

/*
 ******	Variáveis Externas **************************************
 */
extern void	*_sm_base, *_sm_next, *_sm_end;

/*
 ******	Protótipos das funções **********************************
 */
extern void	*_smalloc (int);
extern void	sfree (void);
