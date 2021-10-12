|*
|****************************************************************
|*								*
|*			abs.s					*
|*								*
|*	Obtém o valor absoluto					*
|*								*
|*	Versão	2.3.0, de 05.03.90				*
|*		3.0.0, de 08.06.95				*
|*								*
|*	Módulo: abs						*
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
|*	Obtém o valor absoluto					*
|****************************************************************
|*
|*	#include <stdlib.h>
|*
|*	int	abs (int j);
|*	long	labs (long j);
|*
	.global	_abs, _labs
_abs:
_labs:
	movl	4(sp),d0
	jge	1$

	negl	d0
1$:
	rts	
