|*
|*	Teste de endereçamento 16 bits
|*
delta0:
	decw	(bx,si)
	decw	(bx,di)
	decw	(bp,si)
	decw	(bp,di)
	decw	(si)
	decw	(di)
	decw	delta0
	decw	(bx)

	decw	0(bx,si)
	decw	0(bx,di)
	decw	0(bp,si)
	decw	0(bp,di)
	decw	0(si)
	decw	0(di)
	decw	0(bx)

	decw	8(bx,si)
	decw	8(bx,di)
	decw	8(bp,si)
	decw	8(bp,di)
	decw	8(si)
	decw	8(di)
	decw	8(bp)
	decw	8(bx)

	decw	512(bx,si)
	decw	512(bx,di)
	decw	512(bp,si)
	decw	512(bp,di)
	decw	512(si)
	decw	512(di)
	decw	512(bp)
	decw	512(bx)
delta4:
	divuw	eax
	divuw	ecx
	divuw	edx
	divuw	ebx
	divuw	sp
	divuw	fp
	divuw	esi
	divuw	edi
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
|*	LINK
|*
	link	#0
	link	#126
	link	#127
	link	#128
	link	#129
	link	#65535
|*	link	#65536

|*
|*	Terminou
|*
final:
	decl	ra

