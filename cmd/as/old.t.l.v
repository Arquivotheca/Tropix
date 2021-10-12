       0000 1234        alpha	= 0x1234
                        
                        	.data
000B88 0000 0000        	.long	0
                        	.global	beta
000B8C                  beta:
000B8C 0000 0000        	.long	0
                        	.text
                        
                        |*
                        |*	XCHG
                        |*
000000 86 45 08         	xchgb	8(fp),r0
000003 86 D8            	xchgb	r3,r0
000005 86 45 08         	xchgb	r0,8(fp)
                        
000008 66 87 45 08      	xchgw	8(fp),r0
00000C 66 93            	xchgw	r3,r0
00000E 66 93            	xchgw	r0,r3
000010 66 87 45 08      	xchgw	r0,8(fp)
000014 66 87 5D 08      	xchgw	8(fp),r3
000018 66 87 D9         	xchgw	r3,r1
00001B 66 87 5D 08      	xchgw	r3,8(fp)
                        
00001F 87 45 08         	xchgl	8(fp),r0
000022 93               	xchgl	r3,r0
000023 93               	xchgl	r0,r3
000024 87 45 08         	xchgl	r0,8(fp)
000027 87 5D 08         	xchgl	8(fp),r3
00002A 87 D9            	xchgl	r3,r1
00002C 87 5D 08         	xchgl	r3,8(fp)
                        
                        |*
                        |*	TST
                        |*
00002F 84 C0            	tstb	r0
000031 84 C9            	tstb	r1
000033 84 D2            	tstb	r2
000035 84 DB            	tstb	r3
000037 84 E4            	tstb	h0
000039 84 ED            	tstb	h1
00003B 84 F6            	tstb	h2
00003D 84 FF            	tstb	h3
                        
00003F 66 85 D2         	tstw	r2
                        
000042 85 C0            	tstl	r0
000044 85 C9            	tstl	r1
000046 85 D2            	tstl	r2
000048 85 DB            	tstl	r3
00004A 85 F6            	tstl	r4
00004C 85 FF            	tstl	r5
00004E 85 ED            	tstl	fp
000050 85 E4            	tstl	sp
                        
000052 A8 23            	testb	#0x23,r0
000054 F6 C3 23         	testb	#0x23,r3
000057 F6 45 08 23      	testb	#0x23,8(fp)
00005B 84 4D 08         	testb	r1,8(fp)
                        
00005E 66 A9 0023       	testw	#0x23,r0
000062 66 F7 C3 0023    	testw	#0x23,r3
000067 66 F7 45 08 0023 	testw	#0x23,8(fp)
00006D 66 85 4D 08      	testw	r1,8(fp)
                        
000071 A9 0000 0023     	testl	#0x23,r0
000076 F7 C3 0000 0023  	testl	#0x23,r3
00007C F7 45 08 0000 0023	testl	#0x23,8(fp)
000083 85 4D 08         	testl	r1,8(fp)
                        
                        |*
                        |*	SLDT e outras
                        |*
000086 0F00 45 08       	sldt	8(fp)
00008A 0F01 65 08       	smsw	8(fp)
00008E 0F00 4D 08       	str	8(fp)
000092 0F00 65 08       	verr	8(fp)
000096 0F00 6D 08       	verw	8(fp)
                        
                        |*
                        |*	SHLD/SHRD
                        |*
00009A 66 0FA4 55 08 07 	shldw	#7,r2,8(fp)
0000A0 66 0FA5 55 08    	shldw	cl,r2,8(fp)
0000A5 0FA4 55 08 07    	shldl	#7,r2,8(fp)
0000AA 0FA5 55 08       	shldl	cl,r2,8(fp)
                        
0000AE 66 0FAC 55 08 07 	shrdw	#7,r2,8(fp)
0000B4 66 0FAD 55 08    	shrdw	cl,r2,8(fp)
0000B9 0FAC 55 08 07    	shrdl	#7,r2,8(fp)
0000BE 0FAD 55 08       	shrdl	cl,r2,8(fp)
                        
                        |*
                        |*	LGDT/LIDT
                        |*
0000C2 0F01 45 08       	sgdt	8(fp)
0000C6 0F01 4D 08       	sidt	8(fp)
                        
                        |*
                        |*	SETcc
                        |*
0000CA 0F90 45 08       	setvs	8(fp)
0000CE 0F91 45 08       	setvc	8(fp)
0000D2 0F92 45 08       	setlo	8(fp)
0000D6 0F92 45 08       	setcs	8(fp)
0000DA 0F93 45 08       	seths	8(fp)
0000DE 0F93 45 08       	setcc	8(fp)
0000E2 0F94 45 08       	seteq	8(fp)
0000E6 0F95 45 08       	setne	8(fp)
0000EA 0F96 45 08       	setls	8(fp)
0000EE 0F97 45 08       	sethi	8(fp)
0000F2 0F98 45 08       	setmi	8(fp)
0000F6 0F99 45 08       	setpl	8(fp)
0000FA 0F9A 45 08       	setp	8(fp)
0000FE 0F9A 45 08       	setpe	8(fp)
000102 0F9B 45 08       	setpo	8(fp)
000106 0F9C 45 08       	setlt	8(fp)
00010A 0F9D 45 08       	setge	8(fp)
00010E 0F9E 45 08       	setle	8(fp)
000112 0F9F 45 08       	setgt	8(fp)
                        
                        |*
                        |*	SHIFT
                        |*
000116 D0 65 08         	aslb	#1,8(fp)
000119 D0 65 08         	salb	#1,8(fp)
00011C D2 65 08         	salb	cl,8(fp)
00011F C0 65 08 07      	salb	#7,8(fp)
000123 66 D1 65 08      	aslw	#1,8(fp)
000127 66 D1 65 08      	salw	#1,8(fp)
00012B 66 D3 65 08      	salw	cl,8(fp)
00012F 66 C1 65 08 07   	salw	#7,8(fp)
000134 D1 65 08         	asll	#1,8(fp)
000137 D1 65 08         	sall	#1,8(fp)
00013A D3 65 08         	sall	cl,8(fp)
00013D C1 65 08 07      	sall	#7,8(fp)
                        
000141 D0 7D 08         	asrb	#1,8(fp)
000144 D0 7D 08         	sarb	#1,8(fp)
000147 D2 7D 08         	sarb	cl,8(fp)
00014A C0 7D 08 07      	sarb	#7,8(fp)
00014E 66 D1 7D 08      	asrw	#1,8(fp)
000152 66 D1 7D 08      	sarw	#1,8(fp)
000156 66 D3 7D 08      	sarw	cl,8(fp)
00015A 66 C1 7D 08 07   	sarw	#7,8(fp)
00015F D1 7D 08         	asrl	#1,8(fp)
000162 D1 7D 08         	sarl	#1,8(fp)
000165 D3 7D 08         	sarl	cl,8(fp)
000168 C1 7D 08 07      	sarl	#7,8(fp)
                        
