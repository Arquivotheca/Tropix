       0000 007B        	alpha	= 123
       0000 0000        	beta	= 0
       FFFF FF9C        	delta	= -100
                        
                        |****************************************************************
                        
                        .if	alpha
000000 6A 01            	pushl	#1
                        .else
                        	.warn	"ERRO 1"
                        .endif
                        
                        |****************************************************************
                        
                        .if	beta == 0
000002 6A 02            	pushl	#2
                        .else
                        	.warn	"ERRO 2"
                        .endif
                        
                        |****************************************************************
                        
                        .ifdef	alpha
000004 6A 03            	pushl	#3
                        .else
                        	.warn	"ERRO 3"
                        .endif
                        
                        |****************************************************************
                        
                        .ifndef	gamma
000006 6A 04            	pushl	#4
                        .else
                        	.warn	"ERRO 4"
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha == 0
                        	.warn	"ERRO 5"
                        .elif	beta != 0
                        	.warn	"ERRO 6"
                        .elndef	alpha
                        	.warn	"ERRO 7"
                        .eldef	gamma
                        	.warn	"ERRO 8"
                        .else
000008 6A 05            	pushl	#5
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha != 0
                        .if	beta == 0
                        .ifdef	alpha
                        .ifdef	beta
                        .ifndef	gamma
00000A 6A 06            	pushl	#6
                        .endif
                        .endif
                        .endif
                        .endif
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha != 0
                        .if	beta == 0
                        .ifndef	alpha
                        .ifdef	beta
                        .ifndef	gamma
                        	.warn	"ERRO 9"
                        .endif
                        .endif
                        .endif
                        .endif
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha == 123
00000C 6A 07            	pushl	#7
                        .else
                        	.warn	"ERRO 10"
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha != 345
00000E 6A 08            	pushl	#8
                        .else
                        	.warn	"ERRO 11"
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha > 100
000010 6A 09            	pushl	#9
                        .else
                        	.warn	"ERRO 12"
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha >= -10
000012 6A 0A            	pushl	#10
                        .else
                        	.warn	"ERRO 13"
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha < 200
000014 6A 0B            	pushl	#11
                        .else
                        	.warn	"ERRO 14"
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha < 1000
000016 6A 0C            	pushl	#12
                        .else
                        	.warn	"ERRO 15"
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha != 123
                        	.warn	"ERRO 16"
                        .else
000018 6A 0D            	pushl	#13
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha == 345
                        	.warn	"ERRO 17"
                        .else
00001A 6A 0E            	pushl	#14
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha <= 100
                        	.warn	"ERRO 18"
                        .else
00001C 6A 0F            	pushl	#15
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha < -10
                        	.warn	"ERRO 19"
                        .else
00001E 6A 10            	pushl	#16
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha >= 200
                        	.warn	"ERRO 20"
                        .else
000020 6A 11            	pushl	#17
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha > 1000
                        	.warn	"ERRO 21"
                        .else
000022 6A 12            	pushl	#18
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha > 120 && beta == 0
000024 6A 13            	pushl	#19
                        .else
                        	.warn	"ERRO 22"
                        .endif
                        
                        .if	alpha > 120 && beta == 1
                        	.warn	"ERRO 23"
                        .else
000026 6A 14            	pushl	#20
                        .endif
                        
                        .if	alpha > 1000 && beta == 0
                        	.warn	"ERRO 24"
                        .else
000028 6A 15            	pushl	#21
                        .endif
                        
                        .if	alpha > 1000 && beta == 3
                        	.warn	"ERRO 25"
                        .else
00002A 6A 16            	pushl	#22
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha > 120 || beta == 0
00002C 6A 17            	pushl	#23
                        .else
                        	.warn	"ERRO 26"
                        .endif
                        
                        .if	alpha > 120 || beta == 1
00002E 6A 18            	pushl	#24
                        .else
                        	.warn	"ERRO 27"
                        .endif
                        
                        .if	alpha > 1000 || beta == 0
000030 6A 19            	pushl	#25
                        .else
                        	.warn	"ERRO 28"
                        .endif
                        
                        .if	alpha > 1000 || beta == 3
                        	.warn	"ERRO 29"
                        .else
000032 6A 1A            	pushl	#26
                        .endif
                        
                        |****************************************************************
                        
                        .if	alpha && beta
                        	.warn	"ERRO 30"
                        .else
000034 6A 1B            	pushl	#27
                        .endif
                        
                        .if	alpha && !beta
000036 6A 1C            	pushl	#28
                        .else
                        	.warn	"ERRO 31"
                        .endif
                        
                        |****************************************************************
                        
       0000 0003        a0	= 1 | 3
       0000 0001        a1	= 1 & 3
       0000 0002        a2	= 1 ^ 3
       0000 F000        a3	= 0x0000000F << 12
       000F 0000        a4	= 0xF0000000 >> 12
       0000 0007        a5	= 5 + 2
       0000 0003        a6	= 5 - 2
       0000 000A        a7	= 5 * 2
       0000 0002        a8	= 5 / 2
       0000 0001        a9	= 5 % 2
       FFFF FFFA        a10	= ~5
       0000 0000        a11	= !5
       0000 0001        a12	= !0
       FFFF FFFB        a13	= -5
                        
                        |****************************************************************
                        
000038                  rot:			|* Teste de defasagem
000038 6A 1D            	pushl	#29



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

ABS   alpha                                            0000 007B
ABS   beta                                             0000 0000
ABS   delta                                            FFFF FF9C
UND   gamma                                        E   0000 0000
ABS   a0                                               0000 0003
ABS   a1                                               0000 0001
ABS   a2                                               0000 0002
ABS   a3                                               0000 F000
ABS   a4                                               000F 0000
ABS   a5                                               0000 0007
ABS   a6                                               0000 0003
ABS   a7                                               0000 000A
ABS   a8                                               0000 0002
ABS   a9                                               0000 0001
ABS   a10                                              FFFF FFFA
ABS   a11                                              0000 0000
ABS   a12                                              0000 0001
ABS   a13                                              FFFF FFFB
TEXT  rot                                      R       0000 0038
