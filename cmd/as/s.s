|*
|*	Teste dos códigos de desvio
|*
delta1:
	bvs	delta1
	bvc	delta1
	blo	delta1
	bcs	delta1
	bhs	delta1
	bcc	delta1
	beq	delta1
	bne	delta1
	bls	delta1
	bhi	delta1
	bmi	delta1
	bpl	delta2
	bp	delta2
	bpe	delta2
	bpo	delta2
	blt	delta2
	bge	delta2
	ble	delta2
	bgt	delta2
	br1z	delta2
	bcxz	delta2
	becxz	delta2
delta2:
	bvsl	delta3
	bvcl	delta3
	blol	delta3
	bcsl	delta3
	bhsl	delta3
	bccl	delta3
	beql	delta3
	bnel	delta3
	blsl	delta4
	bhil	delta4
	bmil	delta4
	bpll	delta4
	bpel	delta4
	bpol	delta4
	bltl	delta4
	bgel	delta4
	blel	delta4
	bgtl	delta4
delta3:
	jvs	delta3
	jvc	delta3
	jlo	delta3
	jcs	delta3
	jhs	delta3
	jcc	delta3
	jeq	delta3
	jne	delta3
	jls	delta4
	jhi	delta4
	jmi	delta4
	jpl	delta4
	jpe	delta4
	jpo	delta4
	jlt	delta4
	jge	delta4
	jle	delta4
	jgt	delta4
1$:
	jmp	.

|*
|*	Agora, que todos os códigos de condição foram
|*	testados, podemos nos concentrar nos deslocamentos
|*
delta4:
	beq	delta5	|* Grupo 50
	beql	delta5	|* Grupo 51
	jeq	delta5	|* Grupo 55

	bra	delta5	|* Grupo 50
	bral	delta5	|* Grupo 53
	jmp	delta5	|* Grupo 56

	call	delta5	|* Grupo 53
1$:
	jmp	.
delta5:
	beq	delta4	|* Grupo 50
	beql	delta4	|* Grupo 52
	jeq	delta4	|* Grupo 55

	bra	delta4	|* Grupo 50
	bral	delta4	|* Grupo 53
	jmp	delta4	|* Grupo 56

	call	delta4	|* Grupo 53
8$:
	jmp	.

|*
|*	Teste com deslocamento NULO
|*
	beq	1$	|* Grupo 50
1$:
	beql	2$	|* Grupo 52
2$:
	bra	3$	|* Grupo 50
3$:
	bral	4$	|* Grupo 53
4$:
	jeq	5$	|* Grupo 55
5$:
	jra	6$	|* Grupo 56
6$:
	call	7$	|* Grupo 53
7$:
	jmp	.

|*
|*	Outras formas
|*
delta6:
	bral	8(fp)
	bral	8(ebx,edi@w)
	bral	(ra)
	bral	ra
	bral	_printf
	bral	_memcpy+8

	jra	8(fp)
	jra	8(ebx,edi@w)
	jra	(ra)
	jra	ra
	jra 	_printf
1$:
	jmp	.

|*
|*	Chamadas a funções
|*
delta7:
	call	8(fp)
	call	8(ebx,edi@w)
	call	(ra)
	call	ra

	call	_printf
	call	_memcpy+8
delta30:
	call	delta
1$:
	jmp	.

|*
|*	SDIs para a frente
|*
frente:
1$:
	jgt	2$
	.dsb	126
2$:
	jgt	3$
	.dsb	127
3$:
	jgt	4$
	.dsb	128
4$:
	jgt	5$
	.dsb	129
5$:
	jmp	.

|*
|*	SDIs para trás (os rótulos extras são para verificar a fase)
|*
trás:
2$:
	.dsb	125
22$:
	jgt	2$
3$:
	.dsb	126
23$:
	jgt	3$
4$:
	.dsb	127
24$:
	jgt	4$
5$:
	.dsb	128
25$:
	jgt	5$
1$:
	jmp	.

|*
|*	Desvios incondicionais
|*
incond:
1$:
	jra	2$
	.dsb	20
2$:
	jra	3$
	.dsb	200
3$:
	jra	.

|*
|*	Interdependência de SDIs
|*
dep:
1$:
	.dsb	124
	jgt	2$
	jgt	1$
	.dsb	125
2$:
	.dsb	125
	jgt	3$
	jgt	2$
	.dsb	125
3$:
	.dsb	124
	jgt	4$
	jgt	3$
	.dsb	126
4$:
	bra	.

|*
|*	Externo
|*
	.global	_printf
externo:
1$:
	jgt	_printf
2$:
	jgt	. + 2
3$:
	jmp	.

|*
|*	Com o LC
|*
lc:
1$:
	jgt	.

	jgt	. + 127
	jgt	. + 128
	jgt	. + 129
	jgt	. + 130
	jgt	. + 131
	jgt	. + 132
2$:
	jgt	. - 124
	jgt	. - 125
	jgt	. - 126
	jgt	. - 127
	jgt	. - 128
	jgt	. - 129
3$:
	jgt	.

|*
|*	JMPI
|*
	.dsb	16
go:
	.dsb	16
1$:
	jmpiw	#0x1234,final
2$:
	.dsb	16
3$:
	farjmpl	#0x1234,go
4$:
	.dsb	16
5$:
	farjmpw	#0x1234,8(fp)
	jmpil	#0x1234,8(fp)
6$:
	jmp	.