00016C D0 65 08         	lslb	#1,8(fp)
00016F D0 65 08         	shlb	#1,8(fp)
000172 D2 65 08         	shlb	cl,8(fp)
000175 C0 65 08 07      	shlb	#7,8(fp)
000179 66 D1 65 08      	lslw	#1,8(fp)
00017D 66 D1 65 08      	shlw	#1,8(fp)
000181 66 D3 65 08      	shlw	cl,8(fp)
000185 66 C1 65 08 07   	shlw	#7,8(fp)
00018A D1 65 08         	lsll	#1,8(fp)
00018D D1 65 08         	shll	#1,8(fp)
000190 D3 65 08         	shll	cl,8(fp)
000193 C1 65 08 07      	shll	#7,8(fp)
                        
000197 D0 6D 08         	lsrb	#1,8(fp)
00019A D0 6D 08         	shrb	#1,8(fp)
00019D D2 6D 08         	shrb	cl,8(fp)
0001A0 C0 6D 08 07      	shrb	#7,8(fp)
0001A4 66 D1 6D 08      	lsrw	#1,8(fp)
0001A8 66 D1 6D 08      	shrw	#1,8(fp)
0001AC 66 D3 6D 08      	shrw	cl,8(fp)
0001B0 66 C1 6D 08 07   	shrw	#7,8(fp)
0001B5 D1 6D 08         	lsrl	#1,8(fp)
0001B8 D1 6D 08         	shrl	#1,8(fp)
0001BB D3 6D 08         	shrl	cl,8(fp)
0001BE C1 6D 08 07      	shrl	#7,8(fp)
                        
                        |*
                        |*	ROTATE
                        |*
0001C2 D0 55 08         	rclb	#1,8(fp)
0001C5 D2 55 08         	rclb	cl,8(fp)
0001C8 C0 55 08 07      	rclb	#7,8(fp)
0001CC 66 D1 55 08      	rclw	#1,8(fp)
0001D0 66 D3 55 08      	rclw	cl,8(fp)
0001D4 66 C1 55 08 07   	rclw	#7,8(fp)
0001D9 D1 55 08         	rcll	#1,8(fp)
0001DC D3 55 08         	rcll	cl,8(fp)
0001DF C1 55 08 07      	rcll	#7,8(fp)
                        
0001E3 D0 5D 08         	rcrb	#1,8(fp)
0001E6 D2 5D 08         	rcrb	cl,8(fp)
0001E9 C0 5D 08 07      	rcrb	#7,8(fp)
0001ED 66 D1 5D 08      	rcrw	#1,8(fp)
0001F1 66 D3 5D 08      	rcrw	cl,8(fp)
0001F5 66 C1 5D 08 07   	rcrw	#7,8(fp)
0001FA D1 5D 08         	rcrl	#1,8(fp)
0001FD D3 5D 08         	rcrl	cl,8(fp)
000200 C1 5D 08 07      	rcrl	#7,8(fp)
                        
000204 D0 45 08         	rolb	#1,8(fp)
000207 D2 45 08         	rolb	cl,8(fp)
00020A C0 45 08 07      	rolb	#7,8(fp)
00020E 66 D1 45 08      	rolw	#1,8(fp)
000212 66 D3 45 08      	rolw	cl,8(fp)
000216 66 C1 45 08 07   	rolw	#7,8(fp)
00021B D1 45 08         	roll	#1,8(fp)
00021E D3 45 08         	roll	cl,8(fp)
000221 C1 45 08 07      	roll	#7,8(fp)
                        
000225 D0 4D 08         	rorb	#1,8(fp)
000228 D2 4D 08         	rorb	cl,8(fp)
00022B C0 4D 08 07      	rorb	#7,8(fp)
00022F 66 D1 4D 08      	rorw	#1,8(fp)
000233 66 D3 4D 08      	rorw	cl,8(fp)
000237 66 C1 4D 08 07   	rorw	#7,8(fp)
00023C D1 4D 08         	rorl	#1,8(fp)
00023F D3 4D 08         	rorl	cl,8(fp)
000242 C1 4D 08 07      	rorl	#7,8(fp)
                        
                        |*
                        |*	OUT
                        |*
000246 E6 08            	outb	ra,#8
000248 66 E7 08         	outw	ra,#8
00024B E7 08            	outl	ra,#8
                        
00024D EE               	outb	ra,edx
00024E 66 EF            	outw	ra,edx
000250 EF               	outl	ra,edx
                        
                        |*
                        |*	NEG
                        |*
000251 F6 5D 08         	negb	8(fp)
000254 66 F7 5D 08      	negw	8(fp)
000258 F7 5D 08         	negl	8(fp)
00025B F7 D8            	negl	r0
                        
                        |*
                        |*	NOT
                        |*
00025D F6 55 08         	notb	8(fp)
000260 66 F7 55 08      	notw	8(fp)
000264 F7 55 08         	notl	8(fp)
000267 F7 D0            	notl	r0
                        
                        |*
                        |*	MOVx - moves exóticos
                        |*
000269 8E 45 08         	movw	8(fp),es
00026C 8E 4D 08         	movw	8(fp),cs
00026F 8E 55 08         	movw	8(fp),ss
000272 8E 5D 08         	movw	8(fp),ds
000275 8E 65 08         	movw	8(fp),fs
000278 8E 6D 08         	movw	8(fp),gs
00027B 8E E8            	movw	r0,gs
                        
00027D 8C 45 08         	movw	es,8(fp)
000280 8C 4D 08         	movw	cs,8(fp)
000283 8C 55 08         	movw	ss,8(fp)
000286 8C 5D 08         	movw	ds,8(fp)
000289 8C 65 08         	movw	fs,8(fp)
00028C 8C 6D 08         	movw	gs,8(fp)
00028F 8C E8            	movw	gs,r0
                        
000291 0F20 C3          	movl	cr0,r3
000294 0F20 D3          	movl	cr2,r3
000297 0F20 DB          	movl	cr3,r3
                        
00029A 0F22 C1          	movl	r1,cr0
00029D 0F22 D1          	movl	r1,cr2
0002A0 0F22 D9          	movl	r1,cr3
                        
0002A3 0F21 C1          	movl	dr0,r1
0002A6 0F21 C9          	movl	dr1,r1
0002A9 0F21 D1          	movl	dr2,r1
0002AC 0F21 D9          	movl	dr3,r1
0002AF 0F21 F1          	movl	dr6,r1
0002B2 0F21 F9          	movl	dr7,r1
                        
0002B5 0F23 C2          	movl	r2,dr0
0002B8 0F23 CA          	movl	r2,dr1
0002BB 0F23 D2          	movl	r2,dr2
0002BE 0F23 DA          	movl	r2,dr3
0002C1 0F23 F2          	movl	r2,dr6
0002C4 0F23 FA          	movl	r2,dr7
                        
0002C7 0F24 F4          	movl	tr6,sp
0002CA 0F24 FC          	movl	tr7,sp
                        
0002CD 0F26 F2          	movl	r2,tr6
0002D0 0F26 FA          	movl	r2,tr7
                        
                        |*
                        |*	LTR
                        |*
