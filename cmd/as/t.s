alpha	= 0x1234

	.data
	.long	0
	.global	beta
beta:
	.long	0
	.text

|*
|*	XCHG
|*
	xchgb	8(fp),r0
	xchgb	r3,r0
	xchgb	r0,8(fp)

	xchgw	8(fp),r0
	xchgw	r3,r0
	xchgw	r0,r3
	xchgw	r0,8(fp)
	xchgw	8(fp),r3
	xchgw	r3,r1
	xchgw	r3,8(fp)

	xchgl	8(fp),r0
	xchgl	r3,r0
	xchgl	r0,r3
	xchgl	r0,8(fp)
	xchgl	8(fp),r3
	xchgl	r3,r1
	xchgl	r3,8(fp)

|*
|*	TST
|*
	tstb	r0
	tstb	r1
	tstb	r2
	tstb	r3
	tstb	h0
	tstb	h1
	tstb	h2
	tstb	h3

	tstw	r2

	tstl	r0
	tstl	r1
	tstl	r2
	tstl	r3
	tstl	r4
	tstl	r5
	tstl	fp
	tstl	sp

	testb	#0x23,r0
	testb	#0x23,r3
	testb	#0x23,8(fp)
	testb	r1,8(fp)

	testw	#0x23,r0
	testw	#0x23,r3
	testw	#0x23,8(fp)
	testw	r1,8(fp)

	testl	#0x23,r0
	testl	#0x23,r3
	testl	#0x23,8(fp)
	testl	r1,8(fp)

|*
|*	SLDT e outras
|*
	sldt	8(fp)
	smsw	8(fp)
	str	8(fp)
	verr	8(fp)
	verw	8(fp)

|*
|*	SHLD/SHRD
|*
	shldw	#7,r2,8(fp)
	shldw	cl,r2,8(fp)
	shldl	#7,r2,8(fp)
	shldl	cl,r2,8(fp)

	shrdw	#7,r2,8(fp)
	shrdw	cl,r2,8(fp)
	shrdl	#7,r2,8(fp)
	shrdl	cl,r2,8(fp)

|*
|*	LGDT/LIDT
|*
	sgdt	8(fp)
	sidt	8(fp)

|*
|*	SETcc
|*
	setvs	8(fp)
	setvc	8(fp)
	setlo	8(fp)
	setcs	8(fp)
	seths	8(fp)
	setcc	8(fp)
	seteq	8(fp)
	setne	8(fp)
	setls	8(fp)
	sethi	8(fp)
	setmi	8(fp)
	setpl	8(fp)
	setp	8(fp)
	setpe	8(fp)
	setpo	8(fp)
	setlt	8(fp)
	setge	8(fp)
	setle	8(fp)
	setgt	8(fp)

|*
|*	SHIFT
|*
	aslb	#1,8(fp)
	salb	#1,8(fp)
	salb	cl,8(fp)
	salb	#7,8(fp)
	aslw	#1,8(fp)
	salw	#1,8(fp)
	salw	cl,8(fp)
	salw	#7,8(fp)
	asll	#1,8(fp)
	sall	#1,8(fp)
	sall	cl,8(fp)
	sall	#7,8(fp)

	asrb	#1,8(fp)
	sarb	#1,8(fp)
	sarb	cl,8(fp)
	sarb	#7,8(fp)
	asrw	#1,8(fp)
	sarw	#1,8(fp)
	sarw	cl,8(fp)
	sarw	#7,8(fp)
	asrl	#1,8(fp)
	sarl	#1,8(fp)
	sarl	cl,8(fp)
	sarl	#7,8(fp)

	lslb	#1,8(fp)
	shlb	#1,8(fp)
	shlb	cl,8(fp)
	shlb	#7,8(fp)
	lslw	#1,8(fp)
	shlw	#1,8(fp)
	shlw	cl,8(fp)
	shlw	#7,8(fp)
	lsll	#1,8(fp)
	shll	#1,8(fp)
	shll	cl,8(fp)
	shll	#7,8(fp)

	lsrb	#1,8(fp)
	shrb	#1,8(fp)
	shrb	cl,8(fp)
	shrb	#7,8(fp)
	lsrw	#1,8(fp)
	shrw	#1,8(fp)
	shrw	cl,8(fp)
	shrw	#7,8(fp)
	lsrl	#1,8(fp)
	shrl	#1,8(fp)
	shrl	cl,8(fp)
	shrl	#7,8(fp)

