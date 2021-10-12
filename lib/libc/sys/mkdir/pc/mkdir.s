|*
|****************************************************************
|*								*
|*			mkdir.s					*
|*								*
|*	Cria um diretório					*
|*								*
|*	Versão	3.0.0, de 07.01.95				*
|*		3.0.0, de 07.01.95				*
|*								*
|*	Módulo: mkdir						*
|*		libc/sys					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

MKDIR		= 75
SYS_CALL	= { 6 * 8 + 3 }

|*
|******	Chamada ao sistema "mkdir" *******************************
|*
|*	int	mkdir (char *path, int mode);
|*
|*	Retorna = Sucesso: 0; Erro: -1
|*
	.global	_mkdir
_mkdir:
	movl	#MKDIR,r0
	callgl	#SYS_CALL,0

	tstl	r2
	jnz	syscall_error

	rts