0002D3 0F00 5D 08       	ltr	8(fp)
                        
                        |*
                        |*	LSL => LSEGL
                        |*
0002D7 66 0F03 55 08    	lseglw	8(fp),r2
0002DC 0F03 55 08       	lsegll	8(fp),r2
                        
                        |*
                        |*	LOOP
                        |*
0002E0 E212             	loop	1$
0002E2 E110             	loopeq	1$
0002E4 E10E             	loope	1$
0002E6 E10C             	loopz	1$
0002E8 E00A             	loopne	1$
0002EA E008             	loopnz	1$
                        
                        |*
                        |*	LLDT, LMSW
                        |*
0002EC 0F00 55 08       	lldt	8(fp)
0002F0 0F01 75 08       	lmsw	8(fp)
                        
                        |*
                        |*	LGS, ...
                        |*
0002F4                  1$:
0002F4 66 C5 75 08      	ldsw	8(fp),r4
0002F8 66 0FB2 75 08    	lssw	8(fp),r4
0002FD 66 C4 75 08      	lesw	8(fp),r4
000301 66 0FB4 75 08    	lfsw	8(fp),r4
000306 66 0FB5 75 08    	lgsw	8(fp),r4
00030B C5 75 08         	ldsl	8(fp),r4
00030E 0FB2 75 08       	lssl	8(fp),r4
000312 C4 75 08         	lesl	8(fp),r4
000315 0FB4 75 08       	lfsl	8(fp),r4
000319 0FB5 75 08       	lgsl	8(fp),r4
                        
                        |*
                        |*	LGDT/LIDT
                        |*
00031D 66 0F01 55 08    	lgdtw	8(fp)
000322 66 0F01 5D 08    	lidtw	8(fp)
000327 0F01 55 08       	lgdtl	8(fp)
00032B 0F01 5D 08       	lidtl	8(fp)
                        
                        |*
                        |*	LAR
                        |*
00032F 66 0F02 55 08    	larw	8(fp),r2
000334 0F02 55 08       	larl	8(fp),r2
                        
                        |*
                        |*	Multiplicação
                        |*
000338 F6 6D 08         	mulsb	8(fp)
00033B F6 6D 08         	mulsb	8(fp),r0
                        
00033E 66 F7 6D 08      	mulsw	8(fp)
000342 66 0FAF 45 08    	mulsw	8(fp),r0
000347 66 0FAF 55 08    	mulsw	8(fp),r2
00034C 66 6B D2 12      	mulsw	#0x12,r2
000350 66 6B D2 12      	mulsw	#0x12,r2,r2
000354 66 6B F2 12      	mulsw	#0x12,r2,r4
000358 66 6B 4D 08 12   	mulsw	#0x12,8(fp),r1
00035D 66 69 D2 1234    	mulsw	#0x1234,r2
000362 66 69 D2 1234    	mulsw	#0x1234,r2,r2
000367 66 69 4D 08 1234 	mulsw	#0x1234,8(fp),r1
00036D 66 69 4D 08 1234 	imulw	#0x1234,8(fp),r1
                        
000373 F7 6D 08         	mulsl	8(fp)
000376 0FAF 45 08       	mulsl	8(fp),r0
00037A 0FAF 55 08       	mulsl	8(fp),r2
00037E 6B D2 12         	mulsl	#0x12,r2
000381 6B D2 12         	mulsl	#0x12,r2,r2
000384 6B F2 12         	mulsl	#0x12,r2,r4
000387 6B 4D 08 12      	mulsl	#0x12,8(fp),r1
00038B 69 D2 0000 1234  	mulsl	#0x1234,r2
000391 69 D2 0000 1234  	mulsl	#0x1234,r2,r2
000397 69 4D 08 0000 1234	mulsl	#0x1234,8(fp),r1
00039E 69 4D 08 0000 1234	imull	#0x1234,8(fp),r1
                        
0003A5 F6 65 08         	mulub	8(fp),r0
0003A8 66 F7 65 08      	muluw	8(fp),r0
0003AC F7 65 08         	mulul	8(fp),r0
0003AF F7 65 08         	mull	8(fp),r0
                        
                        |*
                        |*	In
                        |*
0003B2 E4 08            	inb	#8,r0
0003B4 66 E5 08         	inw	#8,r0
0003B7 E5 08            	inl	#8,r0
0003B9 EC               	inb	r2,r0
0003BA 66 ED            	inw	r2,r0
0003BC ED               	inl	r2,r0
0003BD ED               	inl	dx,r0
                        
                        |*
                        |*	Invlpg
                        |*
0003BE 0F01 38          	invlpg	(r0)
0003C1 0F01 39          	invlpg	(r1)
0003C4 0F01 7D 08       	invlpg	8(fp)
0003C8 0F01 3D 0000 1234	invlpg	alpha
                        
                        |*
                        |*	Bswap
                        |*
0003CF 0FC8             	bswap	r0
0003D1 0FC9             	bswap	r1
0003D3 0FCF             	bswapl	r5
                        
                        |*
                        |*	Enter
                        |*
0003D5 C8 2710 03       	enter	#10000,#3
0003D9 C8 2710 03       	enter	#10000,#3
                        
                        |*
                        |*	Multiplicação/divisão
                        |*
0003DD F6 75 08         	divub	8(fp)
0003E0 F6 75 08         	divub	8(fp),r0
0003E3 F6 75 08         	divb	8(fp)
0003E6 F6 75 08         	divb	8(fp),r0
                        
0003E9 66 F7 75 08      	divuw	8(fp)
0003ED 66 F7 75 08      	divuw	8(fp),r0
0003F1 66 F7 75 08      	divw	8(fp)
0003F5 66 F7 75 08      	divw	8(fp),r0
                        
0003F9 F7 75 08         	divul	8(fp)
0003FC F7 75 08         	divul	8(fp),r0
0003FF F7 75 08         	divl	8(fp)
000402 F7 75 08         	divl	8(fp),r0
                        
000405 F6 7D 08         	divsb	8(fp)
000408 F6 7D 08         	divsb	8(fp),r0
00040B F6 7D 08         	idivb	8(fp)
00040E F6 7D 08         	idivb	8(fp),r0
                        
000411 66 F7 7D 08      	divsw	8(fp)
000415 66 F7 7D 08      	divsw	8(fp),r0
000419 66 F7 7D 08      	idivw	8(fp)
00041D 66 F7 7D 08      	idivw	8(fp),r0
                        
000421 F7 7D 08         	divsl	8(fp)
000424 F7 7D 08         	divsl	8(fp),r0
000427 F7 7D 08         	idivl	8(fp)
00042A F7 7D 08         	idivl	8(fp),r0
                        
                        |*
                        |*	Incremento/decremento
                        |*
00042D                  12$:
00042D FE C9            	decb	r1
00042F 66 49            	decw	r1
000431 49               	decl	r1
000432 FE 4D 08         	decb	8(fp)
000435 66 FF 4D 08      	decw	8(fp)
000439 FF 4D 08         	decl	8(fp)
                        
