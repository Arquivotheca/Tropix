                        |*
                        |*	Teste dos códigos de desvio
                        |*
000000                  delta1:
000000 70FE             	bvs	delta1
000002 71FC             	bvc	delta1
000004 72FA             	blo	delta1
000006 72F8             	bcs	delta1
000008 73F6             	bhs	delta1
00000A 73F4             	bcc	delta1
00000C 74F2             	beq	delta1
00000E 75F0             	bne	delta1
000010 76EE             	bls	delta1
000012 77EC             	bhi	delta1
000014 78EA             	bmi	delta1
000016 7914             	bpl	delta2
000018 7A12             	bp	delta2
00001A 7A10             	bpe	delta2
00001C 7B0E             	bpo	delta2
00001E 7C0C             	blt	delta2
000020 7D0A             	bge	delta2
000022 7E08             	ble	delta2
000024 7F06             	bgt	delta2
000026 E304             	br1z	delta2
000028 E302             	bcxz	delta2
00002A E300             	becxz	delta2
00002C                  delta2:
00002C 0F80 0000 0066   	bvsl	delta3
000032 0F81 0000 0060   	bvcl	delta3
000038 0F82 0000 005A   	blol	delta3
00003E 0F82 0000 0054   	bcsl	delta3
000044 0F83 0000 004E   	bhsl	delta3
00004A 0F83 0000 0048   	bccl	delta3
000050 0F84 0000 0042   	beql	delta3
000056 0F85 0000 003C   	bnel	delta3
00005C 0F86 0000 005C   	blsl	delta4
000062 0F87 0000 0056   	bhil	delta4
000068 0F88 0000 0050   	bmil	delta4
00006E 0F89 0000 004A   	bpll	delta4
000074 0F8A 0000 0044   	bpel	delta4
00007A 0F8B 0000 003E   	bpol	delta4
000080 0F8C 0000 0038   	bltl	delta4
000086 0F8D 0000 0032   	bgel	delta4
00008C 0F8E 0000 002C   	blel	delta4
000092 0F8F 0000 0026   	bgtl	delta4
000098                  delta3:
000098 70FE             	jvs	delta3
00009A 71FC             	jvc	delta3
00009C 72FA             	jlo	delta3
00009E 72F8             	jcs	delta3
0000A0 73F6             	jhs	delta3
0000A2 73F4             	jcc	delta3
0000A4 74F2             	jeq	delta3
0000A6 75F0             	jne	delta3
0000A8 7614             	jls	delta4
0000AA 7712             	jhi	delta4
0000AC 7810             	jmi	delta4
0000AE 790E             	jpl	delta4
0000B0 7A0C             	jpe	delta4
0000B2 7B0A             	jpo	delta4
0000B4 7C08             	jlt	delta4
0000B6 7D06             	jge	delta4
0000B8 7E04             	jle	delta4
0000BA 7F02             	jgt	delta4
0000BC                  1$:
0000BC EBFE             	jmp	.
                        
                        |*
                        |*	Agora, que todos os códigos de condição foram
                        |*	testados, podemos nos concentrar nos deslocamentos
                        |*
0000BE                  delta4:
0000BE 7418             	beq	delta5	|* Grupo 50
0000C0 0F84 0000 0012   	beql	delta5	|* Grupo 51
0000C6 7410             	jeq	delta5	|* Grupo 55
                        
0000C8 EB0E             	bra	delta5	|* Grupo 50
0000CA E9 0000 0009     	bral	delta5	|* Grupo 53
0000CF EB07             	jmp	delta5	|* Grupo 56
                        
0000D1 E8 0000 0002     	call	delta5	|* Grupo 53
0000D6                  1$:
0000D6 EBFE             	jmp	.
0000D8                  delta5:
0000D8 74E4             	beq	delta4	|* Grupo 50
0000DA 0F84 FFFF FFDE   	beql	delta4	|* Grupo 52
0000E0 74DC             	jeq	delta4	|* Grupo 55
                        
0000E2 EBDA             	bra	delta4	|* Grupo 50
0000E4 E9 FFFF FFD5     	bral	delta4	|* Grupo 53
0000E9 EBD3             	jmp	delta4	|* Grupo 56
                        
