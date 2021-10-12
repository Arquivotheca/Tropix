                        |*
                        |*	Teste de FADD
                        |*
000000 D8 45 08         	faddf	8(fp)
000003 D8 05 0000 0000  	faddf	_alpha
                        
000009 DC 45 08         	faddd	8(fp)
00000C DC 05 0000 0000  	faddd	_alpha
                        
000012 D8 C0            	fadd	f0,f0
000014 D8 C1            	fadd	f1,f0
000016 D8 C2            	fadd	f2,f0
000018 D8 C3            	fadd	f3,f0
00001A D8 C4            	fadd	f4,f0
00001C D8 C5            	fadd	f5,f0
00001E D8 C6            	fadd	f6,f0
000020 D8 C7            	fadd	f7,f0
                        
000022 D8 C0            	fadd	f0
000024 D8 C1            	fadd	f1
000026 D8 C7            	fadd	f7
                        
000028 D8 C0            	fadd	f0,f0
00002A DC C1            	fadd	f0,f1
00002C DC C2            	fadd	f0,f2
00002E DC C3            	fadd	f0,f3
000030 DC C4            	fadd	f0,f4
000032 DC C5            	fadd	f0,f5
000034 DC C6            	fadd	f0,f6
000036 DC C7            	fadd	f0,f7
                        
000038 DE C0            	faddp	f0,f0
00003A DE C1            	faddp	f0,f1
00003C DE C2            	faddp	f0,f2
00003E DE C3            	faddp	f0,f3
000040 DE C4            	faddp	f0,f4
000042 DE C5            	faddp	f0,f5
000044 DE C6            	faddp	f0,f6
000046 DE C7            	faddp	f0,f7
                        
000048 DE C0            	faddp	f0
00004A DE C1            	faddp	f1
00004C DE C7            	faddp	f7
00004E DEC1             	faddp
                        
000050 DA 45 08         	fiaddl	8(fp)
000053 DA 05 0000 0000  	fiaddl	_alpha
                        
000059 DE 45 08         	fiaddw	8(fp)
00005C DE 05 0000 0000  	fiaddw	_alpha
000062                  1$:
000062 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSUB
                        |*
000063 D8 65 08         	fsubf	8(fp)
000066 D8 25 0000 0000  	fsubf	_alpha
                        
00006C DC 65 08         	fsubd	8(fp)
00006F DC 25 0000 0000  	fsubd	_alpha
                        
000075 D8 E0            	fsub	f0,f0
000077 D8 E1            	fsub	f1,f0
000079 D8 E2            	fsub	f2,f0
00007B D8 E3            	fsub	f3,f0
00007D D8 E4            	fsub	f4,f0
00007F D8 E5            	fsub	f5,f0
000081 D8 E6            	fsub	f6,f0
000083 D8 E7            	fsub	f7,f0
                        
000085 D8 E0            	fsub	f0
000087 D8 E1            	fsub	f1
000089 D8 E7            	fsub	f7
                        
00008B D8 E0            	fsub	f0,f0
00008D DC E9            	fsub	f0,f1
00008F DC EA            	fsub	f0,f2
000091 DC EB            	fsub	f0,f3
000093 DC EC            	fsub	f0,f4
000095 DC ED            	fsub	f0,f5
000097 DC EE            	fsub	f0,f6
000099 DC EF            	fsub	f0,f7
                        
00009B DE E8            	fsubp	f0,f0
00009D DE E9            	fsubp	f0,f1
00009F DE EA            	fsubp	f0,f2
0000A1 DE EB            	fsubp	f0,f3
0000A3 DE EC            	fsubp	f0,f4
0000A5 DE ED            	fsubp	f0,f5
0000A7 DE EE            	fsubp	f0,f6
0000A9 DE EF            	fsubp	f0,f7
                        
0000AB DE E8            	fsubp	f0
0000AD DE E9            	fsubp	f1
0000AF DE EF            	fsubp	f7
0000B1 DEE9             	fsubp
                        
0000B3 DA 65 08         	fisubl	8(fp)
0000B6 DA 25 0000 0000  	fisubl	_alpha
                        
0000BC DE 65 08         	fisubw	8(fp)
0000BF DE 25 0000 0000  	fisubw	_alpha
0000C5                  2$:
0000C5 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSUBR
                        |*