00043C FE C2            	incb	r2
00043E 66 42            	incw	r2
000440 42               	incl	r2
000441 FE 45 08         	incb	8(fp)
000444 66 FF 45 08      	incw	8(fp)
000448 FF 45 08         	incl	8(fp)
                        
                        |*
                        |*	Interrupções
                        |*
00044B                  2$:
00044B CC               	int3
00044C CE               	into
00044D CD 22            	int	#34
                        
                        |*
                        |*	Instruções sem operandos, de 1 byte
                        |*
00044F                  3$:
00044F 37               	aaa
000450 3F               	aas
000451 66 98            	cbw
000453 98               	cwde
000454 F8               	clc
000455 FC               	cld
000456 FA               	intdis
000457 FA               	disint
000458 FA               	cli
000459 F5               	cmc
00045A A6               	cmpsb
00045B 66 A7            	cmpsw
00045D A7               	cmpsl
00045E A7               	cmpsd
00045F 66 99            	cwd
000461 99               	cdq
000462 27               	daa
000463 2F               	das
000464 6C               	insb
000465 66 6D            	insw
000467 6D               	insl
000468 6D               	insd
000469 F4               	hlt
                        
                        |*
                        |*	Instrução IRET
                        |*
00046A 66 CF            	iretw
00046C CF               	rte
00046D CF               	iretl
00046E CF               	iretd
                        
00046F 9F               	lahf
000470 C9               	leave
000471 F0               	lock
000472 AC               	lodsb
000473 66 AD            	lodsw
000475 AD               	lodsl
000476 AD               	lodsd
000477 A4               	movsb
000478 66 A5            	movsw
00047A A5               	movsl
00047B A5               	movsd
00047C 90               	nop
00047D 6E               	outsb
00047E 66 6F            	outsw
000480 6F               	outsl
000481 6F               	outsd
000482 66 61            	popaw
000484 61               	popal
000485 61               	popad
000486 66 9D            	popfw
000488 9D               	popfl
000489 9D               	popfd
00048A 66 60            	pushaw
00048C 60               	pushal
00048D 60               	pushad
00048E 66 9C            	pushfw
000490 9C               	pushfl
000491 9C               	pushfd
000492 F3               	rep
000493 F3               	repe
000494 F3               	repeq
000495 F3               	repz
000496 F2               	repne
000497 F2               	repnz
000498 9E               	sahf
000499 F9               	stc
00049A FD               	std
00049B FB               	inten
00049C FB               	enint
00049D FB               	sti
00049E 9B               	wait
00049F AA               	stosb
0004A0 66 AB            	stosw
0004A2 AB               	stosl
0004A3 AB               	stosd
0004A4 D7               	xlat
0004A5 D7               	xlatb
                        
                        |*
                        |*	Instruções sem operandos, de 2 bytes
                        |*
0004A6                  4$:
0004A6 D50A             	aad
0004A8 D40A             	aam
0004AA 0F06             	clts
0004AC 0FA2             	cpuid
0004AE 0F31             	rdtsc
0004B0 0F08             	invd
0004B2 0F32             	rdmsr
0004B4 0FAA             	rsm
0004B6 0F09             	wbinvd
0004B8 0F30             	wrmsr
                        
                        |*
                        |******* Outras instruções com sintaxe do grupo 1 ***************
                        |*
0004BA                  5$:
0004BA 63 4D 08         	arpl	r1,8(fp)
0004BD 66 62 4D 08      	boundw	8(fp),r1
0004C1 62 4D 08         	boundl	8(fp),r1
                        
                        |*
                        |****** Operações de BITs ***************************************
                        |*
0004C4                  6$:
0004C4 66 0FBC 4D 08    	bsfw	8(fp),r1
0004C9 0FBC 4D 08       	bsfl	8(fp),r1
0004CD 66 0FBD 4D 08    	bsrw	8(fp),r1
0004D2 0FBD 4D 08       	bsrl	8(fp),r1
0004D6 66 0FA3 4D 08    	btw	r1,8(fp)
0004DB 66 0FBA 65 08 0C 	btw	#12,8(fp)
0004E1 0FA3 4D 08       	btl	r1,8(fp)
0004E5 0FBA 65 08 0C    	btl	#12,8(fp)
0004EA 66 0FBB 4D 08    	btcw	r1,8(fp)
0004EF 66 0FBA 7D 08 0C 	btcw	#12,8(fp)
0004F5 0FBB 4D 08       	btcl	r1,8(fp)
0004F9 0FBA 7D 08 0C    	btcl	#12,8(fp)
0004FE 66 0FB3 4D 08    	btrw	r1,8(fp)
000503 66 0FBA 75 08 0C 	btrw	#12,8(fp)
000509 0FB3 4D 08       	btrl	r1,8(fp)
00050D 0FBA 75 08 0C    	btrl	#12,8(fp)
000512 66 0FAB 4D 08    	btsw	r1,8(fp)
000517 66 0FBA 6D 08 0C 	btsw	#12,8(fp)
00051D 0FAB 4D 08       	btsl	r1,8(fp)
000521 0FBA 6D 08 0C    	btsl	#12,8(fp)
                        
                        |*
                        |*	MOVs de conversão
                        |*
000526                  7$:
000526 66 0FBE 45 08    	movsbw	8(fp),r0
00052B 0FBE 45 08       	movsbl	8(fp),r0
00052F 0FBF 45 08       	movswl	8(fp),r0
                        
000533 66 0FB6 45 08    	movzbw	8(fp),r0
000538 0FB6 45 08       	movzbl	8(fp),r0
00053C 0FB7 45 08       	movzwl	8(fp),r0
                        
                        |*
                        |*	Instrução RET
                        |*
000540                  8$:
000540 C3               	rts
000541 C3               	ret
000542 CB               	farrts
000543 CB               	lret
                        
000544 C2 0009          	rtd	#9
000547 C2 1234          	rtd	#alpha
00054A CA 0009          	farrtd	#9
00054D CA 0009          	lrtd	#9
                        
                        |*
                        |*	Instrução POP
                        |*
000550                  9$:
000550 66 8F 45 08      	popw	8(fp)
000554 66 5D            	popw	fp
                        
000556 07               	popl	es
                        |*	popl	cs	|* Não pode
000557 17               	popl	ss
000558 1F               	popl	ds
000559 0FA1             	popl	fs
00055B 0FA9             	popl	gs
                        
00055D 8F 45 08         	popl	8(fp)
000560 5D               	popl	fp
                        
                        |*
                        |*	Instrução "lea"
                        |*
000561                  10$:
000561 8D 5D 08         	lea	8(fp),r3
000564 8D 05 0000 1234  	lea	alpha,r0
00056A 8D 05 0000 0000  	lea	undef,r0
000570 8D 640B 08       	lea	8(r3,r1),sp
                        
000574 8D 5D 08         	leal	8(fp),r3
000577 8D 05 0000 1234  	leal	alpha,r0
00057D 8D 05 0000 0000  	lea	undef,r0
000583 8D 640B 08       	leal	8(r3,r1),sp
                        
                        |*
                        |*	Instrução "movb"
                        |*
