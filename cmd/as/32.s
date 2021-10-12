|*
|*	Teste de endereçamento de 32 bits
|*
delta0:
	decl	(eax)
	decl	(ecx)
	decl	(edx)
	decl	(ebx)
	decl	(sp)
	decl	(fp)
	decl	(esi)
	decl	(edi)
	decl	delta0
delta1:
	decl	0(eax)
	decl	0(ecx)
	decl	0(edx)
	decl	0(ebx)
	decl	0(sp)
	decl	0(fp)
	decl	0(esi)
	decl	0(edi)
delta2:
	decl	8(eax)
	decl	8(ecx)
	decl	8(edx)
	decl	8(ebx)
	decl	8(sp)
	decl	8(fp)
	decl	8(esi)
	decl	8(edi)
delta3:
	decl	512(eax)
	decl	512(ecx)
	decl	512(edx)
	decl	512(ebx)
	decl	512(sp)
	decl	512(fp)
	decl	512(esi)
	decl	512(edi)
	decl	delta0(edi)
delta4:
	divul	eax
	divul	ecx
	divul	edx
	divul	ebx
	divul	sp
	divul	fp
	divul	esi
	divul	edi
delta5:
	divub	al
	divub	cl
	divub	dl
	divub	bl
	divub	ah
	divub	ch
	divub	dh
	divub	bh

|*
|*	Com índice
|*
	decl	(eax,edx)
	decl	(ecx,edx)
	decl	(edx,edx)
	decl	(ebx,edx)
	decl	(sp,edx)
	decl	(fp,edx)
	decl	(esi,edx)
	decl	(edi,edx)

	decl	8(eax,edx)
	decl	8(ecx,edx)
	decl	8(edx,edx)
	decl	8(ebx,edx)
	decl	8(sp,edx)
	decl	8(fp,edx)
	decl	8(esi,edx)
	decl	8(edi,edx)

	decl	512(eax,edx)
	decl	512(ecx,edx)
	decl	512(edx,edx)
	decl	512(ebx,edx)
	decl	512(sp,edx)
	decl	512(fp,edx)
	decl	512(esi,edx)
	decl	512(edi,edx)
delta6:
	decl	(ebx,eax)
	decl	(ebx,ecx)
	decl	(ebx,edx)
	decl	(ebx,ebx)
|*	decl	(ebx,sp)
	decl	(ebx,fp)
	decl	(ebx,esi)
	decl	(ebx,edi)

	decl	8(ebx,eax)
	decl	8(ebx,ecx)
	decl	8(ebx,edx)
	decl	8(ebx,ebx)
|*	decl	8(ebx,sp)
	decl	8(ebx,fp)
	decl	8(ebx,esi)
	decl	8(ebx,edi)

	decl	512(ebx,eax)
	decl	512(ebx,ecx)
	decl	512(ebx,edx)
	decl	512(ebx,ebx)
|*	decl	512(ebx,sp)
	decl	512(ebx,fp)
	decl	512(ebx,esi)
	decl	512(ebx,edi)

|*
|*	Escalonamento
|*
delta7:
	decl	8(ebx,edi@b)
	decl	8(ebx,edi@w)
	decl	8(ebx,edi@l)
	decl	8(ebx,edi@q)

|*
|*	Índice mas sem base
|*
	decl	(*,eax)
	decl	(*,ecx)
	decl	(*,edx)
	decl	(*,ebx)
	decl	(*,sp)
	decl	(*,fp)
	decl	(*,esi)
	decl	(*,edi)
	decl	(,edi)

	decl	8(*,eax)
	decl	8(*,ecx)
	decl	8(*,edx)
	decl	8(*,ebx)
	decl	8(*,sp)
	decl	8(*,fp)
	decl	8(*,esi)
	decl	8(*,edi)
	decl	8(,edi)

	decl	512(*,eax)
	decl	512(*,ecx)
	decl	512(*,edx)
	decl	512(*,ebx)
	decl	512(*,sp)
	decl	512(*,fp)
	decl	512(*,esi)
	decl	512(*,edi)
	decl	512(,edi)

	decl	(*,edx@b)
	decl	(*,edx@w)
	decl	(*,edx@l)
	decl	(*,edx@q)

	decl	8(*,edx@b)
	decl	8(*,edx@w)
	decl	8(*,edx@l)
	decl	8(*,edx@q)

	decl	512(*,edx@b)
	decl	512(*,edx@w)
	decl	512(*,edx@l)
	decl	512(*,edx@q)

|*
|*	Problema do fp como base
|*
delta8:
	decl	(*,edx)
	decl	8(*,edx)
	decl	512(*,edx)
	decl	(fp,edx)
	decl	8(fp,edx)
	decl	512(fp,edx)

|*
|*	Troca de modo
|*
	movl	r1,8(fp)
	.seg16
	movl	r1,8(fp)
	.seg32
	movl	r1,8(fp)

|*
|*	LINK
|*
	link	#0
	link	#126
	link	#127
	link	#128
	link	#129
	link	#100000

|*
|*	PUSH e POP
|*
	pushw	ds
	pushl	ds
	popw	ds
	popl	ds

	.seg16
	pushw	ds
	pushl	ds
	popw	ds
	popl	ds

	.seg32

|*
|*	Terminou
|*
final:
	decl	ra