0000C6 D8 6D 08         	fsubrf	8(fp)
0000C9 D8 2D 0000 0000  	fsubrf	_alpha
                        
0000CF DC 6D 08         	fsubrd	8(fp)
0000D2 DC 2D 0000 0000  	fsubrd	_alpha
                        
0000D8 D8 E8            	fsubr	f0,f0
0000DA D8 E9            	fsubr	f1,f0
0000DC D8 EA            	fsubr	f2,f0
0000DE D8 EB            	fsubr	f3,f0
0000E0 D8 EC            	fsubr	f4,f0
0000E2 D8 ED            	fsubr	f5,f0
0000E4 D8 EE            	fsubr	f6,f0
0000E6 D8 EF            	fsubr	f7,f0
                        
0000E8 D8 E8            	fsubr	f0
0000EA D8 E9            	fsubr	f1
0000EC D8 EF            	fsubr	f7
                        
0000EE D8 E8            	fsubr	f0,f0
0000F0 DC E1            	fsubr	f0,f1
0000F2 DC E2            	fsubr	f0,f2
0000F4 DC E3            	fsubr	f0,f3
0000F6 DC E4            	fsubr	f0,f4
0000F8 DC E5            	fsubr	f0,f5
0000FA DC E6            	fsubr	f0,f6
0000FC DC E7            	fsubr	f0,f7
                        
0000FE DE E0            	fsubrp	f0,f0
000100 DE E1            	fsubrp	f0,f1
000102 DE E2            	fsubrp	f0,f2
000104 DE E3            	fsubrp	f0,f3
000106 DE E4            	fsubrp	f0,f4
000108 DE E5            	fsubrp	f0,f5
00010A DE E6            	fsubrp	f0,f6
00010C DE E7            	fsubrp	f0,f7
                        
00010E DE E0            	fsubrp	f0
000110 DE E1            	fsubrp	f1
000112 DE E7            	fsubrp	f7
000114 DEE1             	fsubrp
                        
000116 DA 6D 08         	fisubrl	8(fp)
000119 DA 2D 0000 0000  	fisubrl	_alpha
                        
00011F DE 6D 08         	fisubrw	8(fp)
000122 DE 2D 0000 0000  	fisubrw	_alpha
000128                  3$:
000128 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FMUL
                        |*
000129 D8 4D 08         	fmulf	8(fp)
00012C D8 0D 0000 0000  	fmulf	_alpha
                        
000132 DC 4D 08         	fmuld	8(fp)
000135 DC 0D 0000 0000  	fmuld	_alpha
                        
00013B D8 C8            	fmul	f0,f0
00013D D8 C9            	fmul	f1,f0
00013F D8 CA            	fmul	f2,f0
000141 D8 CB            	fmul	f3,f0
000143 D8 CC            	fmul	f4,f0
000145 D8 CD            	fmul	f5,f0
000147 D8 CE            	fmul	f6,f0
000149 D8 CF            	fmul	f7,f0
                        
00014B D8 C8            	fmul	f0
00014D D8 C9            	fmul	f1
00014F D8 CF            	fmul	f7
                        
000151 D8 C8            	fmul	f0,f0
000153 DC C9            	fmul	f0,f1
000155 DC CA            	fmul	f0,f2
000157 DC CB            	fmul	f0,f3
000159 DC CC            	fmul	f0,f4
00015B DC CD            	fmul	f0,f5
00015D DC CE            	fmul	f0,f6
00015F DC CF            	fmul	f0,f7
                        
000161 DE C8            	fmulp	f0,f0
000163 DE C9            	fmulp	f0,f1
000165 DE CA            	fmulp	f0,f2
000167 DE CB            	fmulp	f0,f3
000169 DE CC            	fmulp	f0,f4
00016B DE CD            	fmulp	f0,f5
00016D DE CE            	fmulp	f0,f6
00016F DE CF            	fmulp	f0,f7
                        
000171 DE C8            	fmulp	f0
000173 DE C9            	fmulp	f1
000175 DE CF            	fmulp	f7
000177 DEC9             	fmulp
                        
000179 DA 4D 08         	fimull	8(fp)
00017C DA 0D 0000 0000  	fimull	_alpha
                        
000182 DE 4D 08         	fimulw	8(fp)
000185 DE 0D 0000 0000  	fimulw	_alpha
00018B                  4$:
00018B 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FDIV
                        |*