000587                  11$:
000587 88 5D 08         	movb	r3,8(fp)
00058A 88 7D 08         	movb	h3,8(fp)
00058D 8A 5D 08         	movb	8(fp),r3
000590 A0 0000 1234     	movb	alpha,r0
000595 A2 0000 1234     	movb	r0,alpha
00059A B3 12            	movb	#0x12,r3
00059C C6 440B 08 12    	movb	#0x12,8(r3,r1)
                        
                        |*
                        |*	Instrução "movw"
                        |*
                        	.global	gamma0
0005A1                  gamma0:
0005A1 66 89 5D 08      	movw	r3,8(fp)
0005A5 66 8B 5D 08      	movw	8(fp),r3
0005A9 66 A1 0000 1234  	movw	alpha,r0
0005AF 66 A3 0000 1234  	movw	r0,alpha
0005B5 66 BB 1234       	movw	#0x1234,r3
0005B9 66 C7 440B 08 1234	movw	#0x1234,8(r3,r1)
                        
                        |*
                        |*	Instrução "movl"
                        |*
                        	.global	gamma1
0005C0                  gamma1:
0005C0 89 5D 08         	movl	r3,8(fp)
0005C3 8B 5D 08         	movl	8(fp),r3
0005C6 A1 0000 1234     	movl	alpha,r0
0005CB A3 0000 1234     	movl	r0,alpha
0005D0 BB 0000 1234     	movl	#0x1234,r3
0005D5 C7 440B 08 0000  	movl	#0x1234,8(r3,r1)
       1234
                        
                        |*
                        |*	Estudo de registros high
                        |*
                        	.global	gamma2
0005DD                  gamma2:
0005DD 83 E8 0C         	subl	#12,ra
0005E0 66 83 E8 0C      	subw	#12,ra
0005E4 2C 0C            	subb	#12,ra
0005E6 2C 0C            	subb	#12,al
0005E8 80 EC 0C         	subb	#12,ah
                        
0005EB 83 EB 0C         	subl	#12,rb
0005EE 66 83 EB 0C      	subw	#12,rb
0005F2 80 EB 0C         	subb	#12,rb
0005F5 80 EB 0C         	subb	#12,bl
0005F8 80 EF 0C         	subb	#12,bh
                        
0005FB 83 EE 0C         	subl	#12,esi
0005FE 66 83 EE 0C      	subw	#12,si
                        
000602 2C 0C            	subb	#12,r0
000604 80 EB 0C         	subb	#12,r3
000607 80 E9 0C         	subb	#12,r1
00060A 80 EA 0C         	subb	#12,r2
                        |*	subb	#12,r4
                        |*	subb	#12,r5
                        |*	subb	#12,fp
                        |*	subb	#12,sp
                        
                        |*
                        |*	Instrução ADC
                        |*
                        	.global	epsilon0
00060D                  epsilon0:
00060D 14 12            	adcb	#0x12,al
00060F 66 15 1234       	adcw	#0x1234,ax
000613 15 0012 3456     	adcl	#0x123456,eax
000618 80 55 08 12      	adcb	#0x12,8(ebp)
00061C 66 81 55 08 1234 	adcw	#0x1234,8(ebp)
000622 81 55 08 0012 3456	adcl	#0x123456,8(ebp)
000629 66 83 55 08 12   	adcw	#0x12,8(ebp)
00062E 83 55 08 12      	adcl	#0x12,8(ebp)
000632 10 65 08         	adcb	ah,8(ebp)
000635 66 11 45 08      	adcw	ax,8(ebp)
000639 11 45 08         	adcl	eax,8(ebp)
00063C 12 65 08         	adcb	8(ebp),ah
00063F 66 13 45 08      	adcw	8(ebp),ax
000643 13 45 08         	adcl	8(ebp),eax
                        
                        |*
                        |*	Instrução ADD
                        |*
                        	.global	gamma4
000646                  gamma4:
000646 04 12            	addb	#0x12,al
000648 66 05 1234       	addw	#0x1234,ax
00064C 05 0012 3456     	addl	#0x123456,eax
000651 80 45 08 12      	addb	#0x12,8(ebp)
000655 66 81 45 08 1234 	addw	#0x1234,8(ebp)
00065B 81 45 08 0012 3456	addl	#0x123456,8(ebp)
000662 66 83 45 08 12   	addw	#0x12,8(ebp)
000667 83 45 08 12      	addl	#0x12,8(ebp)
00066B 00 65 08         	addb	ah,8(ebp)
00066E 66 01 45 08      	addw	ax,8(ebp)
000672 01 45 08         	addl	eax,8(ebp)
000675 02 65 08         	addb	8(ebp),ah
000678 66 03 45 08      	addw	8(ebp),ax
00067C 03 45 08         	addl	8(ebp),eax
                        
                        |*
                        |*	Instrução XADD
                        |*
00067F 0FC0 65 08       	xaddb	ah,8(ebp)
000683 66 0FC1 45 08    	xaddw	ax,8(ebp)
000688 0FC1 45 08       	xaddl	eax,8(ebp)
                        
00068C 0FC0 7D 08       	xaddb	bh,8(ebp)
000690 66 0FC1 5D 08    	xaddw	bx,8(ebp)
000695 0FC1 5D 08       	xaddl	ebx,8(ebp)
                        
                        |*
                        |*	Instrução AND
                        |*
                        	.global	epsilon1
000699                  epsilon1:
000699 24 12            	andb	#0x12,al
00069B 66 25 1234       	andw	#0x1234,ax
00069F 25 0012 3456     	andl	#0x123456,eax
0006A4 80 65 08 12      	andb	#0x12,8(ebp)
0006A8 66 81 65 08 1234 	andw	#0x1234,8(ebp)
0006AE 81 65 08 0012 3456	andl	#0x123456,8(ebp)
0006B5 66 83 65 08 12   	andw	#0x12,8(ebp)
0006BA 83 65 08 12      	andl	#0x12,8(ebp)
0006BE 20 65 08         	andb	ah,8(ebp)
0006C1 66 21 45 08      	andw	ax,8(ebp)
0006C5 21 45 08         	andl	eax,8(ebp)
0006C8 22 65 08         	andb	8(ebp),ah
0006CB 66 23 45 08      	andw	8(ebp),ax
0006CF 23 45 08         	andl	8(ebp),eax
                        
                        |*
                        |*	Instrução CMP
                        |*
                        	.global	epsilon2
0006D2                  epsilon2:
0006D2 3C 12            	cmpb	#0x12,al
0006D4 66 3D 1234       	cmpw	#0x1234,ax
0006D8 3D 0012 3456     	cmpl	#0x123456,eax
0006DD 80 7D 08 12      	cmpb	#0x12,8(ebp)
0006E1 66 81 7D 08 1234 	cmpw	#0x1234,8(ebp)
0006E7 81 7D 08 0012 3456	cmpl	#0x123456,8(ebp)
0006EE 66 83 7D 08 12   	cmpw	#0x12,8(ebp)
0006F3 83 7D 08 12      	cmpl	#0x12,8(ebp)
0006F7 38 65 08         	cmpb	ah,8(ebp)
0006FA 66 39 45 08      	cmpw	ax,8(ebp)
0006FE 39 45 08         	cmpl	eax,8(ebp)
000701 3A 65 08         	cmpb	8(ebp),ah
000704 66 3B 45 08      	cmpw	8(ebp),ax
000708 3B 45 08         	cmpl	8(ebp),eax
                        
                        |*
                        |*	Instrução OR
                        |*
                        	.global	epsilon3
