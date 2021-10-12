|*
|****************************************************************
|*								*
|*			clrtgrp.s				*
|*								*
|*	Retira um processo do grupo de terminais		*
|*								*
|*	Versão	1.0.0, de 03.02.87				*
|*		2.3.0, de 07.09.88				*
|*								*
|*	Modulo: clrtgrp						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|*	int
|*	clrtgrp ()
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_clrtgrp, _errno

CLRTGRP = 104

_clrtgrp:
	moveq	#CLRTGRP,d0

	trap	#0
	jcc	1$

	movl	d0,_errno
	moveq	#-1,d0
1$:
	rts
