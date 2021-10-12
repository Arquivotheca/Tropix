                        |*
                        |*	Teste de endereçamento de 32 bits
                        |*
000000                  delta0:
000000 FF 08            	decl	(eax)
000002 FF 09            	decl	(ecx)
000004 FF 0A            	decl	(edx)
000006 FF 0B            	decl	(ebx)
000008 FF 0C24          	decl	(sp)
00000B FF 4D 00         	decl	(fp)
00000E FF 0E            	decl	(esi)
000010 FF 0F            	decl	(edi)
000012 FF 0D 0000 0000  	decl	delta0
000018                  delta1:
000018 FF 08            	decl	0(eax)
00001A FF 09            	decl	0(ecx)
00001C FF 0A            	decl	0(edx)
00001E FF 0B            	decl	0(ebx)
000020 FF 0C24          	decl	0(sp)
000023 FF 4D 00         	decl	0(fp)
000026 FF 0E            	decl	0(esi)
000028 FF 0F            	decl	0(edi)
00002A                  delta2:
00002A FF 48 08         	decl	8(eax)
00002D FF 49 08         	decl	8(ecx)
000030 FF 4A 08         	decl	8(edx)
000033 FF 4B 08         	decl	8(ebx)
000036 FF 4C24 08       	decl	8(sp)
00003A FF 4D 08         	decl	8(fp)
00003D FF 4E 08         	decl	8(esi)
000040 FF 4F 08         	decl	8(edi)
000043                  delta3:
000043 FF 88 0000 0200  	decl	512(eax)
000049 FF 89 0000 0200  	decl	512(ecx)
00004F FF 8A 0000 0200  	decl	512(edx)
000055 FF 8B 0000 0200  	decl	512(ebx)
00005B FF 8C24 0000 0200	decl	512(sp)
000062 FF 8D 0000 0200  	decl	512(fp)
000068 FF 8E 0000 0200  	decl	512(esi)
00006E FF 8F 0000 0200  	decl	512(edi)
000074 FF 8F 0000 0000  	decl	delta0(edi)
00007A                  delta4:
00007A F7 F0            	divul	eax
00007C F7 F1            	divul	ecx
00007E F7 F2            	divul	edx
000080 F7 F3            	divul	ebx
000082 F7 F4            	divul	sp
000084 F7 F5            	divul	fp
000086 F7 F6            	divul	esi
000088 F7 F7            	divul	edi
00008A                  delta5:
00008A F6 F0            	divub	al
00008C F6 F1            	divub	cl
00008E F6 F2            	divub	dl
000090 F6 F3            	divub	bl
000092 F6 F4            	divub	ah
000094 F6 F5            	divub	ch
000096 F6 F6            	divub	dh
000098 F6 F7            	divub	bh
                        
                        |*
                        |*	Com índice
                        |*
00009A FF 0C10          	decl	(eax,edx)
00009D FF 0C11          	decl	(ecx,edx)
0000A0 FF 0C12          	decl	(edx,edx)
0000A3 FF 0C13          	decl	(ebx,edx)
0000A6 FF 0C14          	decl	(sp,edx)
0000A9 FF 4C15 00       	decl	(fp,edx)
0000AD FF 0C16          	decl	(esi,edx)
0000B0 FF 0C17          	decl	(edi,edx)
                        
0000B3 FF 4C10 08       	decl	8(eax,edx)
0000B7 FF 4C11 08       	decl	8(ecx,edx)
0000BB FF 4C12 08       	decl	8(edx,edx)
0000BF FF 4C13 08       	decl	8(ebx,edx)
0000C3 FF 4C14 08       	decl	8(sp,edx)
0000C7 FF 4C15 08       	decl	8(fp,edx)
0000CB FF 4C16 08       	decl	8(esi,edx)
0000CF FF 4C17 08       	decl	8(edi,edx)
                        
0000D3 FF 8C10 0000 0200	decl	512(eax,edx)
0000DA FF 8C11 0000 0200	decl	512(ecx,edx)
0000E1 FF 8C12 0000 0200	decl	512(edx,edx)
0000E8 FF 8C13 0000 0200	decl	512(ebx,edx)
0000EF FF 8C14 0000 0200	decl	512(sp,edx)
0000F6 FF 8C15 0000 0200	decl	512(fp,edx)
0000FD FF 8C16 0000 0200	decl	512(esi,edx)
000104 FF 8C17 0000 0200	decl	512(edi,edx)
00010B                  delta6:
00010B FF 0C03          	decl	(ebx,eax)
00010E FF 0C0B          	decl	(ebx,ecx)
000111 FF 0C13          	decl	(ebx,edx)
000114 FF 0C1B          	decl	(ebx,ebx)
                        |*	decl	(ebx,sp)
000117 FF 0C2B          	decl	(ebx,fp)
00011A FF 0C33          	decl	(ebx,esi)
00011D FF 0C3B          	decl	(ebx,edi)
                        
000120 FF 4C03 08       	decl	8(ebx,eax)
000124 FF 4C0B 08       	decl	8(ebx,ecx)
000128 FF 4C13 08       	decl	8(ebx,edx)
00012C FF 4C1B 08       	decl	8(ebx,ebx)
                        |*	decl	8(ebx,sp)
000130 FF 4C2B 08       	decl	8(ebx,fp)
000134 FF 4C33 08       	decl	8(ebx,esi)
000138 FF 4C3B 08       	decl	8(ebx,edi)
                        