00018C D8 75 08         	fdivf	8(fp)
00018F D8 35 0000 0000  	fdivf	_alpha
                        
000195 DC 75 08         	fdivd	8(fp)
000198 DC 35 0000 0000  	fdivd	_alpha
                        
00019E D8 F0            	fdiv	f0,f0
0001A0 D8 F1            	fdiv	f1,f0
0001A2 D8 F2            	fdiv	f2,f0
0001A4 D8 F3            	fdiv	f3,f0
0001A6 D8 F4            	fdiv	f4,f0
0001A8 D8 F5            	fdiv	f5,f0
0001AA D8 F6            	fdiv	f6,f0
0001AC D8 F7            	fdiv	f7,f0
                        
0001AE D8 F0            	fdiv	f0
0001B0 D8 F1            	fdiv	f1
0001B2 D8 F7            	fdiv	f7
                        
0001B4 D8 F0            	fdiv	f0,f0
0001B6 DC F9            	fdiv	f0,f1
0001B8 DC FA            	fdiv	f0,f2
0001BA DC FB            	fdiv	f0,f3
0001BC DC FC            	fdiv	f0,f4
0001BE DC FD            	fdiv	f0,f5
0001C0 DC FE            	fdiv	f0,f6
0001C2 DC FF            	fdiv	f0,f7
                        
0001C4 DE F8            	fdivp	f0,f0
0001C6 DE F9            	fdivp	f0,f1
0001C8 DE FA            	fdivp	f0,f2
0001CA DE FB            	fdivp	f0,f3
0001CC DE FC            	fdivp	f0,f4
0001CE DE FD            	fdivp	f0,f5
0001D0 DE FE            	fdivp	f0,f6
0001D2 DE FF            	fdivp	f0,f7
                        
0001D4 DE F8            	fdivp	f0
0001D6 DE F9            	fdivp	f1
0001D8 DE FF            	fdivp	f7
0001DA DEF9             	fdivp
                        
0001DC DA 75 08         	fidivl	8(fp)
0001DF DA 35 0000 0000  	fidivl	_alpha
                        
0001E5 DE 75 08         	fidivw	8(fp)
0001E8 DE 35 0000 0000  	fidivw	_alpha
0001EE                  5$:
0001EE 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FDIVR
                        |*
0001EF D8 7D 08         	fdivrf	8(fp)
0001F2 D8 3D 0000 0000  	fdivrf	_alpha
                        
0001F8 DC 7D 08         	fdivrd	8(fp)
0001FB DC 3D 0000 0000  	fdivrd	_alpha
                        
000201 D8 F8            	fdivr	f0,f0
000203 D8 F9            	fdivr	f1,f0
000205 D8 FA            	fdivr	f2,f0
000207 D8 FB            	fdivr	f3,f0
000209 D8 FC            	fdivr	f4,f0
00020B D8 FD            	fdivr	f5,f0
00020D D8 FE            	fdivr	f6,f0
00020F D8 FF            	fdivr	f7,f0
                        
000211 D8 F8            	fdivr	f0
000213 D8 F9            	fdivr	f1
000215 D8 FF            	fdivr	f7
                        
000217 D8 F8            	fdivr	f0,f0
000219 DC F1            	fdivr	f0,f1
00021B DC F2            	fdivr	f0,f2
00021D DC F3            	fdivr	f0,f3
00021F DC F4            	fdivr	f0,f4
000221 DC F5            	fdivr	f0,f5
000223 DC F6            	fdivr	f0,f6
000225 DC F7            	fdivr	f0,f7
                        
000227 DE F0            	fdivrp	f0,f0
000229 DE F1            	fdivrp	f0,f1
00022B DE F2            	fdivrp	f0,f2
00022D DE F3            	fdivrp	f0,f3
00022F DE F4            	fdivrp	f0,f4
000231 DE F5            	fdivrp	f0,f5
000233 DE F6            	fdivrp	f0,f6
000235 DE F7            	fdivrp	f0,f7
                        
000237 DE F0            	fdivrp	f0
000239 DE F1            	fdivrp	f1
00023B DE F7            	fdivrp	f7
                        
00023D DA 7D 08         	fidivrl	8(fp)
000240 DA 3D 0000 0000  	fidivrl	_alpha
                        
000246 DE 7D 08         	fidivrw	8(fp)
000249 DE 3D 0000 0000  	fidivrw	_alpha
00024F                  6$:
00024F 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FTST
                        |*
