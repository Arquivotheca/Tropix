/*
 ****************************************************************
 *								*
 *			as.h					*
 *								*
 *	Variáveis globais do montador				*
 *								*
 *	Versão	1.0.00, de 05.03.86				*
 *		3.0.12, de 07.08.97				*
 *								*
 *	Módulo: AS-80386/486/PENTIUM				*
 *		Tradutores					*
 *		Categoria D					*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

extern long	*lc;		/* Location counter */

extern long	lctext,		/* Location counter, TEXT */
		lcconst,	/* Location counter, CONST */
		lcdata,		/* Location counter, DATA */
		lcbss;		/* Location counter, BSS */

extern long	tsize,		/* Tamanho da seção TEXT */
		csize,		/* Tamanho da seção CONST */
		dsize,		/* Tamanho da seção DATA */
		bsize;		/* Tamanho da seção BSS */

extern int	pass;		/* Passo atual (1 ou 2) */

extern int	section;	/* Seção atual */

extern long	opcodeno;	/* Numerador sequencial das operações */

extern int	symblk;		/* "Local symbol block" atual */

extern int	aflag,		/* Gera todos os simbolos */
		vflag,		/* Verbose */
		wflag,		/* O "normal" é 16 bits (ao invés de 32) */
		eflag,		/* A listagem deve ser "little-endian" */
		lflag;		/* Lista o Programa Fonte */

extern int	seg16;		/* O segmento é de 16 bits (ao invés de 32) */

extern const char *objname,	/* Nome do arquivo objeto (default: "a.out") */
		*srcname,	/* Nome do arquivo fonte */
		*listname;	/* Nome da listagem */

extern char 	*rtname,	/* Nome do temporario de Relocação de TEXT */
		*rdname;	/* Nome do temporario de Relocação de DATA */

extern FILE	*src,		/* Arquivo Fonte */
		*list,		/* Listagem */
		*aout,		/* a.out */
		*reltext,	/* Relocação do text */
		*reldata;	/* Relocação do DATA */

extern int	nmsgs;		/* Número de Erros Encontrados */

extern long	sectoff;	/* Deslocamento dentro da seção atual */

extern char	*nextp;		/* Ponteiro do analisar léxico */