0000EB E8 FFFF FFCE     	call	delta4	|* Grupo 53
0000F0                  8$:
0000F0 EBFE             	jmp	.
                        
                        |*
                        |*	Teste com deslocamento NULO
                        |*
0000F2 7400             	beq	1$	|* Grupo 50
0000F4                  1$:
0000F4 0F84 0000 0000   	beql	2$	|* Grupo 52
0000FA                  2$:
0000FA EB00             	bra	3$	|* Grupo 50
0000FC                  3$:
0000FC E9 0000 0000     	bral	4$	|* Grupo 53
000101                  4$:
000101 7400             	jeq	5$	|* Grupo 55
000103                  5$:
000103 EB00             	jra	6$	|* Grupo 56
000105                  6$:
000105 E8 0000 0000     	call	7$	|* Grupo 53
00010A                  7$:
00010A EBFE             	jmp	.
                        
                        |*
                        |*	Outras formas
                        |*
00010C                  delta6:
00010C FF 65 08         	bral	8(fp)
00010F FF 647B 08       	bral	8(ebx,edi@w)
000113 FF 20            	bral	(ra)
000115 FF E0            	bral	ra
000117 E9 FFFF FEE4     	bral	_printf
00011C E9 FFFF FEE7     	bral	_memcpy+8
                        
000121 FF 65 08         	jra	8(fp)
000124 FF 647B 08       	jra	8(ebx,edi@w)
000128 FF 20            	jra	(ra)
00012A FF E0            	jra	ra
00012C E9 FFFF FECF     	jra 	_printf
000131                  1$:
000131 EBFE             	jmp	.
                        
                        |*
                        |*	Chamadas a funções
                        |*
000133                  delta7:
000133 FF 55 08         	call	8(fp)
000136 FF 547B 08       	call	8(ebx,edi@w)
00013A FF 10            	call	(ra)
00013C FF D0            	call	ra
                        
00013E E8 FFFF FEBD     	call	_printf
000143 E8 FFFF FEC0     	call	_memcpy+8
000148                  delta30:
000148 E8 FFFF FEB3     	call	delta
00014D                  1$:
00014D EBFE             	jmp	.
                        
                        |*
                        |*	SDIs para a frente
                        |*
00014F                  frente:
00014F                  1$:
00014F 7F7E             	jgt	2$
000151                  	.dsb	126
0001CF                  2$:
0001CF 7F7F             	jgt	3$
0001D1                  	.dsb	127
000250                  3$:
000250 0F8F 0000 0080   	jgt	4$
000256                  	.dsb	128
0002D6                  4$:
0002D6 0F8F 0000 0081   	jgt	5$
0002DC                  	.dsb	129
00035D                  5$:
00035D EBFE             	jmp	.
                        
                        |*
                        |*	SDIs para trás (os rótulos extras são para verificar a fase)
                        |*
00035F                  trás:
00035F                  2$:
00035F                  	.dsb	125
0003DC                  22$:
0003DC 7F81             	jgt	2$
0003DE                  3$:
0003DE                  	.dsb	126
00045C                  23$:
00045C 7F80             	jgt	3$
00045E                  4$:
00045E                  	.dsb	127
0004DD                  24$:
0004DD 0F8F FFFF FF7B   	jgt	4$
0004E3                  5$:
0004E3                  	.dsb	128
000563                  25$:
000563 0F8F FFFF FF7A   	jgt	5$
000569                  1$:
000569 EBFE             	jmp	.
                        
                        |*
                        |*	Desvios incondicionais
                        |*
00056B                  incond:
00056B                  1$:
00056B EB14             	jra	2$
00056D                  	.dsb	20
000581                  2$:
000581 E9 0000 00C8     	jra	3$
000586                  	.dsb	200
00064E                  3$:
00064E EBFE             	jra	.
                        
                        |*
                        |*	Interdependência de SDIs
                        |*
