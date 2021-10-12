                        |*
                        |*	Teste de endereçamento 16 bits
                        |*
000000                  delta0:
000000 FF 08            	decw	(bx,si)
000002 FF 09            	decw	(bx,di)
000004 FF 0A            	decw	(bp,si)
000006 FF 0B            	decw	(bp,di)
000008 FF 0C            	decw	(si)
00000A FF 0D            	decw	(di)
00000C FF 0E 0000       	decw	delta0
000010 FF 0F            	decw	(bx)
                        
000012 FF 08            	decw	0(bx,si)
000014 FF 09            	decw	0(bx,di)
000016 FF 0A            	decw	0(bp,si)
000018 FF 0B            	decw	0(bp,di)
00001A FF 0C            	decw	0(si)
00001C FF 0D            	decw	0(di)
00001E FF 0F            	decw	0(bx)
                        
000020 FF 48 08         	decw	8(bx,si)
000023 FF 49 08         	decw	8(bx,di)
000026 FF 4A 08         	decw	8(bp,si)
000029 FF 4B 08         	decw	8(bp,di)
00002C FF 4C 08         	decw	8(si)
00002F FF 4D 08         	decw	8(di)
000032 FF 4E 08         	decw	8(bp)
000035 FF 4F 08         	decw	8(bx)
                        
000038 FF 88 0200       	decw	512(bx,si)
00003C FF 89 0200       	decw	512(bx,di)
000040 FF 8A 0200       	decw	512(bp,si)
000044 FF 8B 0200       	decw	512(bp,di)
000048 FF 8C 0200       	decw	512(si)
00004C FF 8D 0200       	decw	512(di)
000050 FF 8E 0200       	decw	512(bp)
000054 FF 8F 0200       	decw	512(bx)
000058                  delta4:
000058 F7 F0            	divuw	eax
00005A F7 F1            	divuw	ecx
00005C F7 F2            	divuw	edx
00005E F7 F3            	divuw	ebx
000060 F7 F4            	divuw	sp
000062 F7 F5            	divuw	fp
000064 F7 F6            	divuw	esi
000066 F7 F7            	divuw	edi
000068                  delta5:
000068 F6 F0            	divub	al
00006A F6 F1            	divub	cl
00006C F6 F2            	divub	dl
00006E F6 F3            	divub	bl
000070 F6 F4            	divub	ah
000072 F6 F5            	divub	ch
000074 F6 F6            	divub	dh
000076 F6 F7            	divub	bh
                        
                        |*
                        |*	LINK
                        |*
000078 55 89E5          	link	#0
00007B 55 89E5 83EC 7E  	link	#126
000081 55 89E5 83EC 7F  	link	#127
000087 55 89E5 81EC 0080	link	#128
00008E 55 89E5 81EC 0081	link	#129
000095 55 89E5 81EC FFFF	link	#65535
                        |*	link	#65536
                        
                        |*
                        |*	Terminou
                        |*
00009C                  final:
00009C 66 48            	decl	ra
                        



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

TEXT  delta0                                   R       0000 0000
TEXT  delta4                                   R       0000 0058
TEXT  delta5                                   R       0000 0068
TEXT  final                                    R       0000 009C
