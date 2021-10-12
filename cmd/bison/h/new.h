/*
 ****************************************************************
 *								*
 *			new.h					*
 *								*
 *	Storage allocation interface for bison			*
 *								*
 *	Versão	3.0.0, de 06.07.93				*
 *		3.0.0, de 06.07.93				*
 *								*
 *	Módulo: GBISON						*
 *		Gerador de analisadores sintáticos		*
 *		Categoria B					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 		Baseado em software homônimo do GNU		*
 * 								*
 ****************************************************************
 */

/*
 ****************************************************************
 *	Storage allocation interface for bison			*
 ****************************************************************
 */
#define	NEW(t)		((t *) mallocate((unsigned) sizeof(t)))
#define	NEW2(n, t)	((t *) mallocate((unsigned) ((n) * sizeof(t))))

#define	FREE(x)		if (x)	\
				 free ((void *) (x))

extern	char *mallocate ();
