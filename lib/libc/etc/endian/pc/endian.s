|*
|****************************************************************
|*								*
|*			endian.s				*
|*								*
|*	Conversão de "big" para "little" endian e vice-versa	*
|*								*
|*	Versão	3.0.0, de 06.01.95				*
|*		3.0.0, de 20.06.95				*
|*								*
|*	Módulo: endian						*
|*		libc/etc					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

 
|*
|****************************************************************
|*  Converte um longo de little em big endian (ou vice-versa)	*
|****************************************************************
|*
	.text
	.global	_long_endian_conversion, _long_endian_cv
_long_endian_conversion:
_long_endian_cv:
	movl	4(sp),r0

	xchgb	r0,h0
	rorl	#16,r0
	xchgb	r0,h0

	rts

|*
|****************************************************************
|*  Converte um short de little em big endian (ou vice-versa)	*
|****************************************************************
|*
	.global	_short_endian_conversion, _short_endian_cv
_short_endian_conversion:
_short_endian_cv:
	clrl	r0
	movw	4(sp),r0

	xchgb	r0,h0

	rts