00070B                  epsilon3:
00070B 0C 12            	orb	#0x12,al
00070D 66 0D 1234       	orw	#0x1234,ax
000711 0D 0012 3456     	orl	#0x123456,eax
000716 80 4D 08 12      	orb	#0x12,8(ebp)
00071A 66 81 4D 08 1234 	orw	#0x1234,8(ebp)
000720 81 4D 08 0012 3456	orl	#0x123456,8(ebp)
000727 66 83 4D 08 12   	orw	#0x12,8(ebp)
00072C 83 4D 08 12      	orl	#0x12,8(ebp)
000730 08 65 08         	orb	ah,8(ebp)
000733 66 09 45 08      	orw	ax,8(ebp)
000737 09 45 08         	orl	eax,8(ebp)
00073A 0A 65 08         	orb	8(ebp),ah
00073D 66 0B 45 08      	orw	8(ebp),ax
000741 0B 45 08         	orl	8(ebp),eax
                        
                        |*
                        |*	Instrução SBB
                        |*
                        	.global	epsilon4
000744                  epsilon4:
000744 1C 12            	sbbb	#0x12,al
000746 66 1D 1234       	sbbw	#0x1234,ax
00074A 1D 0012 3456     	sbbl	#0x123456,eax
00074F 80 5D 08 12      	sbbb	#0x12,8(ebp)
000753 66 81 5D 08 1234 	sbbw	#0x1234,8(ebp)
000759 81 5D 08 0012 3456	sbbl	#0x123456,8(ebp)
000760 66 83 5D 08 12   	sbbw	#0x12,8(ebp)
000765 83 5D 08 12      	sbbl	#0x12,8(ebp)
000769 18 65 08         	sbbb	ah,8(ebp)
00076C 66 19 45 08      	sbbw	ax,8(ebp)
000770 19 45 08         	sbbl	eax,8(ebp)
000773 1A 65 08         	sbbb	8(ebp),ah
000776 66 1B 45 08      	sbbw	8(ebp),ax
00077A 1B 45 08         	sbbl	8(ebp),eax
                        
                        |*
                        |*	Instrução SUBL
                        |*
                        	.global	gamma5
00077D                  gamma5:
00077D 83 E8 0C         	subl	#12,ra
000780 83 E8 0C         	subl	#12,eax
000783 83 E8 0C         	subl	#12,%eax
000786 83 EC 08         	subl	$8,%esp
000789 2D 0000 04B0     	subl	#1200,ra
00078E 83 EB 0C         	subl	#12,rb
000791 81 EB 0000 04B0  	subl	#1200,rb
000797 83 6D 08 0C      	subl	#12,8(fp)
00079B 81 6D 08 0000 04B0	subl	$1200,8(fp)
0007A2 81 6C9D 08 0000  	subl	$1200,8(fp,rb@l)
       04B0
0007AA 81 6C9D 08 0000  	subl	#1200,8(%ebp,%ebx@l)
       04B0
0007B2 81 AC9D 0000 0B8C	subl	#1200,beta(%ebp,%ebx@l)
       0000 04B0
0007BD 81 AC9D 0000 0000	subl	#1200,muito_longe(%ebp,%ebx@l)
       0000 04B0
0007C8 81 2D 0000 1234  	subl	#1200,alpha
       0000 04B0
0007D2 81 2D 0000 0B8C  	subl	#1200,beta
       0000 04B0
0007DC 81 2D 0000 0000  	subl	#1200,muito_longe
       0000 04B0
                        
0007E6 29 C3            	subl	ra,rb
                        
0007E8 29 5D 08         	subl	rb,8(fp)
0007EB 29 9D 0000 0400  	subl	rb,1024(fp)
                        
0007F1 29 5C35 08       	subl	rb,8(fp,esi)
0007F5 29 9C35 0000 0400	subl	rb,1024(fp,esi)
0007FC 29 5CB5 08       	subl	rb,8(fp,esi@l)
000800 29 1D 0000 1234  	subl	rb,alpha
000806 29 1D 0000 0B8C  	subl	rb,beta
00080C 29 1D 0000 0000  	subl	rb,muito_longe
                        
000812 2B 5D 08         	subl	8(fp),rb
000815 2B 9D 0000 0400  	subl	1024(fp),rb
00081B 2B 5C35 08       	subl	8(fp,esi),rb
00081F 2B 9C35 0000 0400	subl	1024(fp,esi),rb
000826 2B 5CB5 08       	subl	8(fp,esi@l),rb
00082A 2B 1D 0000 1234  	subl	alpha,rb
000830 2B 1D 0000 0B8C  	subl	beta,rb
000836 2B 1D 0000 0000  	subl	muito_longe,rb
                        
                        |*
                        |*	Instrução SUBW
                        |*
                        	.global	gamma6
00083C                  gamma6:
00083C 66 83 E8 0C      	subw	#12,ra
000840 66 83 E8 0C      	subw	#12,eax
000844 66 83 E8 0C      	subw	#12,%eax
000848 66 2D 04B0       	subw	#1200,ra
                        
00084C 66 83 EB 0C      	subw	#12,rb
000850 66 81 EB 04B0    	subw	#1200,rb
000855 66 83 6D 08 0C   	subw	#12,8(fp)
00085A 66 81 6D 08 04B0 	subw	$1200,8(fp)
000860 66 81 6C9D 08 04B0	subw	$1200,8(fp,rb@l)
000867 66 81 6C9D 08 04B0	subw	#1200,8(%ebp,%ebx@l)
00086E 66 81 AC9D 0000  	subw	#1200,beta(%ebp,%ebx@l)
       0B8C 04B0
000878 66 81 AC9D 0000  	subw	#1200,mais_longe_ainda(%ebp,%ebx@l)
       0000 04B0
000882 66 81 2D 0000 1234	subw	#1200,alpha
       04B0
00088B 66 81 2D 0000 0B8C	subw	#1200,beta
       04B0
000894 66 81 2D 0000 0000	subw	#1200,mais_longe_ainda
       04B0
                        
00089D 66 29 C3         	subw	ra,rb
                        
0008A0 66 29 5D 08      	subw	rb,8(fp)
0008A4 66 29 9D 0000 0400	subw	rb,1024(fp)
                        
0008AB 66 29 5C35 08    	subw	rb,8(fp,esi)
0008B0 66 29 9C35 0000  	subw	rb,1024(fp,esi)
       0400