000250 D9E4             	ftst
000252 D9E4             	ftst	f0
                        
                        |*
                        |*	Teste de FSTSW
                        |*
000254 9B DD 7D 08      	fstsw	8(fp)
000258 9B DD 3D 0000 0000	fstsw	_alpha
                        
00025F 9B DFE0          	fstsw	r0
000262 9B DFE0          	fstsw
                        
000265 DD 7D 08         	fnstsw	8(fp)
000268 DD 3D 0000 0000  	fnstsw	_alpha
                        
00026E DFE0             	fnstsw	r0
000270 DFE0             	fnstsw
                        
000272                  7$:
000272 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FST
                        |*
000273 D9 55 08         	fstf	8(fp)
000276 D9 15 0000 0000  	fstf	_alpha
                        
00027C DD 55 08         	fstd	8(fp)
00027F DD 15 0000 0000  	fstd	_alpha
                        
000285 DD D0            	fst	f0,f0
000287 DD D1            	fst	f0,f1
000289 DD D7            	fst	f0,f7
                        
00028B DD D0            	fst	f0
00028D DD D1            	fst	f1
00028F DD D7            	fst	f7
                        
000291 D9 5D 08         	fstpf	8(fp)
000294 D9 1D 0000 0000  	fstpf	_alpha
                        
00029A DD 5D 08         	fstpd	8(fp)
00029D DD 1D 0000 0000  	fstpd	_alpha
                        
0002A3 DB 7D 08         	fstpe	8(fp)
0002A6 DB 3D 0000 0000  	fstpe	_alpha
                        
0002AC DD D8            	fstp	f0,f0
0002AE DD D9            	fstp	f0,f1
0002B0 DD DF            	fstp	f0,f7
                        
0002B2 DD D8            	fstp	f0
0002B4 DD D9            	fstp	f1
0002B6 DD DF            	fstp	f7
0002B8                  8$:
0002B8 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FCOM
                        |*
0002B9 D8 55 08         	fcomf	8(fp)
0002BC D8 15 0000 0000  	fcomf	_alpha
                        
0002C2 DC 55 08         	fcomd	8(fp)
0002C5 DC 15 0000 0000  	fcomd	_alpha
                        
0002CB D8 D0            	fcom	f0,f0
0002CD D8 D1            	fcom	f0,f1
0002CF D8 D7            	fcom	f0,f7
                        
0002D1 D8 D0            	fcom	f0
0002D3 D8 D1            	fcom	f1
0002D5 D8 D7            	fcom	f7
0002D7 D8D1             	fcom
                        
0002D9 D8 5D 08         	fcompf	8(fp)
0002DC D8 1D 0000 0000  	fcompf	_alpha
                        
0002E2 DC 5D 08         	fcompd	8(fp)
0002E5 DC 1D 0000 0000  	fcompd	_alpha
                        
0002EB D8 D8            	fcomp	f0,f0
0002ED D8 D9            	fcomp	f0,f1
0002EF D8 DF            	fcomp	f0,f7
                        
0002F1 D8 D8            	fcomp	f0
0002F3 D8 D9            	fcomp	f1
0002F5 D8 DF            	fcomp	f7
0002F7 D8D9             	fcomp
                        
0002F9 DE D9            	fcompp	f0,f1
0002FB DE D9            	fcompp	f1
0002FD DED9             	fcompp
0002FF                  9$:
0002FF 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FILD
                        |*
000300 DF 45 08         	fildw	8(fp)
000303 DF 05 0000 0000  	fildw	_alpha
                        
000309 DB 45 08         	fildl	8(fp)
00030C DB 05 0000 0000  	fildl	_alpha
                        
000312 DF 6D 08         	fildq	8(fp)
000315 DF 2D 0000 0000  	fildq	_alpha
                        
00031B                  10$:
00031B 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FCHS
                        |*
00031C D9E0             	fchs
00031E D9E0             	fchs	f0
                        
000320                  11$:
000320 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FABS
                        |*
000321 D9E1             	fabs
000323 D9E1             	fabs	f0
                        
000325                  12$:
000325 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FLD
                        |*
000326 D9 45 08         	fldf	8(fp)
000329 D9 05 0000 0000  	fldf	_alpha
                        
00032F DD 45 08         	fldd	8(fp)
000332 DD 05 0000 0000  	fldd	_alpha
                        