|*
|*	ROTATE
|*
	rclb	#1,8(fp)
	rclb	cl,8(fp)
	rclb	#7,8(fp)
	rclw	#1,8(fp)
	rclw	cl,8(fp)
	rclw	#7,8(fp)
	rcll	#1,8(fp)
	rcll	cl,8(fp)
	rcll	#7,8(fp)

	rcrb	#1,8(fp)
	rcrb	cl,8(fp)
	rcrb	#7,8(fp)
	rcrw	#1,8(fp)
	rcrw	cl,8(fp)
	rcrw	#7,8(fp)
	rcrl	#1,8(fp)
	rcrl	cl,8(fp)
	rcrl	#7,8(fp)

	rolb	#1,8(fp)
	rolb	cl,8(fp)
	rolb	#7,8(fp)
	rolw	#1,8(fp)
	rolw	cl,8(fp)
	rolw	#7,8(fp)
	roll	#1,8(fp)
	roll	cl,8(fp)
	roll	#7,8(fp)

	rorb	#1,8(fp)
	rorb	cl,8(fp)
	rorb	#7,8(fp)
	rorw	#1,8(fp)
	rorw	cl,8(fp)
	rorw	#7,8(fp)
	rorl	#1,8(fp)
	rorl	cl,8(fp)
	rorl	#7,8(fp)

|*
|*	OUT
|*
	outb	ra,#8
	outw	ra,#8
	outl	ra,#8

	outb	ra,edx
	outw	ra,edx
	outl	ra,edx

|*
|*	NEG
|*
	negb	8(fp)
	negw	8(fp)
	negl	8(fp)
	negl	r0

|*
|*	NOT
|*
	notb	8(fp)
	notw	8(fp)
	notl	8(fp)
	notl	r0

|*
|*	MOVx - moves exóticos
|*
	movw	8(fp),es
	movw	8(fp),cs
	movw	8(fp),ss
	movw	8(fp),ds
	movw	8(fp),fs
	movw	8(fp),gs
	movw	r0,gs

	movw	es,8(fp)
	movw	cs,8(fp)
	movw	ss,8(fp)
	movw	ds,8(fp)
	movw	fs,8(fp)
	movw	gs,8(fp)
	movw	gs,r0

	movl	cr0,r3
	movl	cr2,r3
	movl	cr3,r3

	movl	r1,cr0
	movl	r1,cr2
	movl	r1,cr3

	movl	dr0,r1
	movl	dr1,r1
	movl	dr2,r1
	movl	dr3,r1
	movl	dr6,r1
	movl	dr7,r1

	movl	r2,dr0
	movl	r2,dr1
	movl	r2,dr2
	movl	r2,dr3
	movl	r2,dr6
	movl	r2,dr7

	movl	tr6,sp
	movl	tr7,sp

	movl	r2,tr6
	movl	r2,tr7

|*
|*	LTR
|*
	ltr	8(fp)

|*
|*	LSL => LSEGL
|*
	lseglw	8(fp),r2
	lsegll	8(fp),r2

|*
|*	LOOP
|*
	loop	1$
	loopeq	1$
	loope	1$
	loopz	1$
	loopne	1$
	loopnz	1$

|*
|*	LLDT, LMSW
|*
	lldt	8(fp)
	lmsw	8(fp)

|*
|*	LGS, ...
|*
1$:
	ldsw	8(fp),r4
	lssw	8(fp),r4
	lesw	8(fp),r4
	lfsw	8(fp),r4
	lgsw	8(fp),r4
	ldsl	8(fp),r4
	lssl	8(fp),r4
	lesl	8(fp),r4
	lfsl	8(fp),r4
	lgsl	8(fp),r4

