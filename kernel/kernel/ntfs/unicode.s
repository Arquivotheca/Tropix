|*
|****************************************************************
|*	Converte UNICODE para ISO				*
|****************************************************************
|*
|*	char	*unicode2iso (char *dst, const char *src, int len);
|*
	.global	_unicode2iso
_unicode2iso:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* dst
	movl	12(fp),r4	|* src
	movl	16(fp),r1	|* len
2$:
	up
|*	incl	r4 		|* Pula o '\0'
	lodsb			|* Pega o caractere
   |***	movb	(r4)+,r0

	stosb			|* Armazena o caractere
   |***	movb	r0,(r5)+

	incl	r4 		|* Pula o '\0'

	subl	#1,r1		|* Verifica se chegou ao final
	jgt	2$

	clrb	(r5)		|* Fim da cadeia

	movl	8(fp),r0	|* Retorna o endereço do destino

	popl	r4
	popl	r5
	unlk
	rts

