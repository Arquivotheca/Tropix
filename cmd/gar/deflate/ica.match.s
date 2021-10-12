|*
|****************************************************************
|*								*
|*			ica.match.s				*
|*								*
|*	Busca de padrão otimizado em "assembly" (M68000)	*
|*								*
|*	Versão	3.0.00, de 05.06.93				*
|*		3.1.06, de 04.05.97				*
|*								*
|*	Módulo: GZIP						*
|*		Utilitários de compressão/descompressão		*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 		Baseado em software homônimo do GNU		*
|* 								*
|****************************************************************
|*

|*
|*	Constantes
|*
MAX_MATCH	= 	258
MIN_MATCH	=	3
WSIZE		=	32768
MAX_DIST	= 	WSIZE - MAX_MATCH - MIN_MATCH - 1

|*
|*	int	longest_match (cur_match)
|*
|*	Versão para alocação dinâmica das tabelas!
|*
	.global _longest_match
_longest_match:
	movl	4(sp),d1
	moveml	#<a2,a3,a4,d2,d3,d4,d5,d6,d7>,-(sp)

	movl	_max_chain_length,d6
	movl	_prev_length,d0
	movl	_prev,a2		|* == tab_prefix

	movl	_window,a4
	addql	#MIN_MATCH,a4

	movl	_strstart,d5
	movl	a4,a3
	addl	d5,a3
	subw	#MAX_DIST,d5
	jhi	L__limit_ok

	clrl	d5

L__limit_ok:
	cmpl	_good_match,d0
	jlo	L__length_ok

	lsrl	#2,d6

L__length_ok:
	subql	#1,d6
	movb	-MIN_MATCH(a3),d3
	lslw	#8,d3
	movb	-MIN_MATCH+1(a3),d3
	movb	-MIN_MATCH-1(a3,d0:w),d4
	lslw	#8,d4
	movb	-MIN_MATCH(a3,d0:w),d4
	jra	L__do_scan

L__long_loop:
	movb	-MIN_MATCH-1(a3,d0:w),d4
	lslw	#8,d4
	movb	-MIN_MATCH(a3,d0:w),d4

L__short_loop:
	lslw	#1,d1
	movw	0(a2,d1),d1
	cmpw	d5,d1
	dbls	d6,L__do_scan
	jra	L__return

L__do_scan:
	movl	a4,a1
	addl	d1,a1
	movb	-MIN_MATCH-1(a1,d0:w),d7
	lslw	#8,d7
	movb	-MIN_MATCH(a1,d0:w),d7
	cmpw	d7,d4
	jne	L__short_loop

	movb	-MIN_MATCH(a1),d7
	lslw	#8,d7
	movb	-MIN_MATCH+1(a1),d7
	cmpw	d7,d3
	jne	L__short_loop

	movw	#MAX_MATCH-MIN_MATCH+1-1,d2
	movl	a3,a0

L__scan_loop:
	cmpmb	(a1)+,(a0)+
	dbne	d2,L__scan_loop

	subl	a3,a0
	addql	#MIN_MATCH-1,a0
	cmpl	d0,a0
	jls	L__short_loop

	movl	a0,d0
	movl	d1,_match_start
	cmpl	_nice_match,d0
	jlo	L__long_loop

L__return:
	moveml	(sp)+,#<a2,a3,a4,d2,d3,d4,d5,d6,d7>
	rts