|*
|*	LGDT/LIDT
|*
	lgdtw	8(fp)
	lidtw	8(fp)
	lgdtl	8(fp)
	lidtl	8(fp)

|*
|*	LAR
|*
	larw	8(fp),r2
	larl	8(fp),r2

|*
|*	Multiplicação
|*
	mulsb	8(fp)
	mulsb	8(fp),r0

	mulsw	8(fp)
	mulsw	8(fp),r0
	mulsw	8(fp),r2
	mulsw	#0x12,r2
	mulsw	#0x12,r2,r2
	mulsw	#0x12,r2,r4
	mulsw	#0x12,8(fp),r1
	mulsw	#0x1234,r2
	mulsw	#0x1234,r2,r2
	mulsw	#0x1234,8(fp),r1
	imulw	#0x1234,8(fp),r1

	mulsl	8(fp)
	mulsl	8(fp),r0
	mulsl	8(fp),r2
	mulsl	#0x12,r2
	mulsl	#0x12,r2,r2
	mulsl	#0x12,r2,r4
	mulsl	#0x12,8(fp),r1
	mulsl	#0x1234,r2
	mulsl	#0x1234,r2,r2
	mulsl	#0x1234,8(fp),r1
	imull	#0x1234,8(fp),r1

	mulub	8(fp),r0
	muluw	8(fp),r0
	mulul	8(fp),r0
	mull	8(fp),r0

|*
|*	In
|*
	inb	#8,r0
	inw	#8,r0
	inl	#8,r0
	inb	r2,r0
	inw	r2,r0
	inl	r2,r0
	inl	dx,r0

|*
|*	Invlpg
|*
	invlpg	(r0)
	invlpg	(r1)
	invlpg	8(fp)
	invlpg	alpha

|*
|*	Bswap
|*
	bswap	r0
	bswap	r1
	bswapl	r5

|*
|*	Enter
|*
	enter	#10000,#3
	enter	#10000,#3

|*
|*	Multiplicação/divisão
|*
	divub	8(fp)
	divub	8(fp),r0
	divb	8(fp)
	divb	8(fp),r0

	divuw	8(fp)
	divuw	8(fp),r0
	divw	8(fp)
	divw	8(fp),r0

	divul	8(fp)
	divul	8(fp),r0
	divl	8(fp)
	divl	8(fp),r0

	divsb	8(fp)
	divsb	8(fp),r0
	idivb	8(fp)
	idivb	8(fp),r0

	divsw	8(fp)
	divsw	8(fp),r0
	idivw	8(fp)
	idivw	8(fp),r0

	divsl	8(fp)
	divsl	8(fp),r0
	idivl	8(fp)
	idivl	8(fp),r0

|*
|*	Incremento/decremento
|*
12$:
	decb	r1
	decw	r1
	decl	r1
	decb	8(fp)
	decw	8(fp)
	decl	8(fp)

	incb	r2
	incw	r2
	incl	r2
	incb	8(fp)
	incw	8(fp)
	incl	8(fp)

|*
|*	Interrupções
|*
2$:
	int3
	into
	int	#34

|*
|*	Instruções sem operandos, de 1 byte
|*
3$:
	aaa
	aas
	cbw
	cwde
	clc
	cld
	intdis
	disint
	cli
	cmc
	cmpsb
	cmpsw
	cmpsl
	cmpsd
	cwd
	cdq
	daa
	das
	insb
	insw
	insl
	insd
	hlt

|*
|*	Instrução IRET
|*
	iretw
	rte
	iretl
	iretd

	lahf
	leave
	lock
	lodsb
	lodsw
	lodsl
	lodsd
	movsb
	movsw
	movsl
	movsd
	nop
	outsb
	outsw
	outsl
	outsd
	popaw
	popal
	popad
	popfw
	popfl
	popfd
	pushaw
	pushal
	pushad
	pushfw
	pushfl
	pushfd
	rep
	repe
	repeq
	repz
	repne
	repnz
	sahf
	stc
	std
	inten
	enint
	sti
	wait
	stosb
	stosw
	stosl
	stosd
	xlat
	xlatb