000650                  dep:
000650                  1$:
000650                  	.dsb	124
0006CC 7F7F             	jgt	2$
0006CE 7F80             	jgt	1$
0006D0                  	.dsb	125
00074D                  2$:
00074D                  	.dsb	125
0007CA 0F8F 0000 0083   	jgt	3$
0007D0 0F8F FFFF FF77   	jgt	2$
0007D6                  	.dsb	125
000853                  3$:
000853                  	.dsb	124
0008CF 0F8F 0000 0084   	jgt	4$
0008D5 0F8F FFFF FF78   	jgt	3$
0008DB                  	.dsb	126
000959                  4$:
000959 EBFE             	bra	.
                        
                        |*
                        |*	Externo
                        |*
                        	.global	_printf
00095B                  externo:
00095B                  1$:
00095B 0F8F FFFF F69F   	jgt	_printf
000961                  2$:
000961 7F00             	jgt	. + 2
000963                  3$:
000963 EBFE             	jmp	.
                        
                        |*
                        |*	Com o LC
                        |*
000965                  lc:
000965                  1$:
000965 7FFE             	jgt	.
                        
000967 7F7D             	jgt	. + 127
000969 7F7E             	jgt	. + 128
00096B 7F7F             	jgt	. + 129
00096D 0F8F 0000 007C   	jgt	. + 130
000973 0F8F 0000 007D   	jgt	. + 131
000979 0F8F 0000 007E   	jgt	. + 132
00097F                  2$:
00097F 7F82             	jgt	. - 124
000981 7F81             	jgt	. - 125
000983 7F80             	jgt	. - 126
000985 0F8F FFFF FF7B   	jgt	. - 127
00098B 0F8F FFFF FF7A   	jgt	. - 128
000991 0F8F FFFF FF79   	jgt	. - 129
000997                  3$:
000997 7FFE             	jgt	.
                        
                        |*
                        |*	JMPI
                        |*
000999                  	.dsb	16
0009A9                  go:
0009A9                  	.dsb	16
0009B9                  1$:
0009B9 66 EA 93C7 1234  	jmpiw	#0x1234,final
0009BF                  2$:
0009BF                  	.dsb	16
0009CF                  3$:
0009CF EA 0000 09A9 1234	farjmpl	#0x1234,go
0009D6                  4$:
0009D6                  	.dsb	16
0009E6                  5$:
0009E6 66 FF 6D 08 1234 	farjmpw	#0x1234,8(fp)
0009EC FF 6D 08 1234    	jmpil	#0x1234,8(fp)
0009F1                  6$:
0009F1 EBFE             	jmp	.
                        
                        |*
                        |*	CALL GATE
                        |*
0009F3                  	.dsb	16
000A03                  callg32:
000A03                  	.dsb	16
000A13                  1$:
000A13 66 9A 93C7 1234  	callgw	#0x1234,final
000A19                  2$:
000A19                  	.dsb	16
000A29                  3$:
000A29 9A 0000 09A9 1234	callgl	#0x1234,go
000A30                  4$:
000A30                  	.dsb	16
000A40                  5$:
000A40 66 FF 5D 08 1234 	callgw	#0x1234,8(fp)
000A46 FF 5D 08 1234    	callgl	#0x1234,8(fp)
                        
                        |*
                        |*	Agora, tudo de novo para segmentos de 16 bits
                        |*
                        	.seg16
000A4B                  gamma4:
000A4B 7412             	beq	gamma5	|* Grupo 50
000A4D 0F84 000E        	beql	gamma5	|* Grupo 51
000A51 740C             	jeq	gamma5	|* Grupo 55
                        
000A53 EB0A             	bra	gamma5	|* Grupo 50
000A55 E9 0007          	bral	gamma5	|* Grupo 53
000A58 EB05             	jmp	gamma5	|* Grupo 56
                        
000A5A E8 0002          	call	gamma5	|* Grupo 53
000A5D                  1$:
000A5D EBFE             	jmp	.
000A5F                  gamma5:
000A5F 74EA             	beq	gamma4	|* Grupo 50
000A61 0F84 FFE6        	beql	gamma4	|* Grupo 52
000A65 74E4             	jeq	gamma4	|* Grupo 55
                        
000A67 EBE2             	bra	gamma4	|* Grupo 50
000A69 E9 FFDF          	bral	gamma4	|* Grupo 53
000A6C EBDD             	jmp	gamma4	|* Grupo 56
                        
