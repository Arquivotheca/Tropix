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
	movl	4(sp),d0

	rolw	#8,d0
	swap	d0
	rolw	#8,d0

	rts

|*
|****************************************************************
|*  Converte um short de little em big endian (ou vice-versa)	*
|****************************************************************
|*
	.global	_short_endian_conversion, _short_endian_cv
_short_endian_conversion:
_short_endian_cv:
	clrl	d0
	movw	4+2(sp),d0

	rolw	#8,d0

	rts