|*
|*	Instruções sem operandos, de 2 bytes
|*
4$:
	aad
	aam
	clts
	cpuid
	rdtsc
	invd
	rdmsr
	rsm
	wbinvd
	wrmsr

|*
|******* Outras instruções com sintaxe do grupo 1 ***************
|*
5$:
	arpl	r1,8(fp)
	boundw	8(fp),r1
	boundl	8(fp),r1

|*
|****** Operações de BITs ***************************************
|*
6$:
	bsfw	8(fp),r1
	bsfl	8(fp),r1
	bsrw	8(fp),r1
	bsrl	8(fp),r1
	btw	r1,8(fp)
	btw	#12,8(fp)
	btl	r1,8(fp)
	btl	#12,8(fp)
	btcw	r1,8(fp)
	btcw	#12,8(fp)
	btcl	r1,8(fp)
	btcl	#12,8(fp)
	btrw	r1,8(fp)
	btrw	#12,8(fp)
	btrl	r1,8(fp)
	btrl	#12,8(fp)
	btsw	r1,8(fp)
	btsw	#12,8(fp)
	btsl	r1,8(fp)
	btsl	#12,8(fp)

|*
|*	MOVs de conversão
|*
7$:
	movsbw	8(fp),r0
	movsbl	8(fp),r0
	movswl	8(fp),r0

	movzbw	8(fp),r0
	movzbl	8(fp),r0
	movzwl	8(fp),r0

|*
|*	Instrução RET
|*
8$:
	rts
	ret
	farrts
	lret

	rtd	#9
	rtd	#alpha
	farrtd	#9
	lrtd	#9

|*
|*	Instrução POP
|*
9$:
	popw	8(fp)
	popw	fp

	popl	es
|*	popl	cs	|* Não pode
	popl	ss
	popl	ds
	popl	fs
	popl	gs

	popl	8(fp)
	popl	fp

|*
|*	Instrução "lea"
|*
10$:
	lea	8(fp),r3
	lea	alpha,r0
	lea	undef,r0
	lea	8(r3,r1),sp

	leal	8(fp),r3
	leal	alpha,r0
	lea	undef,r0
	leal	8(r3,r1),sp

|*
|*	Instrução "movb"
|*
11$:
	movb	r3,8(fp)
	movb	h3,8(fp)
	movb	8(fp),r3
	movb	alpha,r0
	movb	r0,alpha
	movb	#0x12,r3
	movb	#0x12,8(r3,r1)

|*
|*	Instrução "movw"
|*
	.global	gamma0
gamma0:
	movw	r3,8(fp)
	movw	8(fp),r3
	movw	alpha,r0
	movw	r0,alpha
	movw	#0x1234,r3
	movw	#0x1234,8(r3,r1)

|*
|*	Instrução "movl"
|*
	.global	gamma1
gamma1:
	movl	r3,8(fp)
	movl	8(fp),r3
	movl	alpha,r0
	movl	r0,alpha
	movl	#0x1234,r3
	movl	#0x1234,8(r3,r1)

|*
|*	Estudo de registros high
|*
	.global	gamma2
gamma2:
	subl	#12,ra
	subw	#12,ra
	subb	#12,ra
	subb	#12,al
	subb	#12,ah

	subl	#12,rb
	subw	#12,rb
	subb	#12,rb
	subb	#12,bl
	subb	#12,bh

	subl	#12,esi
	subw	#12,si

	subb	#12,r0
	subb	#12,r3
	subb	#12,r1
	subb	#12,r2
|*	subb	#12,r4
|*	subb	#12,r5
|*	subb	#12,fp
|*	subb	#12,sp