00013C FF 8C03 0000 0200	decl	512(ebx,eax)
000143 FF 8C0B 0000 0200	decl	512(ebx,ecx)
00014A FF 8C13 0000 0200	decl	512(ebx,edx)
000151 FF 8C1B 0000 0200	decl	512(ebx,ebx)
                        |*	decl	512(ebx,sp)
000158 FF 8C2B 0000 0200	decl	512(ebx,fp)
00015F FF 8C33 0000 0200	decl	512(ebx,esi)
000166 FF 8C3B 0000 0200	decl	512(ebx,edi)
                        
                        |*
                        |*	Escalonamento
                        |*
00016D                  delta7:
00016D FF 4C3B 08       	decl	8(ebx,edi@b)
000171 FF 4C7B 08       	decl	8(ebx,edi@w)
000175 FF 4CBB 08       	decl	8(ebx,edi@l)
000179 FF 4CFB 08       	decl	8(ebx,edi@q)
                        
                        |*
                        |*	Índice mas sem base
                        |*
00017D FF 08            	decl	(*,eax)
00017F FF 09            	decl	(*,ecx)
000181 FF 0A            	decl	(*,edx)
000183 FF 0B            	decl	(*,ebx)
000185 FF 0C24          	decl	(*,sp)
000188 FF 4D 00         	decl	(*,fp)
00018B FF 0E            	decl	(*,esi)
00018D FF 0F            	decl	(*,edi)
00018F FF 0F            	decl	(,edi)
                        
000191 FF 48 08         	decl	8(*,eax)
000194 FF 49 08         	decl	8(*,ecx)
000197 FF 4A 08         	decl	8(*,edx)
00019A FF 4B 08         	decl	8(*,ebx)
00019D FF 4C24 08       	decl	8(*,sp)
0001A1 FF 4D 08         	decl	8(*,fp)
0001A4 FF 4E 08         	decl	8(*,esi)
0001A7 FF 4F 08         	decl	8(*,edi)
0001AA FF 4F 08         	decl	8(,edi)
                        
0001AD FF 88 0000 0200  	decl	512(*,eax)
0001B3 FF 89 0000 0200  	decl	512(*,ecx)
0001B9 FF 8A 0000 0200  	decl	512(*,edx)
0001BF FF 8B 0000 0200  	decl	512(*,ebx)
0001C5 FF 8C24 0000 0200	decl	512(*,sp)
0001CC FF 8D 0000 0200  	decl	512(*,fp)
0001D2 FF 8E 0000 0200  	decl	512(*,esi)
0001D8 FF 8F 0000 0200  	decl	512(*,edi)
0001DE FF 8F 0000 0200  	decl	512(,edi)
                        
0001E4 FF 0A            	decl	(*,edx@b)
0001E6 FF 0C55 0000 0000	decl	(*,edx@w)
0001ED FF 0C95 0000 0000	decl	(*,edx@l)
0001F4 FF 0CD5 0000 0000	decl	(*,edx@q)
                        
0001FB FF 4A 08         	decl	8(*,edx@b)
0001FE FF 0C55 0000 0008	decl	8(*,edx@w)
000205 FF 0C95 0000 0008	decl	8(*,edx@l)
00020C FF 0CD5 0000 0008	decl	8(*,edx@q)
                        
000213 FF 8A 0000 0200  	decl	512(*,edx@b)
000219 FF 0C55 0000 0200	decl	512(*,edx@w)
000220 FF 0C95 0000 0200	decl	512(*,edx@l)
000227 FF 0CD5 0000 0200	decl	512(*,edx@q)
                        
                        |*
                        |*	Problema do fp como base
                        |*
00022E                  delta8:
00022E FF 0A            	decl	(*,edx)
000230 FF 4A 08         	decl	8(*,edx)
000233 FF 8A 0000 0200  	decl	512(*,edx)
000239 FF 4C15 00       	decl	(fp,edx)
00023D FF 4C15 08       	decl	8(fp,edx)
000241 FF 8C15 0000 0200	decl	512(fp,edx)
                        
                        |*
                        |*	Troca de modo
                        |*
000248 89 4D 08         	movl	r1,8(fp)
                        	.seg16
00024B 66 89 4E 08      	movl	r1,8(fp)
                        	.seg32
00024F 89 4D 08         	movl	r1,8(fp)
                        
                        |*
                        |*	LINK
                        |*
000252 55 89E5          	link	#0
000255 55 89E5 83EC 7E  	link	#126
00025B 55 89E5 83EC 7F  	link	#127
000261 55 89E5 81EC 0000	link	#128
       0080
00026A 55 89E5 81EC 0000	link	#129
       0081
000273 55 89E5 81EC 0001	link	#100000
       86A0
                        
                        |*
                        |*	PUSH e POP
                        |*
00027C 66 1E            	pushw	ds
00027E 1E               	pushl	ds
00027F 66 1F            	popw	ds
000281 1F               	popl	ds
                        
                        	.seg16
000282 1E               	pushw	ds
000283 66 1E            	pushl	ds
000285 1F               	popw	ds
000286 66 1F            	popl	ds
                        
                        	.seg32
                        
                        |*
                        |*	Terminou
                        |*
000288                  final:
000288 48               	decl	ra
                        



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

TEXT  delta0                                   R       0000 0000
TEXT  delta1                                   R       0000 0018
TEXT  delta2                                   R       0000 002A
TEXT  delta3                                   R       0000 0043
TEXT  delta4                                   R       0000 007A
TEXT  delta5                                   R       0000 008A
TEXT  delta6                                   R       0000 010B
TEXT  delta7                                   R       0000 016D
TEXT  delta8                                   R       0000 022E
TEXT  final                                    R       0000 0288