000338 DB 6D 08         	flde	8(fp)
00033B DB 2D 0000 0000  	flde	_alpha
                        
000341 D9 C0            	fld	f0
000343 D9 C1            	fld	f1
000345 D9 C7            	fld	f7
000347                  13$:
000347 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSTCW
                        |*
000348 D9 38            	fstcw	(r0)
00034A D9 7D 08         	fstcw	8(fp)
00034D D9 3D 0000 0000  	fstcw	_alpha
000353                  14$:
000353 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FINIT
                        |*
000354 DBE3             	finit
000356                  15$:
000356 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSAVE
                        |*
000357 DD 30            	fsave	(r0)
000359 DD 75 08         	fsave	8(fp)
00035C DD 35 0000 0000  	fsave	_alpha
000362                  16$:
000362 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FRSTOR
                        |*
000363 DD 20            	frstor	(r0)
000365 DD 65 08         	frstor	8(fp)
000368 DD 25 0000 0000  	frstor	_alpha
00036E                  17$:
00036E 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FLDCW
                        |*
00036F D9 28            	fldcw	(r0)
000371 D9 6D 08         	fldcw	8(fp)
000374 D9 2D 0000 0000  	fldcw	_alpha
00037A                  18$:
00037A 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FRNDINT
                        |*
00037B D9FC             	frndint
00037D D9FC             	frndint	f0
00037F                  19$:
00037F 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FISTL
                        |*
000380 DF 10            	fistw	(r0)
000382 DB 10            	fistl	(r0)
000384 DF 18            	fistpw	(r0)
000386 DB 18            	fistpl	(r0)
000388 DF 38            	fistpq	(r0)
00038A                  20$:
00038A 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FLD1, ...
                        |*
00038B D9E8             	fld1
00038D D9E9             	fldl2t
00038F D9EA             	fldl2e
000391 D9EB             	fldpi
000393 D9EC             	fldlg2
000395 D9ED             	fldln2
000397 D9EE             	fldz
000399                  21$:
000399 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSCALE
                        |*
00039A D9FD             	fscale
00039C                  22$:
00039C 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de F2XM1
                        |*
00039D D9F0             	f2xm1
00039F                  23$:
00039F 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FBLD/FBSTP
                        |*
0003A0 DF 20            	fbld	(r0)
0003A2 DF 6424 08       	fbld	8(sp)
0003A6 DF 25 0000 0000  	fbld	_alpha
                        
0003AC DF 30            	fbstp	(r0)
0003AE DF 7424 08       	fbstp	8(sp)
0003B2 DF 35 0000 0000  	fbstp	_alpha
0003B8                  24$:
0003B8 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FCLEX
                        |*
0003B9 DBE2             	fclex
0003BB                  25$:
0003BB 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FCOS
                        |*
0003BC D9FF             	fcos
0003BE D9FF             	fcos	f0
0003C0                  26$:
0003C0 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FDECSTP
                        |*
0003C1 D9F6             	fdecstp
0003C3                  27$:
0003C3 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FFREE
                        |*
0003C4 DD C0            	ffree	f0
0003C6 DD C1            	ffree	f1
0003C8 DD C7            	ffree	f7
0003CA                  28$:
0003CA 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FICOM
                        |*
0003CB DE 10            	ficomw	(r0)
0003CD DA 10            	ficoml	(r0)
0003CF DE 18            	ficompw	(r0)
0003D1 DA 18            	ficompl	(r0)
0003D3                  29$:
0003D3 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FINCSTP
                        |*
0003D4 D9F7             	fincstp
0003D6                  30$:
0003D6 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FLDENV
                        |*
0003D7 D9 20            	fldenv	(r0)
0003D9                  31$:
0003D9 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FNOP
                        |*
0003DA D9D0             	fnop
0003DC                  32$:
0003DC 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FPATAN
                        |*
0003DD D9F3             	fpatan
0003DF                  33$:
0003DF 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FPREM
                        |*
0003E0 D9F8             	fprem
0003E2 D9F5             	fprem1
0003E4                  34$:
0003E4 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FPTAN
                        |*
0003E5 D9F2             	fptan
0003E7                  35$:
0003E7 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSIN
                        |*
0003E8 D9FE             	fsin
0003EA                  36$:
0003EA 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSINCON
                        |*