0008B8 66 29 5CB5 08    	subw	rb,8(fp,esi@l)
0008BD 66 29 1D 0000 1234	subw	rb,alpha
0008C4 66 29 1D 0000 0B8C	subw	rb,beta
0008CB 66 29 1D 0000 0000	subw	rb,mais_longe_ainda
                        
0008D2 66 2B 5D 08      	subw	8(fp),rb
0008D6 66 2B 9D 0000 0400	subw	1024(fp),rb
0008DD 66 2B 5C35 08    	subw	8(fp,esi),rb
0008E2 66 2B 9C35 0000  	subw	1024(fp,esi),rb
       0400
0008EA 66 2B 5CB5 08    	subw	8(fp,esi@l),rb
0008EF 66 2B 1D 0000 1234	subw	alpha,rb
0008F6 66 2B 1D 0000 0B8C	subw	beta,rb
0008FD 66 2B 1D 0000 0000	subw	mais_longe_ainda,rb
                        
                        |*
                        |*	Instrução SUBB
                        |*
                        	.global	gamma7
000904                  gamma7:
000904 2C 0C            	subb	#12,ra
000906 2C 0C            	subb	#12,eax
000908 2C 0C            	subb	#12,%eax
                        
00090A 80 EB 0C         	subb	#12,rb
00090D 80 6D 08 0C      	subb	#12,8(fp)
000911 80 6D 08 0C      	subb	$12,8(fp)
000915 80 6C9D 08 0C    	subb	$12,8(fp,rb@l)
00091A 80 6C9D 08 0C    	subb	#12,8(%ebp,%ebx@l)
00091F 80 AC9D 0000 0B8C	subb	#12,beta(%ebp,%ebx@l)
       0C
000927 80 AC9D 0000 0000	subb	#12,mais_longe_ainda(%ebp,%ebx@l)
       0C
00092F 80 2D 0000 1234  	subb	#12,alpha
       0C
000936 80 2D 0000 0B8C  	subb	#12,beta
       0C
00093D 80 2D 0000 0000  	subb	#12,somente_longe
       0C
                        
000944 28 C3            	subb	ra,rb
                        
000946 28 5D 08         	subb	rb,8(fp)
000949 28 9D 0000 0400  	subb	rb,1024(fp)
                        
00094F 28 5C35 08       	subb	rb,8(fp,esi)
000953 28 9C35 0000 0400	subb	rb,1024(fp,esi)
00095A 28 5CB5 08       	subb	rb,8(fp,esi@l)
00095E 28 1D 0000 1234  	subb	rb,alpha
000964 28 1D 0000 0B8C  	subb	rb,beta
00096A 28 1D 0000 0000  	subb	rb,somente_longe
                        
000970 2A 5D 08         	subb	8(fp),rb
000973 2A 9D 0000 0400  	subb	1024(fp),rb
000979 2A 5C35 08       	subb	8(fp,esi),rb
00097D 2A 9C35 0000 0400	subb	1024(fp,esi),rb
000984 2A 5CB5 08       	subb	8(fp,esi@l),rb
000988 2A 1D 0000 1234  	subb	alpha,rb
00098E 2A 1D 0000 0B8C  	subb	beta,rb
000994 2A 1D 0000 0000  	subb	somente_longe,rb
                        
                        |*
                        |*	Instrução XOR
                        |*
                        	.global	epsilon5
00099A                  epsilon5:
00099A 34 12            	xorb	#0x12,al
00099C 66 35 1234       	xorw	#0x1234,ax
0009A0 35 0012 3456     	xorl	#0x123456,eax
0009A5 80 75 08 12      	xorb	#0x12,8(ebp)
0009A9 66 81 75 08 1234 	xorw	#0x1234,8(ebp)
0009AF 81 75 08 0012 3456	xorl	#0x123456,8(ebp)
0009B6 66 83 75 08 12   	xorw	#0x12,8(ebp)
0009BB 83 75 08 12      	xorl	#0x12,8(ebp)
0009BF 30 65 08         	xorb	ah,8(ebp)
0009C2 66 31 45 08      	xorw	ax,8(ebp)
0009C6 31 45 08         	xorl	eax,8(ebp)
0009C9 32 65 08         	xorb	8(ebp),ah
0009CC 66 33 45 08      	xorw	8(ebp),ax
0009D0 33 45 08         	xorl	8(ebp),eax
                        
0009D3 80 75 08 12      	eorb	#0x12,8(ebp)
0009D7 66 81 75 08 1234 	eorw	#0x1234,8(ebp)
0009DD 81 75 08 0012 3456	eorl	#0x123456,8(ebp)
                        
                        |*
                        |*
                        |*
                        	.global	gamma8
0009E4                  gamma8:
0009E4 83 EC 0C         	subl	#12,sp
0009E7 83 6D 08 0C      	subl	#12,8(fp)
0009EB 83 E8 0C         	subl	#12,ra
                        
0009EE 81 EC 0000 04B0  	subl	#1200,sp
0009F4 81 6D 08 0000 04B0	subl	#1200,8(fp)
0009FB 2D 0000 04B0     	subl	#1200,ra
                        
000A00 83 EC 08         	subl	#8,esp
000A03 83 EC 0C         	subl	#12,%esp
000A06 83 EC 0C         	subl	#12,esp
000A09 83 EC 0C         	subl	#12,sp
000A0C 83 6D 08 08      	subl	#8,8(fp)
000A10 83 6D 08 08      	subl	#8,8(%fp)
000A14 83 6C05 08 08    	subl	#8,8(fp,eax)
000A19 83 6C05 08 08    	subl	#8,8(fp,%eax)
000A1E 83 6C85 08 08    	subl	#8,8(fp,%eax@l)
000A23 83 6CC5 08 08    	subl	#8,8(fp,%eax@q)
000A28 83 EC 0C         	subl	$12,%esp
                        
                        |*
                        |*	Instrução PUSH
                        |*
                        	.global	gamma3
000A2B                  gamma3:
000A2B 66 FF 75 08      	pushw	8(fp)
000A2F FF 75 08         	pushl	8(fp)
000A32 66 50            	pushw	ra
000A34 50               	pushl	ra
000A35 55               	pushl	fp
000A36 55               	pushl	%ebp
000A37 66 6A 0C         	pushw	#12
000A3A 6A 0C            	pushl	#12
000A3C 66 68 1234       	pushw	#0x1234
000A40 68 0000 1234     	pushl	#0x1234
                        
000A45 06               	pushl	es
000A46 0E               	pushl	cs
000A47 16               	pushl	ss
000A48 1E               	pushl	ds
000A49 0FA0             	pushl	fs
000A4B 0FA8             	pushl	gs
                        
                        |*
                        |*	Instrução CLR
                        |*
000A4D 30 C0            	clrb	r0
000A4F 30 C9            	clrb	r1
000A51 30 D2            	clrb	r2
000A53 30 DB            	clrb	r3
000A55 30 E4            	clrb	h0
000A57 30 ED            	clrb	h1
000A59 30 F6            	clrb	h2
000A5B 30 FF            	clrb	h3
                        
000A5D 66 31 D2         	clrw	r2
                        
