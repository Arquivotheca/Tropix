|*
|****************************************************************
|*								*
|*			settgrp.s				*
|*								*
|*	Atribui o processo no grupo de terminais		*
|*								*
|*	Versão	3.0.0, de 20.12.94				*
|*		3.0.0, de 12.08.95				*
|*								*
|*	Módulo: settgrp						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

SETTGRP		= 104
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "settgrp" *******************************
|*
|*	int	settgrp (void);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_settgrp
_settgrp:
	movl	#SETTGRP,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