|*
|*	Instrução ADC
|*
	.global	epsilon0
epsilon0:
	adcb	#0x12,al
	adcw	#0x1234,ax
	adcl	#0x123456,eax
	adcb	#0x12,8(ebp)
	adcw	#0x1234,8(ebp)
	adcl	#0x123456,8(ebp)
	adcw	#0x12,8(ebp)
	adcl	#0x12,8(ebp)
	adcb	ah,8(ebp)
	adcw	ax,8(ebp)
	adcl	eax,8(ebp)
	adcb	8(ebp),ah
	adcw	8(ebp),ax
	adcl	8(ebp),eax

|*
|*	Instrução ADD
|*
	.global	gamma4
gamma4:
	addb	#0x12,al
	addw	#0x1234,ax
	addl	#0x123456,eax
	addb	#0x12,8(ebp)
	addw	#0x1234,8(ebp)
	addl	#0x123456,8(ebp)
	addw	#0x12,8(ebp)
	addl	#0x12,8(ebp)
	addb	ah,8(ebp)
	addw	ax,8(ebp)
	addl	eax,8(ebp)
	addb	8(ebp),ah
	addw	8(ebp),ax
	addl	8(ebp),eax

|*
|*	Instrução XADD
|*
	xaddb	ah,8(ebp)
	xaddw	ax,8(ebp)
	xaddl	eax,8(ebp)

	xaddb	bh,8(ebp)
	xaddw	bx,8(ebp)
	xaddl	ebx,8(ebp)

|*
|*	Instrução AND
|*
	.global	epsilon1
epsilon1:
	andb	#0x12,al
	andw	#0x1234,ax
	andl	#0x123456,eax
	andb	#0x12,8(ebp)
	andw	#0x1234,8(ebp)
	andl	#0x123456,8(ebp)
	andw	#0x12,8(ebp)
	andl	#0x12,8(ebp)
	andb	ah,8(ebp)
	andw	ax,8(ebp)
	andl	eax,8(ebp)
	andb	8(ebp),ah
	andw	8(ebp),ax
	andl	8(ebp),eax

|*
|*	Instrução CMP
|*
	.global	epsilon2
epsilon2:
	cmpb	#0x12,al
	cmpw	#0x1234,ax
	cmpl	#0x123456,eax
	cmpb	#0x12,8(ebp)
	cmpw	#0x1234,8(ebp)
	cmpl	#0x123456,8(ebp)
	cmpw	#0x12,8(ebp)
	cmpl	#0x12,8(ebp)
	cmpb	ah,8(ebp)
	cmpw	ax,8(ebp)
	cmpl	eax,8(ebp)
	cmpb	8(ebp),ah
	cmpw	8(ebp),ax
	cmpl	8(ebp),eax

|*
|*	Instrução OR
|*
	.global	epsilon3
epsilon3:
	orb	#0x12,al
	orw	#0x1234,ax
	orl	#0x123456,eax
	orb	#0x12,8(ebp)
	orw	#0x1234,8(ebp)
	orl	#0x123456,8(ebp)
	orw	#0x12,8(ebp)
	orl	#0x12,8(ebp)
	orb	ah,8(ebp)
	orw	ax,8(ebp)
	orl	eax,8(ebp)
	orb	8(ebp),ah
	orw	8(ebp),ax
	orl	8(ebp),eax

|*
|*	Instrução SBB
|*
	.global	epsilon4
epsilon4:
	sbbb	#0x12,al
	sbbw	#0x1234,ax
	sbbl	#0x123456,eax
	sbbb	#0x12,8(ebp)
	sbbw	#0x1234,8(ebp)
	sbbl	#0x123456,8(ebp)
	sbbw	#0x12,8(ebp)
	sbbl	#0x12,8(ebp)
	sbbb	ah,8(ebp)
	sbbw	ax,8(ebp)
	sbbl	eax,8(ebp)
	sbbb	8(ebp),ah
	sbbw	8(ebp),ax
	sbbl	8(ebp),eax