000A60 31 C0            	clrl	r0
000A62 31 C9            	clrl	r1
000A64 31 D2            	clrl	r2
000A66 31 DB            	clrl	r3
000A68 31 F6            	clrl	r4
000A6A 31 FF            	clrl	r5
000A6C 31 ED            	clrl	fp
000A6E 31 E4            	clrl	sp
                        
000A70 C6 45 08 00      	clrb	8(fp)
000A74 C6 45 08 00      	movb	#0,8(fp)
000A78 66 C7 45 08 0000 	clrw	8(fp)
000A7E 66 C7 45 08 0000 	movw	#0,8(fp)
000A84 C7 45 08 0000 0000	clrl	8(fp)
000A8B C7 45 08 0000 0000	movl	#0,8(fp)
000A92 C7 05 0000 0B84  	clrl	final
       0000 0000
000A9C C7 05 0000 0B84  	movl	#0,final
       0000 0000
                        
000AA6 31 C0            	clrl	r0
000AA8 89 45 08         	movl	r0,8(fp)
                        
                        |*
                        |*	SEG
                        |*
000AAB 26               	seg	es
000AAC 2E               	seg	cs
000AAD 36               	seg	ss
000AAE 3E               	seg	ds
000AAF 64               	seg	fs
000AB0 65               	seg	gs
                        
                        |*
                        |*	Teste de ESPRESSÕES
                        |*
000AB1 B8 0000 00AB     	movl	#0xA0|0x0B,r0
000AB6 B8 0000 00A4     	movl	#0xAF^0x0B,r0
000ABB B8 0000 000B     	movl	#0xAF&0x0B,r0
000AC0 B8 0000 00F0     	movl	#0x0F00>>4,r0
000AC5 B8 0000 F000     	movl	#0x0F00<<4,r0
000ACA B8 0000 0010     	movl	#12+4,r0
000ACF B8 0000 0B88     	movl	#final+4,r0
000AD4 B8 0000 0B88     	movl	#4+final,r0
000AD9 B8 0000 0B80     	movl	#final-4,r0
000ADE B8 0000 003C     	movl	#12*5,r0
000AE3 B8 0000 0002     	movl	#13/5,r0
000AE8 B8 0000 0003     	movl	#13%5,r0
                        
000AED B8 FFFF FFF3     	movl	#-13,r0
000AF2 B8 0000 000D     	movl	#+13,r0
000AF7 B8 FFFF FFF2     	movl	#~13,r0
                        
000AFC B8 FFFF FFF3     	movl	#{-13},r0
000B01 B8 0000 000D     	movl	#{+13},r0
000B06 B8 FFFF FFF2     	movl	#{~13},r0
                        
000B0B B8 0000 0056     	movl	#{100 + ~13},r0
                        
                        |*
                        |*	Alguns testes do NEEDED
                        |*
000B10 D1 E0            	lsll	#1,r0
000B12 C1 E0 03         	lsll	#3,r0
000B15 C1 E0 01         	lsll	#um,r0
000B18 C1 E0 03         	lsll	#tres,r0
                        
       0000 0001        um	= 1
       0000 0003        tres	= 3
                        
000B1B D1 E0            	lsll	#um,r0
000B1D C1 E0 03         	lsll	#tres,r0
000B20 C1 E0 00         	lsll	#externo,r0
                        
                        
       0000 0010        gamma	= 16
                        
000B23 6A 10            	pushl	#gamma
000B25 66 6A 10         	pushw	#gamma
                        
000B28 68 0000 000A     	pushl	#alphabeto
                        
                        	.seg16
000B2D 68 000A          	pushw	#alphabeto
                        
                        	.seg32
000B30 68 0000 0B8C     	pushl	#beta
000B35 6A 0C            	pushl	#12
000B37 68 0000 04B0     	pushl	#1200
000B3C                  tst_label:
000B3C 00               	.byte	0
                        
000B3D 55 89E5 81EC 0000	link	#alphabeto
       000A
                        
000B46 8B 85 0000 000A  	movl	alphabeto(fp),r0
                        
       0000 000A        alphabeto	= 10
                        
000B4C 8B 45 0A         	movl	alphabeto(fp),r0
                        
000B4F 55 89E5 83EC 0A  	link	#alphabeto
                        
000B55 55 89E5 83EC 7E  	link	#126
000B5B 55 89E5 83EC 7F  	link	#127
000B61 55 89E5 81EC 0000	link	#128
       0080
000B6A 55 89E5 81EC 0000	link	#129
       0081
                        
000B73 05 0000 000A     	addl	#argv-.end,r0
000B78                  .end:
000B78 B8 0000 000A     	movl	#argv-.end,r0
000B7D 68 0000 000A     	pushl	#argv-.end
000B82                  argv:
000B82 6A 0A            	pushl	#argv-.end
                        
                        |*
                        |*	Final
                        |*
000B84                  final:
000B84 83 28 01         	subl	#1,(eax)



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

ABS   alpha                                            0000 1234
DATA  beta                                     R   E   0000 0B8C
TEXT  (003),$1                           L     R       0000 02F4
TEXT  (003),$12                          L     R       0000 042D
TEXT  (003),$2                           L     R       0000 044B
TEXT  (003),$3                           L     R       0000 044F
TEXT  (003),$4                           L     R       0000 04A6
TEXT  (003),$5                           L     R       0000 04BA
TEXT  (003),$6                           L     R       0000 04C4
TEXT  (003),$7                           L     R       0000 0526
TEXT  (003),$8                           L     R       0000 0540
TEXT  (003),$9                           L     R       0000 0550
TEXT  (003),$10                          L     R       0000 0561
UND   undef                                        E   0000 0000
TEXT  (003),$11                          L     R       0000 0587
TEXT  gamma0                                   R   E   0000 05A1
TEXT  gamma1                                   R   E   0000 05C0
TEXT  gamma2                                   R   E   0000 05DD
TEXT  epsilon0                                 R   E   0000 060D
TEXT  gamma4                                   R   E   0000 0646
TEXT  epsilon1                                 R   E   0000 0699
TEXT  epsilon2                                 R   E   0000 06D2
TEXT  epsilon3                                 R   E   0000 070B
TEXT  epsilon4                                 R   E   0000 0744
TEXT  gamma5                                   R   E   0000 077D
UND   muito_longe                                  E   0000 0000
TEXT  gamma6                                   R   E   0000 083C
UND   mais_longe_ainda                             E   0000 0000
TEXT  gamma7                                   R   E   0000 0904
UND   somente_longe                                E   0000 0000
TEXT  epsilon5                                 R   E   0000 099A
TEXT  gamma8                                   R   E   0000 09E4
TEXT  gamma3                                   R   E   0000 0A2B
TEXT  final                                    R       0000 0B84
ABS   um                                               0000 0001
ABS   tres                                             0000 0003
UND   externo                                      E   0000 0000
ABS   gamma                                            0000 0010
ABS   alphabeto                                        0000 000A
TEXT  tst_label                                R       0000 0B3C
TEXT  argv                                     R       0000 0B82
TEXT  .end                                     R       0000 0B78
