|*
|****************************************************************
|*								*
|*			memmove.s				*
|*								*
|*	Rotina otimizada de copia de bytes/longs		*
|*								*
|*	Versão	1.0.0, de 26.12.86				*
|*		2.3.2, de 13.04.91				*
|*								*
|*	Modulo: memmove						*
|*		libc/mem					*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|* 								*
|****************************************************************
|*

|*
|****************************************************************
|*	Copia Otimizada						*
|****************************************************************
|*
|*	void *
|*	memmove (void *destino, void *fonte, size_t bytecount);
|*
|*	algoritmo:
|*	    a) Paridades dos Enderecos diferentes => Copia byte-a-byte
|*	    b) Paridades iguais:
|*		- Enderecos impares: move 1 byte e cai no caso abaixo
|*		- Enderecos pares: 
|*			Move long enquanto count >= 4
|*			Move os 3, 2 ou 1 bytes restantes
|*
|*	Observações:
|*		a0 => Endereco Virtual para acesso ao fonte
|*		a1 => Endereco Virtual para acesso ao destino
|*		d0 => Numero de bytes restantes a transferir
|*
	.global	_memmove

_memmove:
	movl	4(sp),a1	|* Destino
	movl	8(sp),a0	|* Fonte

	movl	12(sp),d0	|* d0 e d1 = nbytes
	movl	d0,d1

	jle	ret		|* retorna se nbytes <= 0

	cmpl	a0,a1		|* retorna se fonte == destino
	jne	dtst
ret:
	movl	4(sp),d0	|* Retorna o End. do Destino
	rts
dtst:
	jhi	pbaixo		|* desvia se destino > fonte	

|*
|*	Destino < Fonte => copia de baixo para cima
|*
pcima:
	btst	#0,7(sp)	|* Desvia se o Destino é Impar
	jne	10$

	btst	#0,11(sp)	|* Desvia se a Fonte é Impar
	jne	9$

|*
|*	Ambos enderecos pares; copia de 4 em 4 bytes
|*
4$:
	andl	#3,d1 		|* d1 = nbytes mod 4

	lsrl	#2,d0
	jra	6$
5$:
	movl	(a0)+,(a1)+
6$:
	dbra	d0,5$
	subl	#0x00010000,d0
	jhs	5$

	jra	9$

|*
|*	Copia byte-a-byte
|*
8$:
	movb	(a0)+,(a1)+
9$:
	dbra	d1,8$
	subl	#0x00010000,d1
	jhs	8$

	jra	ret

|*
|*	O Destino é impar
|*
10$:
	btst	#0,11(sp)	|* Desvia se Fonte é par
	jeq	9$

|*
|*	Destino e Fonte sao impares
|*	Move um byte e recai no caso de ambos pares
|*
	movb	(a0)+,(a1)+
	subql	#1,d1
	subql	#1,d0
	jra	4$

|*
|*	Destino > Fonte => copia de cima para baixo
|*
pbaixo:
	addl	d0,a0		|* Atualiza os enderecos
	movl	a0,8(sp)	|* para o final das areas
	addl	d0,a1
	movl	a1,4(sp)

	btst	#0,7(sp)	|* Desvia se o Destino é Impar
	jne	10$

	btst	#0,11(sp)	|* Desvia se a Fonte é Impar
	jne	9$

|*
|*	Ambos enderecos pares; copia de 4 em 4 bytes
|*
4$:
	andl	#3,d1 		|* d1 = nbytes mod 4

	lsrl	#2,d0
	jra	6$
5$:
	movl	-(a0),-(a1)
6$:
	dbra	d0,5$
	subl	#0x00010000,d0
	jhs	5$

	jra	9$

|*
|*	Copia byte-a-byte
|*
8$:
	movb	-(a0),-(a1)
9$:
	dbra	d1,8$
	subl	#0x00010000,d1
	jhs	8$

	movl	a1,d0		|* retorna com o end. do Destino
	rts

|*
|*	O Destino é impar
|*
10$:
	btst	#0,11(sp)	|* Desvia se Fonte é par
	jeq	9$

|*
|*	Destino e Fonte sao impares
|*	Move um byte e recai no caso de ambos pares
|*
	movb	-(a0),-(a1)
	subql	#1,d1
	subql	#1,d0
	jra	4$