000A6E E8 FFDA          	call	gamma4	|* Grupo 53
000A71                  8$:
000A71 EBFE             	jmp	.
                        
                        |*
                        |*	Teste com deslocamento NULO
                        |*
000A73 7400             	beq	1$	|* Grupo 50
000A75                  1$:
000A75 0F84 0000        	beql	2$	|* Grupo 52
000A79                  2$:
000A79 EB00             	bra	3$	|* Grupo 50
000A7B                  3$:
000A7B E9 0000          	bral	4$	|* Grupo 53
000A7E                  4$:
000A7E 7400             	jeq	5$	|* Grupo 55
000A80                  5$:
000A80 EB00             	jra	6$	|* Grupo 56
000A82                  6$:
000A82 E8 0000          	call	7$	|* Grupo 53
000A85                  7$:
000A85 EBFE             	jmp	.
                        
                        |*
                        |*	Outras formas
                        |*
000A87                  gamma6:
000A87 FF 66 08         	bral	8(fp)
000A8A FF 61 08         	bral	8(ebx,edi)
000A8D FF 24            	bral	(r4)
000A8F FF E6            	bral	r4
000A91 E9 F56C          	bral	_printf
000A94 E9 F571          	bral	_memcpy+8
                        
000A97 FF 66 08         	jra	8(fp)
000A9A FF 61 08         	jra	8(ebx,edi)
000A9D FF 24            	jra	(r4)
000A9F FF E6            	jra	r4
000AA1 E9 F55C          	jra 	_printf
000AA4                  1$:
000AA4 EBFE             	jmp	.
                        
                        |*
                        |*	Chamadas a funções
                        |*
000AA6                  gamma7:
000AA6 FF 56 08         	call	8(fp)
000AA9 FF 51 08         	call	8(ebx,edi)
000AAC FF 14            	call	(r4)
000AAE FF D6            	call	r4
                        
000AB0 E8 F54D          	call	_printf
000AB3 E8 F552          	call	_memcpy+8
000AB6                  gamma30:
000AB6 E8 F547          	call	gamma
000AB9                  1$:
000AB9 EBFE             	jmp	.
                        
                        |*
                        |*	SDIs para a frente
                        |*
000ABB                  forward:
000ABB                  1$:
000ABB 7F7E             	jgt	2$
000ABD                  	.dsb	126
000B3B                  2$:
000B3B 7F7F             	jgt	3$
000B3D                  	.dsb	127
000BBC                  3$:
000BBC 0F8F 0080        	jgt	4$
000BC0                  	.dsb	128
000C40                  4$:
000C40 0F8F 0081        	jgt	5$
000C44                  	.dsb	129
000CC5                  5$:
000CC5 0F8F 7FFC        	jgt	6$
000CC9                  	.dsb	32764
008CC5                  6$:
008CC5 0F8F 7FFD        	jgt	7$
008CC9                  	.dsb	32765
010CC6                  7$:
010CC6 EBFE             	jmp	.
                        
                        |*
                        |*	SDIs para trás (os rótulos extras são para verificar a fase)
                        |*
010CC8                  back:
010CC8                  1$:
010CC8                  	.dsb	124
010D44                  21$:
010D44 7F82             	jgt	1$
010D46                  2$:
010D46                  	.dsb	125
010DC3                  22$:
010DC3 7F81             	jgt	2$
010DC5                  3$:
010DC5                  	.dsb	126
010E43                  23$:
010E43 7F80             	jgt	3$
010E45                  4$:
010E45                  	.dsb	127
010EC4                  24$:
010EC4 0F8F FF7D        	jgt	4$
010EC8                  5$:
010EC8                  	.dsb	128
010F48                  25$:
010F48 0F8F FF7C        	jgt	5$
010F4C                  6$:
010F4C                  	.dsb	32761
018F45                  26$:
018F45 0F8F 8003        	jgt	6$
018F49                  7$:
018F49 EBFE             	jmp	.
                        
                        |*
                        |*	Desvios incondicionais
                        |*
