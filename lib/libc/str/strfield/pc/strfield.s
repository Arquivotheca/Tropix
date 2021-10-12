|*
|****************************************************************
|*								*
|*			strfield.s				*
|*								*
|*	Extração de campos em uma cadeia			*
|*								*
|*	Versão	3.0.0, de 07.01.95				*
|*		3.0.0, de 08.08.95				*
|*								*
|*	Módulo: strfield					*
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
|*	Extração de campos em uma cadeia			*
|****************************************************************
|*
|*	char	*strfield (char *str, int sep);
|*
|*	str - cadeia com os campos
|*	sep - separador de campos
|*
	.global	_strfield
_strfield:
	link	#0
	pushl	r5

	movl	8(fp),r5	|* "str"
	movb	12(fp),r0	|* "sep"
	up			|* Sempre para cima

|*
|******	Verifica se é início de nova linha **********************
|*
	tstl	r5
	jnz	loop

	movl	next_str,r5	|* resto da cadeia
	tstl	r5
	jz	no_field

	movl	r5,8(fp)	|* guarda o inicio do campo

|*
|****** Procura o final do campo ********************************
|*
loop:
	cmpb	#0,(r5)		|* desvia se achou o final da cadeia
	jne	1$

	clrl	r5		|* Acabou a linha
	jmp	return_field
1$:
	scasb			|* desvia se não achou o separador
   |***	cmpb	r0,(r5)+
	jne	loop

|*
|******	Achou o separador ***************************************
|*
	clrb	-1(r5)		|* Põe o '\0'

return_field:
	movl	r5,next_str	|* Prepara para a próxima chamada
	movl	8(fp),r5	|* Retorna o endereço do campo

|*
|******	Chegou no final da cadeia *******************************
|*
no_field:
	movl	r5,r0		|* Coloca o resultado em "r0"

	popl	r5
	unlk
	rts

|*
|******	Areas e dados *******************************************
|*
	.data
next_str:
	.long	0		|* Começo do campo seguinte