|*
|*	CALL GATE
|*
	.dsb	16
callg32:
	.dsb	16
1$:
	callgw	#0x1234,final
2$:
	.dsb	16
3$:
	callgl	#0x1234,go
4$:
	.dsb	16
5$:
	callgw	#0x1234,8(fp)
	callgl	#0x1234,8(fp)

|*
|*	Agora, tudo de novo para segmentos de 16 bits
|*
	.seg16
gamma4:
	beq	gamma5	|* Grupo 50
	beql	gamma5	|* Grupo 51
	jeq	gamma5	|* Grupo 55

	bra	gamma5	|* Grupo 50
	bral	gamma5	|* Grupo 53
	jmp	gamma5	|* Grupo 56

	call	gamma5	|* Grupo 53
1$:
	jmp	.
gamma5:
	beq	gamma4	|* Grupo 50
	beql	gamma4	|* Grupo 52
	jeq	gamma4	|* Grupo 55

	bra	gamma4	|* Grupo 50
	bral	gamma4	|* Grupo 53
	jmp	gamma4	|* Grupo 56

	call	gamma4	|* Grupo 53
8$:
	jmp	.

|*
|*	Teste com deslocamento NULO
|*
	beq	1$	|* Grupo 50
1$:
	beql	2$	|* Grupo 52
2$:
	bra	3$	|* Grupo 50
3$:
	bral	4$	|* Grupo 53
4$:
	jeq	5$	|* Grupo 55
5$:
	jra	6$	|* Grupo 56
6$:
	call	7$	|* Grupo 53
7$:
	jmp	.

|*
|*	Outras formas
|*
gamma6:
	bral	8(fp)
	bral	8(ebx,edi)
	bral	(r4)
	bral	r4
	bral	_printf
	bral	_memcpy+8

	jra	8(fp)
	jra	8(ebx,edi)
	jra	(r4)
	jra	r4
	jra 	_printf
1$:
	jmp	.

|*
|*	Chamadas a funções
|*
gamma7:
	call	8(fp)
	call	8(ebx,edi)
	call	(r4)
	call	r4

	call	_printf
	call	_memcpy+8
gamma30:
	call	gamma
1$:
	jmp	.

|*
|*	SDIs para a frente
|*
forward:
1$:
	jgt	2$
	.dsb	126
2$:
	jgt	3$
	.dsb	127
3$:
	jgt	4$
	.dsb	128
4$:
	jgt	5$
	.dsb	129
5$:
	jgt	6$
	.dsb	32764
6$:
	jgt	7$
	.dsb	32765
7$:
	jmp	.

|*
|*	SDIs para trás (os rótulos extras são para verificar a fase)
|*
back:
1$:
	.dsb	124
21$:
	jgt	1$
2$:
	.dsb	125
22$:
	jgt	2$
3$:
	.dsb	126
23$:
	jgt	3$
4$:
	.dsb	127
24$:
	jgt	4$
5$:
	.dsb	128
25$:
	jgt	5$
6$:
	.dsb	32761
26$:
	jgt	6$
7$:
	jmp	.

|*
|*	Desvios incondicionais
|*
incond16:
1$:
	jra	2$
	.dsb	20
2$:
	jra	3$
	.dsb	200
3$:
	jra	.

|*
|*	Interdependência de SDIs
|*
dep16:
1$:
	.dsb	124
	jgt	2$
	jgt	1$
	.dsb	125
2$:
	.dsb	125
	jgt	3$
	jgt	2$
	.dsb	125
3$:
	.dsb	124
	jgt	4$
	jgt	3$
	.dsb	126
4$:
	bra	.

|*
|*	Externo
|*
	.global	_printf
externo16:
1$:
	jgt	_printf
2$:
	jgt	. + 2

|*
|*	Com o LC
|*
lc16:
1$:
	jgt	.

	jgt	. + 127
	jgt	. + 128
	jgt	. + 129
	jgt	. + 130
	jgt	. + 131
	jgt	. + 132
2$:
	jgt	. - 124
	jgt	. - 125
	jgt	. - 126
	jgt	. - 127
	jgt	. - 128
	jgt	. - 129
3$:
	jgt	. - 32761
	jgt	. - 32762
	jgt	. - 32763
	jgt	. - 32764
5$:
	jgt	.

|*
|*	JMPI
|*
	.dsb	16
go16:
	.dsb	16
1$:
	jmpiw	#0x1234,final
2$:
	.dsb	16
3$:
	jmpil	#0x1234,go
4$:
	.dsb	16
5$:
	farjmpw	#0x1234,8(fp)
	farjmpl	#0x1234,8(fp)
final:
	jmp	.

|*
|*	CALL GATE
|*
	.dsb	16
callg16:
	.dsb	16
1$:
	callgw	#0x1234,final
2$:
	.dsb	16
3$:
	callgl	#0x1234,go
4$:
	.dsb	16
5$:
	callgw	#0x1234,8(fp)
	callgl	#0x1234,8(fp)
novofinal:
	jmp	.

|*
|*	Teste do modo de endereçamento "*alpha"
|*
	.seg32
	.text
tau1:
	call	*tau9	
	jmp	*tau9	
tau2:
	jmp	.

	.data
tau9:
	.long	tau1

	.seg16
	.text
tau11:
	call	*tau9	
	jmp	*tau9	
tau12:
	jmp	.

	.data
tau19:
	.long	tau1