0003EB D9FB             	fsincos
0003ED                  37$:
0003ED 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSQRT
                        |*
0003EE D9FA             	fsqrt
0003F0                  38$:
0003F0 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FSTENV
                        |*
0003F1 D9 30            	fstenv	(r0)
0003F3                  39$:
0003F3 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FUCOM
                        |*
0003F4 DDE1             	fucom
0003F6 DD E0            	fucom	f0
0003F8 DD E1            	fucom	f1
0003FA DD E7            	fucom	f7
0003FC DDE9             	fucomp
0003FE DD E8            	fucomp	f0
000400 DD E9            	fucomp	f1
000402 DD EF            	fucomp	f7
000404 DAE9             	fucompp	
000406 DAE9             	fucompp	f1
000408                  40$:
000408 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FXAM
                        |*
000409 D9E5             	fxam
00040B                  41$:
00040B 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FXCH
                        |*
00040C D9C9             	fxch
00040E D9 C8            	fxch	f0
000410 D9 C9            	fxch	f1
000412 D9 CF            	fxch	f7
000414                  42$:
000414 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FXTRACT
                        |*
000415 D9F4             	fxtract
000417                  43$:
000417 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de FYL2X
                        |*
000418 D9F1             	fyl2x
00041A D9F9             	fyl2xp1
00041C                  44$:
00041C 40               	incl	r0		|* Check
                        
                        |*
                        |*	Teste de DOUBLE
                        |*
                        	.const
000420 0000 0000 0000   	.double	1.0
       F03F
000428 0000 0000 0000   	.double	-1.5
       F8BF
000430 0000 803F        	.float	1.0
000434                  99$:
000434 40               	incl	r0		|* Check
                        
                        	.text
                        



TIPO  ------------SÍMBOLO------------  LOCAL  ROT EXT  --VALOR--

UND   _alpha                                       E   0000 0000
TEXT  (000),$1                           L     R       0000 0062
TEXT  (000),$2                           L     R       0000 00C5
TEXT  (000),$3                           L     R       0000 0128
TEXT  (000),$4                           L     R       0000 018B
TEXT  (000),$5                           L     R       0000 01EE
TEXT  (000),$6                           L     R       0000 024F
TEXT  (000),$7                           L     R       0000 0272
TEXT  (000),$8                           L     R       0000 02B8
TEXT  (000),$9                           L     R       0000 02FF
TEXT  (000),$10                          L     R       0000 031B
TEXT  (000),$11                          L     R       0000 0320
TEXT  (000),$12                          L     R       0000 0325
TEXT  (000),$13                          L     R       0000 0347
TEXT  (000),$14                          L     R       0000 0353
TEXT  (000),$15                          L     R       0000 0356
TEXT  (000),$16                          L     R       0000 0362
TEXT  (000),$17                          L     R       0000 036E
TEXT  (000),$18                          L     R       0000 037A
TEXT  (000),$19                          L     R       0000 037F
TEXT  (000),$20                          L     R       0000 038A
TEXT  (000),$21                          L     R       0000 0399
TEXT  (000),$22                          L     R       0000 039C
TEXT  (000),$23                          L     R       0000 039F
TEXT  (000),$24                          L     R       0000 03B8
TEXT  (000),$25                          L     R       0000 03BB
TEXT  (000),$26                          L     R       0000 03C0
TEXT  (000),$27                          L     R       0000 03C3
TEXT  (000),$28                          L     R       0000 03CA
TEXT  (000),$29                          L     R       0000 03D3
TEXT  (000),$30                          L     R       0000 03D6
TEXT  (000),$31                          L     R       0000 03D9
TEXT  (000),$32                          L     R       0000 03DC
TEXT  (000),$33                          L     R       0000 03DF
TEXT  (000),$34                          L     R       0000 03E4
TEXT  (000),$35                          L     R       0000 03E7
TEXT  (000),$36                          L     R       0000 03EA
TEXT  (000),$37                          L     R       0000 03ED
TEXT  (000),$38                          L     R       0000 03F0
TEXT  (000),$39                          L     R       0000 03F3
TEXT  (000),$40                          L     R       0000 0408
TEXT  (000),$41                          L     R       0000 040B
TEXT  (000),$42                          L     R       0000 0414
TEXT  (000),$43                          L     R       0000 0417
TEXT  (000),$44                          L     R       0000 041C
CONST (001),$99                          L     R       0000 0434