|*
|*	Instrução SUBL
|*
	.global	gamma5
gamma5:
	subl	#12,ra
	subl	#12,eax
	subl	#12,%eax
	subl	$8,%esp
	subl	#1200,ra
	subl	#12,rb
	subl	#1200,rb
	subl	#12,8(fp)
	subl	$1200,8(fp)
	subl	$1200,8(fp,rb@l)
	subl	#1200,8(%ebp,%ebx@l)
	subl	#1200,beta(%ebp,%ebx@l)
	subl	#1200,muito_longe(%ebp,%ebx@l)
	subl	#1200,alpha
	subl	#1200,beta
	subl	#1200,muito_longe

	subl	ra,rb

	subl	rb,8(fp)
	subl	rb,1024(fp)

	subl	rb,8(fp,esi)
	subl	rb,1024(fp,esi)
	subl	rb,8(fp,esi@l)
	subl	rb,alpha
	subl	rb,beta
	subl	rb,muito_longe

	subl	8(fp),rb
	subl	1024(fp),rb
	subl	8(fp,esi),rb
	subl	1024(fp,esi),rb
	subl	8(fp,esi@l),rb
	subl	alpha,rb
	subl	beta,rb
	subl	muito_longe,rb

|*
|*	Instrução SUBW
|*
	.global	gamma6
gamma6:
	subw	#12,ra
	subw	#12,eax
	subw	#12,%eax
	subw	#1200,ra

	subw	#12,rb
	subw	#1200,rb
	subw	#12,8(fp)
	subw	$1200,8(fp)
	subw	$1200,8(fp,rb@l)
	subw	#1200,8(%ebp,%ebx@l)
	subw	#1200,beta(%ebp,%ebx@l)
	subw	#1200,mais_longe_ainda(%ebp,%ebx@l)
	subw	#1200,alpha
	subw	#1200,beta
	subw	#1200,mais_longe_ainda

	subw	ra,rb

	subw	rb,8(fp)
	subw	rb,1024(fp)

	subw	rb,8(fp,esi)
	subw	rb,1024(fp,esi)
	subw	rb,8(fp,esi@l)
	subw	rb,alpha
	subw	rb,beta
	subw	rb,mais_longe_ainda

	subw	8(fp),rb
	subw	1024(fp),rb
	subw	8(fp,esi),rb
	subw	1024(fp,esi),rb
	subw	8(fp,esi@l),rb
	subw	alpha,rb
	subw	beta,rb
	subw	mais_longe_ainda,rb

|*
|*	Instrução SUBB
|*
	.global	gamma7
gamma7:
	subb	#12,ra
	subb	#12,eax
	subb	#12,%eax

	subb	#12,rb
	subb	#12,8(fp)
	subb	$12,8(fp)
	subb	$12,8(fp,rb@l)
	subb	#12,8(%ebp,%ebx@l)
	subb	#12,beta(%ebp,%ebx@l)
	subb	#12,mais_longe_ainda(%ebp,%ebx@l)
	subb	#12,alpha
	subb	#12,beta
	subb	#12,somente_longe

	subb	ra,rb

	subb	rb,8(fp)
	subb	rb,1024(fp)

	subb	rb,8(fp,esi)
	subb	rb,1024(fp,esi)
	subb	rb,8(fp,esi@l)
	subb	rb,alpha
	subb	rb,beta
	subb	rb,somente_longe

	subb	8(fp),rb
	subb	1024(fp),rb
	subb	8(fp,esi),rb
	subb	1024(fp,esi),rb
	subb	8(fp,esi@l),rb
	subb	alpha,rb
	subb	beta,rb
	subb	somente_longe,rb

|*
|*	Instrução XOR
|*
	.global	epsilon5