018F4B                  incond16:
018F4B                  1$:
018F4B EB14             	jra	2$
018F4D                  	.dsb	20
018F61                  2$:
018F61 E9 00C8          	jra	3$
018F64                  	.dsb	200
01902C                  3$:
01902C EBFE             	jra	.
                        
                        |*
                        |*	Interdependência de SDIs
                        |*
01902E                  dep16:
01902E                  1$:
01902E                  	.dsb	124
0190AA 7F7F             	jgt	2$
0190AC 7F80             	jgt	1$
0190AE                  	.dsb	125
01912B                  2$:
01912B                  	.dsb	125
0191A8 0F8F 0081        	jgt	3$
0191AC 0F8F FF7B        	jgt	2$
0191B0                  	.dsb	125
01922D                  3$:
01922D                  	.dsb	124
0192A9 0F8F 0082        	jgt	4$
0192AD 0F8F FF7C        	jgt	3$
0192B1                  	.dsb	126
01932F                  4$:
01932F EBFE             	bra	.
                        
                        |*
                        |*	Externo
                        |*
                        	.global	_printf
019331                  externo16:
019331                  1$:
019331 0F8F 6CCB        	jgt	_printf
019335                  2$:
019335 7F00             	jgt	. + 2
                        
                        |*
                        |*	Com o LC
                        |*
019337                  lc16:
019337                  1$:
019337 7FFE             	jgt	.
                        
019339 7F7D             	jgt	. + 127
01933B 7F7E             	jgt	. + 128
01933D 7F7F             	jgt	. + 129
01933F 0F8F 007E        	jgt	. + 130
019343 0F8F 007F        	jgt	. + 131
019347 0F8F 0080        	jgt	. + 132
01934B                  2$:
01934B 7F82             	jgt	. - 124
01934D 7F81             	jgt	. - 125
01934F 7F80             	jgt	. - 126
019351 0F8F FF7D        	jgt	. - 127
019355 0F8F FF7C        	jgt	. - 128
019359 0F8F FF7B        	jgt	. - 129
01935D                  3$:
01935D 0F8F 8003        	jgt	. - 32761
019361 0F8F 8002        	jgt	. - 32762
019365 0F8F 8001        	jgt	. - 32763
019369 0F8F 8000        	jgt	. - 32764
01936D                  5$:
01936D 7FFE             	jgt	.
                        
                        |*
                        |*	JMPI
                        |*
01936F                  	.dsb	16
01937F                  go16:
01937F                  	.dsb	16
01938F                  1$:
01938F EA 93C7 1234     	jmpiw	#0x1234,final
019394                  2$:
019394                  	.dsb	16
0193A4                  3$:
0193A4 66 EA 0000 09A9  	jmpil	#0x1234,go
       1234
0193AC                  4$:
0193AC                  	.dsb	16
0193BC                  5$:
0193BC FF 6E 08 1234    	farjmpw	#0x1234,8(fp)
0193C1 66 FF 6E 08 1234 	farjmpl	#0x1234,8(fp)
0193C7                  final:
0193C7 EBFE             	jmp	.
                        
                        |*
                        |*	CALL GATE
                        |*
0193C9                  	.dsb	16
0193D9                  callg16:
0193D9                  	.dsb	16
0193E9                  1$:
0193E9 9A 93C7 1234     	callgw	#0x1234,final
0193EE                  2$:
0193EE                  	.dsb	16
0193FE                  3$:
0193FE 66 9A 0000 09A9  	callgl	#0x1234,go
       1234
019406                  4$:
019406                  	.dsb	16
019416                  5$:
019416 FF 5E 08 1234    	callgw	#0x1234,8(fp)
01941B 66 FF 5E 08 1234 	callgl	#0x1234,8(fp)
019421                  novofinal:
019421 EBFE             	jmp	.
                        
                        |*
                        |*	Teste do modo de endereçamento "*alpha"
                        |*
                        	.seg32
                        	.text
019423                  tau1:
019423 FF 15 0001 943C  	call	*tau9	
019429 FF 25 0001 943C  	jmp	*tau9	
01942F                  tau2:
01942F EBFE             	jmp	.
                        
                        	.data
