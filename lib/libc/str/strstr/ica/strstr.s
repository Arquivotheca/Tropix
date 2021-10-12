|*
|****************************************************************
|*								*
|*			strstr.s				*
|*								*
|*	Procura uma subcadeia em uma cadeia			*
|*								*
|*	Versão	2.3.0, de 07.03.90				*
|*		2.3.0, de 07.03.90				*
|*								*
|*	Módulo: strstr						*
|*		libc/str					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Procura uma subcadeia em uma cadeia			*
|****************************************************************
|*
|*	char *strstr (const char *cadeia, const char *subcadeia)
|*
	.globl	_strstr

_strstr:
	movl	4(sp),d1	|* d1 = Cadeia

	movl	8(sp),a0	|* Examina o primeiro caractere da subcadeia 
	tstb	(a0)
	jeq	sucesso

|*
|*	Procura inicialmente, o primeiro caracter da subcadeia
|*
tentativa:
	movl	8(sp),a0	|* d0 = 1o. caractere da subcadeia
	movb	(a0),d0

	movl	d1,a0		|* a0 = Ponteiro para a continuação da cadeia
loop1:
	cmpb	(a0),d0
	jeq	achou		|* achou igual

	tstb	(a0)+
	jne	loop1		|* não é fim da cadeia

	moveq	#0,d0		|* A cadeia não contém a subcadeia
	rts

|*
|*	O primeiro caracter confere, verifica o resto
|*
achou:
	movl	a0,d1		|* d1 = Pont. p. o início da subcadeia na cadeia

	movl	8(sp),a1	|* a1 = Ponteiro para o 2o. carac. da subcadeia
	addql	#1,a1

	addql	#1,a0		|* a0 = Ponteiro para o 2o. carac. da cadeia
loop2:
	tstb	(a1)
	jne	compara
sucesso:
	movl	d1,d0		|* ACHOU !!!
	rts
compara:
	cmpmb	(a0)+,(a1)+
	jeq	loop2

|*
|*	O resto não conferiu; prepara novo teste
|*
	addql	#1,d1
	jra	tentativa	