epsilon5:
	xorb	#0x12,al
	xorw	#0x1234,ax
	xorl	#0x123456,eax
	xorb	#0x12,8(ebp)
	xorw	#0x1234,8(ebp)
	xorl	#0x123456,8(ebp)
	xorw	#0x12,8(ebp)
	xorl	#0x12,8(ebp)
	xorb	ah,8(ebp)
	xorw	ax,8(ebp)
	xorl	eax,8(ebp)
	xorb	8(ebp),ah
	xorw	8(ebp),ax
	xorl	8(ebp),eax

	eorb	#0x12,8(ebp)
	eorw	#0x1234,8(ebp)
	eorl	#0x123456,8(ebp)

|*
|*
|*
	.global	gamma8
gamma8:
	subl	#12,sp
	subl	#12,8(fp)
	subl	#12,ra

	subl	#1200,sp
	subl	#1200,8(fp)
	subl	#1200,ra

	subl	#8,esp
	subl	#12,%esp
	subl	#12,esp
	subl	#12,sp
	subl	#8,8(fp)
	subl	#8,8(%fp)
	subl	#8,8(fp,eax)
	subl	#8,8(fp,%eax)
	subl	#8,8(fp,%eax@l)
	subl	#8,8(fp,%eax@q)
	subl	$12,%esp

|*
|*	Instrução PUSH
|*
	.global	gamma3
gamma3:
	pushw	8(fp)
	pushl	8(fp)
	pushw	ra
	pushl	ra
	pushl	fp
	pushl	%ebp
	pushw	#12
	pushl	#12
	pushw	#0x1234
	pushl	#0x1234

	pushl	es
	pushl	cs
	pushl	ss
	pushl	ds
	pushl	fs
	pushl	gs

|*
|*	Instrução CLR
|*
	clrb	r0
	clrb	r1
	clrb	r2
	clrb	r3
	clrb	h0
	clrb	h1
	clrb	h2
	clrb	h3

	clrw	r2

	clrl	r0
	clrl	r1
	clrl	r2
	clrl	r3
	clrl	r4
	clrl	r5
	clrl	fp
	clrl	sp

	clrb	8(fp)
	movb	#0,8(fp)
	clrw	8(fp)
	movw	#0,8(fp)
	clrl	8(fp)
	movl	#0,8(fp)
	clrl	final
	movl	#0,final

	clrl	r0
	movl	r0,8(fp)

|*
|*	SEG
|*
	seg	es
	seg	cs
	seg	ss
	seg	ds
	seg	fs
	seg	gs

|*
|*	Teste de ESPRESSÕES
|*
	movl	#0xA0|0x0B,r0
	movl	#0xAF^0x0B,r0
	movl	#0xAF&0x0B,r0
	movl	#0x0F00>>4,r0
	movl	#0x0F00<<4,r0
	movl	#12+4,r0
	movl	#final+4,r0
	movl	#4+final,r0
	movl	#final-4,r0
	movl	#12*5,r0
	movl	#13/5,r0
	movl	#13%5,r0

	movl	#-13,r0
	movl	#+13,r0
	movl	#~13,r0

	movl	#{-13},r0
	movl	#{+13},r0
	movl	#{~13},r0

	movl	#{100 + ~13},r0

|*
|*	Alguns testes do NEEDED
|*
	lsll	#1,r0
	lsll	#3,r0
	lsll	#um,r0
	lsll	#tres,r0

um	= 1
tres	= 3

	lsll	#um,r0
	lsll	#tres,r0
	lsll	#externo,r0


gamma	= 16

	pushl	#gamma
	pushw	#gamma

	pushl	#alphabeto

	.seg16
	pushw	#alphabeto

	.seg32
	pushl	#beta
	pushl	#12
	pushl	#1200
tst_label:
	.byte	0

	link	#alphabeto

	movl	alphabeto(fp),r0

alphabeto	= 10

	movl	alphabeto(fp),r0

	link	#alphabeto

	link	#126
	link	#127
	link	#128
	link	#129

	addl	#argv-.end,r0
.end:
	movl	#argv-.end,r0
	pushl	#argv-.end
argv:
	pushl	#argv-.end

|*
|*	Final
|*
final:
	subl	#1,(eax)