01943C                  tau9:
01943C 0001 9423        	.long	tau1
                        
                        	.seg16
                        	.text
019431                  tau11:
019431 FF 16 943C       	call	*tau9	
019435 FF 26 943C       	jmp	*tau9	
019439                  tau12:
019439 EBFE             	jmp	.
                        
                        	.data
019440                  tau19:
019440 0001 9423        	.long	tau1



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

TEXT  delta1                                   R       0000 0000
TEXT  delta2                                   R       0000 002C
TEXT  delta3                                   R       0000 0098
TEXT  delta4                                   R       0000 00BE
TEXT  (003),$1                           L     R       0000 00BC
TEXT  delta5                                   R       0000 00D8
TEXT  (004),$1                           L     R       0000 00D6
TEXT  (005),$8                           L     R       0000 00F0
TEXT  (005),$1                           L     R       0000 00F4
TEXT  (005),$2                           L     R       0000 00FA
TEXT  (005),$3                           L     R       0000 00FC
TEXT  (005),$4                           L     R       0000 0101
TEXT  (005),$5                           L     R       0000 0103
TEXT  (005),$6                           L     R       0000 0105
TEXT  (005),$7                           L     R       0000 010A
TEXT  delta6                                   R       0000 010C
UND   _printf                                      E   0000 0000
UND   _memcpy                                      E   0000 0000
TEXT  (006),$1                           L     R       0000 0131
TEXT  delta7                                   R       0000 0133
TEXT  delta30                                  R       0000 0148
UND   delta                                        E   0000 0000
TEXT  (008),$1                           L     R       0000 014D
TEXT  frente                                   R       0000 014F
TEXT  (009),$1                           L     R       0000 014F
TEXT  (009),$2                           L     R       0000 01CF
TEXT  (009),$3                           L     R       0000 0250
TEXT  (009),$4                           L     R       0000 02D6
TEXT  (009),$5                           L     R       0000 035D
TEXT  trás                                     R       0000 035F
TEXT  (010),$2                           L     R       0000 035F
TEXT  (010),$22                          L     R       0000 03DC
TEXT  (010),$3                           L     R       0000 03DE
TEXT  (010),$23                          L     R       0000 045C
TEXT  (010),$4                           L     R       0000 045E
TEXT  (010),$24                          L     R       0000 04DD
TEXT  (010),$5                           L     R       0000 04E3
TEXT  (010),$25                          L     R       0000 0563
TEXT  (010),$1                           L     R       0000 0569
TEXT  incond                                   R       0000 056B
TEXT  (011),$1                           L     R       0000 056B
TEXT  (011),$2                           L     R       0000 0581
TEXT  (011),$3                           L     R       0000 064E
TEXT  dep                                      R       0000 0650
TEXT  (012),$1                           L     R       0000 0650
TEXT  (012),$2                           L     R       0000 074D
TEXT  (012),$3                           L     R       0000 0853
TEXT  (012),$4                           L     R       0000 0959
TEXT  externo                                  R       0000 095B
TEXT  (013),$1                           L     R       0000 095B
TEXT  (013),$2                           L     R       0000 0961
TEXT  (013),$3                           L     R       0000 0963
TEXT  lc                                       R       0000 0965
TEXT  (014),$1                           L     R       0000 0965
TEXT  (014),$2                           L     R       0000 097F
TEXT  (014),$3                           L     R       0000 0997
TEXT  go                                       R       0000 09A9
TEXT  (015),$1                           L     R       0000 09B9
TEXT  final                                    R       0001 93C7
TEXT  (015),$2                           L     R       0000 09BF
TEXT  (015),$3                           L     R       0000 09CF
TEXT  (015),$4                           L     R       0000 09D6
TEXT  (015),$5                           L     R       0000 09E6
TEXT  (015),$6                           L     R       0000 09F1
TEXT  callg32                                  R       0000 0A03
TEXT  (016),$1                           L     R       0000 0A13
TEXT  (016),$2                           L     R       0000 0A19
TEXT  (016),$3                           L     R       0000 0A29
TEXT  (016),$4                           L     R       0000 0A30
TEXT  (016),$5                           L     R       0000 0A40
TEXT  gamma4                                   R       0000 0A4B
TEXT  gamma5                                   R       0000 0A5F
TEXT  (017),$1                           L     R       0000 0A5D
TEXT  (018),$8                           L     R       0000 0A71
TEXT  (018),$1                           L     R       0000 0A75
TEXT  (018),$2                           L     R       0000 0A79
TEXT  (018),$3                           L     R       0000 0A7B
TEXT  (018),$4                           L     R       0000 0A7E
TEXT  (018),$5                           L     R       0000 0A80
TEXT  (018),$6                           L     R       0000 0A82
TEXT  (018),$7                           L     R       0000 0A85
TEXT  gamma6                                   R       0000 0A87
TEXT  (019),$1                           L     R       0000 0AA4
TEXT  gamma7                                   R       0000 0AA6
TEXT  gamma30                                  R       0000 0AB6
UND   gamma                                        E   0000 0000
TEXT  (021),$1                           L     R       0000 0AB9
TEXT  forward                                  R       0000 0ABB
TEXT  (022),$1                           L     R       0000 0ABB
TEXT  (022),$2                           L     R       0000 0B3B
TEXT  (022),$3                           L     R       0000 0BBC
TEXT  (022),$4                           L     R       0000 0C40
TEXT  (022),$5                           L     R       0000 0CC5
TEXT  (022),$6                           L     R       0000 8CC5
TEXT  (022),$7                           L     R       0001 0CC6
TEXT  back                                     R       0001 0CC8
TEXT  (023),$1                           L     R       0001 0CC8
TEXT  (023),$21                          L     R       0001 0D44
TEXT  (023),$2                           L     R       0001 0D46
TEXT  (023),$22                          L     R       0001 0DC3
TEXT  (023),$3                           L     R       0001 0DC5
TEXT  (023),$23                          L     R       0001 0E43
TEXT  (023),$4                           L     R       0001 0E45
TEXT  (023),$24                          L     R       0001 0EC4
TEXT  (023),$5                           L     R       0001 0EC8
TEXT  (023),$25                          L     R       0001 0F48
TEXT  (023),$6                           L     R       0001 0F4C
TEXT  (023),$26                          L     R       0001 8F45
TEXT  (023),$7                           L     R       0001 8F49
TEXT  incond16                                 R       0001 8F4B
TEXT  (024),$1                           L     R       0001 8F4B
TEXT  (024),$2                           L     R       0001 8F61
TEXT  (024),$3                           L     R       0001 902C
TEXT  dep16                                    R       0001 902E
TEXT  (025),$1                           L     R       0001 902E
TEXT  (025),$2                           L     R       0001 912B
TEXT  (025),$3                           L     R       0001 922D
TEXT  (025),$4                           L     R       0001 932F
TEXT  externo16                                R       0001 9331
TEXT  (026),$1                           L     R       0001 9331
TEXT  (026),$2                           L     R       0001 9335
TEXT  lc16                                     R       0001 9337
TEXT  (027),$1                           L     R       0001 9337
TEXT  (027),$2                           L     R       0001 934B
TEXT  (027),$3                           L     R       0001 935D
TEXT  (027),$5                           L     R       0001 936D
TEXT  go16                                     R       0001 937F
TEXT  (028),$1                           L     R       0001 938F
TEXT  (028),$2                           L     R       0001 9394
TEXT  (028),$3                           L     R       0001 93A4
TEXT  (028),$4                           L     R       0001 93AC
TEXT  (028),$5                           L     R       0001 93BC
TEXT  callg16                                  R       0001 93D9
TEXT  (030),$1                           L     R       0001 93E9
TEXT  (030),$2                           L     R       0001 93EE
TEXT  (030),$3                           L     R       0001 93FE
TEXT  (030),$4                           L     R       0001 9406
TEXT  (030),$5                           L     R       0001 9416
TEXT  novofinal                                R       0001 9421
TEXT  tau1                                     R       0001 9423
DATA  tau9                                     R       0001 943C
TEXT  tau2                                     R       0001 942F
TEXT  tau11                                    R       0001 9431
TEXT  tau12                                    R       0001 9439
DATA  tau19                                    R       0001 9440
