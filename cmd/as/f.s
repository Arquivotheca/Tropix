|*
|*	Teste de FADD
|*
	faddf	8(fp)
	faddf	_alpha

	faddd	8(fp)
	faddd	_alpha

	fadd	f0,f0
	fadd	f1,f0
	fadd	f2,f0
	fadd	f3,f0
	fadd	f4,f0
	fadd	f5,f0
	fadd	f6,f0
	fadd	f7,f0

	fadd	f0
	fadd	f1
	fadd	f7

	fadd	f0,f0
	fadd	f0,f1
	fadd	f0,f2
	fadd	f0,f3
	fadd	f0,f4
	fadd	f0,f5
	fadd	f0,f6
	fadd	f0,f7

	faddp	f0,f0
	faddp	f0,f1
	faddp	f0,f2
	faddp	f0,f3
	faddp	f0,f4
	faddp	f0,f5
	faddp	f0,f6
	faddp	f0,f7

	faddp	f0
	faddp	f1
	faddp	f7
	faddp

	fiaddl	8(fp)
	fiaddl	_alpha

	fiaddw	8(fp)
	fiaddw	_alpha
1$:
	incl	r0		|* Check

|*
|*	Teste de FSUB
|*
	fsubf	8(fp)
	fsubf	_alpha

	fsubd	8(fp)
	fsubd	_alpha

	fsub	f0,f0
	fsub	f1,f0
	fsub	f2,f0
	fsub	f3,f0
	fsub	f4,f0
	fsub	f5,f0
	fsub	f6,f0
	fsub	f7,f0

	fsub	f0
	fsub	f1
	fsub	f7

	fsub	f0,f0
	fsub	f0,f1
	fsub	f0,f2
	fsub	f0,f3
	fsub	f0,f4
	fsub	f0,f5
	fsub	f0,f6
	fsub	f0,f7

	fsubp	f0,f0
	fsubp	f0,f1
	fsubp	f0,f2
	fsubp	f0,f3
	fsubp	f0,f4
	fsubp	f0,f5
	fsubp	f0,f6
	fsubp	f0,f7

	fsubp	f0
	fsubp	f1
	fsubp	f7
	fsubp

	fisubl	8(fp)
	fisubl	_alpha

	fisubw	8(fp)
	fisubw	_alpha
2$:
	incl	r0		|* Check

|*
|*	Teste de FSUBR
|*
	fsubrf	8(fp)
	fsubrf	_alpha

	fsubrd	8(fp)
	fsubrd	_alpha

	fsubr	f0,f0
	fsubr	f1,f0
	fsubr	f2,f0
	fsubr	f3,f0
	fsubr	f4,f0
	fsubr	f5,f0
	fsubr	f6,f0
	fsubr	f7,f0

	fsubr	f0
	fsubr	f1
	fsubr	f7

	fsubr	f0,f0
	fsubr	f0,f1
	fsubr	f0,f2
	fsubr	f0,f3
	fsubr	f0,f4
	fsubr	f0,f5
	fsubr	f0,f6
	fsubr	f0,f7

	fsubrp	f0,f0
	fsubrp	f0,f1
	fsubrp	f0,f2
	fsubrp	f0,f3
	fsubrp	f0,f4
	fsubrp	f0,f5
	fsubrp	f0,f6
	fsubrp	f0,f7

	fsubrp	f0
	fsubrp	f1
	fsubrp	f7
	fsubrp

	fisubrl	8(fp)
	fisubrl	_alpha

	fisubrw	8(fp)
	fisubrw	_alpha
3$:
	incl	r0		|* Check

|*
|*	Teste de FMUL
|*
	fmulf	8(fp)
	fmulf	_alpha

	fmuld	8(fp)
	fmuld	_alpha

	fmul	f0,f0
	fmul	f1,f0
	fmul	f2,f0
	fmul	f3,f0
	fmul	f4,f0
	fmul	f5,f0
	fmul	f6,f0
	fmul	f7,f0

	fmul	f0
	fmul	f1
	fmul	f7

	fmul	f0,f0
	fmul	f0,f1
	fmul	f0,f2
	fmul	f0,f3
	fmul	f0,f4
	fmul	f0,f5
	fmul	f0,f6
	fmul	f0,f7

	fmulp	f0,f0
	fmulp	f0,f1
	fmulp	f0,f2
	fmulp	f0,f3
	fmulp	f0,f4
	fmulp	f0,f5
	fmulp	f0,f6
	fmulp	f0,f7

	fmulp	f0
	fmulp	f1
	fmulp	f7
	fmulp

	fimull	8(fp)
	fimull	_alpha

	fimulw	8(fp)
	fimulw	_alpha
4$:
	incl	r0		|* Check

|*
|*	Teste de FDIV
|*
	fdivf	8(fp)
	fdivf	_alpha

	fdivd	8(fp)
	fdivd	_alpha

	fdiv	f0,f0
	fdiv	f1,f0
	fdiv	f2,f0
	fdiv	f3,f0
	fdiv	f4,f0
	fdiv	f5,f0
	fdiv	f6,f0
	fdiv	f7,f0

	fdiv	f0
	fdiv	f1
	fdiv	f7

	fdiv	f0,f0
	fdiv	f0,f1
	fdiv	f0,f2
	fdiv	f0,f3
	fdiv	f0,f4
	fdiv	f0,f5
	fdiv	f0,f6
	fdiv	f0,f7

	fdivp	f0,f0
	fdivp	f0,f1
	fdivp	f0,f2
	fdivp	f0,f3
	fdivp	f0,f4
	fdivp	f0,f5
	fdivp	f0,f6
	fdivp	f0,f7

	fdivp	f0
	fdivp	f1
	fdivp	f7
	fdivp

	fidivl	8(fp)
	fidivl	_alpha

	fidivw	8(fp)
	fidivw	_alpha
5$:
	incl	r0		|* Check

|*
|*	Teste de FDIVR
|*
	fdivrf	8(fp)
	fdivrf	_alpha

	fdivrd	8(fp)
	fdivrd	_alpha

	fdivr	f0,f0
	fdivr	f1,f0
	fdivr	f2,f0
	fdivr	f3,f0
	fdivr	f4,f0
	fdivr	f5,f0
	fdivr	f6,f0
	fdivr	f7,f0

	fdivr	f0
	fdivr	f1
	fdivr	f7

	fdivr	f0,f0
	fdivr	f0,f1
	fdivr	f0,f2
	fdivr	f0,f3
	fdivr	f0,f4
	fdivr	f0,f5
	fdivr	f0,f6
	fdivr	f0,f7

	fdivrp	f0,f0
	fdivrp	f0,f1
	fdivrp	f0,f2
	fdivrp	f0,f3
	fdivrp	f0,f4
	fdivrp	f0,f5
	fdivrp	f0,f6
	fdivrp	f0,f7

	fdivrp	f0
	fdivrp	f1
	fdivrp	f7

	fidivrl	8(fp)
	fidivrl	_alpha

	fidivrw	8(fp)
	fidivrw	_alpha
6$:
	incl	r0		|* Check

|*
|*	Teste de FTST
|*
	ftst
	ftst	f0

|*
|*	Teste de FSTSW
|*
	fstsw	8(fp)
	fstsw	_alpha

	fstsw	r0
	fstsw

	fnstsw	8(fp)
	fnstsw	_alpha

	fnstsw	r0
	fnstsw

7$:
	incl	r0		|* Check

|*
|*	Teste de FST
|*
	fstf	8(fp)
	fstf	_alpha

	fstd	8(fp)
	fstd	_alpha

	fst	f0,f0
	fst	f0,f1
	fst	f0,f7

	fst	f0
	fst	f1
	fst	f7

	fstpf	8(fp)
	fstpf	_alpha

	fstpd	8(fp)
	fstpd	_alpha

	fstpe	8(fp)
	fstpe	_alpha

	fstp	f0,f0
	fstp	f0,f1
	fstp	f0,f7

	fstp	f0
	fstp	f1
	fstp	f7
8$:
	incl	r0		|* Check

|*
|*	Teste de FCOM
|*
	fcomf	8(fp)
	fcomf	_alpha

	fcomd	8(fp)
	fcomd	_alpha

	fcom	f0,f0
	fcom	f0,f1
	fcom	f0,f7

	fcom	f0
	fcom	f1
	fcom	f7
	fcom

	fcompf	8(fp)
	fcompf	_alpha

	fcompd	8(fp)
	fcompd	_alpha

	fcomp	f0,f0
	fcomp	f0,f1
	fcomp	f0,f7

	fcomp	f0
	fcomp	f1
	fcomp	f7
	fcomp

	fcompp	f0,f1
	fcompp	f1
	fcompp
9$:
	incl	r0		|* Check

|*
|*	Teste de FILD
|*
	fildw	8(fp)
	fildw	_alpha

	fildl	8(fp)
	fildl	_alpha

	fildq	8(fp)
	fildq	_alpha

10$:
	incl	r0		|* Check

|*
|*	Teste de FCHS
|*
	fchs
	fchs	f0

11$:
	incl	r0		|* Check

|*
|*	Teste de FABS
|*
	fabs
	fabs	f0

12$:
	incl	r0		|* Check

|*
|*	Teste de FLD
|*
	fldf	8(fp)
	fldf	_alpha

	fldd	8(fp)
	fldd	_alpha

	flde	8(fp)
	flde	_alpha

	fld	f0
	fld	f1
	fld	f7
13$:
	incl	r0		|* Check

|*
|*	Teste de FSTCW
|*
	fstcw	(r0)
	fstcw	8(fp)
	fstcw	_alpha
14$:
	incl	r0		|* Check

|*
|*	Teste de FINIT
|*
	finit
15$:
	incl	r0		|* Check

|*
|*	Teste de FSAVE
|*
	fsave	(r0)
	fsave	8(fp)
	fsave	_alpha
16$:
	incl	r0		|* Check

|*
|*	Teste de FRSTOR
|*
	frstor	(r0)
	frstor	8(fp)
	frstor	_alpha
17$:
	incl	r0		|* Check

|*
|*	Teste de FLDCW
|*
	fldcw	(r0)
	fldcw	8(fp)
	fldcw	_alpha
18$:
	incl	r0		|* Check

|*
|*	Teste de FRNDINT
|*
	frndint
	frndint	f0
19$:
	incl	r0		|* Check

|*
|*	Teste de FISTL
|*
	fistw	(r0)
	fistl	(r0)
	fistpw	(r0)
	fistpl	(r0)
	fistpq	(r0)
20$:
	incl	r0		|* Check

|*
|*	Teste de FLD1, ...
|*
	fld1
	fldl2t
	fldl2e
	fldpi
	fldlg2
	fldln2
	fldz
21$:
	incl	r0		|* Check

|*
|*	Teste de FSCALE
|*
	fscale
22$:
	incl	r0		|* Check

|*
|*	Teste de F2XM1
|*
	f2xm1
23$:
	incl	r0		|* Check

|*
|*	Teste de FBLD/FBSTP
|*
	fbld	(r0)
	fbld	8(sp)
	fbld	_alpha

	fbstp	(r0)
	fbstp	8(sp)
	fbstp	_alpha
24$:
	incl	r0		|* Check

|*
|*	Teste de FCLEX
|*
	fclex
25$:
	incl	r0		|* Check

|*
|*	Teste de FCOS
|*
	fcos
	fcos	f0
26$:
	incl	r0		|* Check

|*
|*	Teste de FDECSTP
|*
	fdecstp
27$:
	incl	r0		|* Check

|*
|*	Teste de FFREE
|*
	ffree	f0
	ffree	f1
	ffree	f7
28$:
	incl	r0		|* Check

|*
|*	Teste de FICOM
|*
	ficomw	(r0)
	ficoml	(r0)
	ficompw	(r0)
	ficompl	(r0)
29$:
	incl	r0		|* Check

|*
|*	Teste de FINCSTP
|*
	fincstp
30$:
	incl	r0		|* Check

|*
|*	Teste de FLDENV
|*
	fldenv	(r0)
31$:
	incl	r0		|* Check

|*
|*	Teste de FNOP
|*
	fnop
32$:
	incl	r0		|* Check

|*
|*	Teste de FPATAN
|*
	fpatan
33$:
	incl	r0		|* Check

|*
|*	Teste de FPREM
|*
	fprem
	fprem1
34$:
	incl	r0		|* Check

|*
|*	Teste de FPTAN
|*
	fptan
35$:
	incl	r0		|* Check

|*
|*	Teste de FSIN
|*
	fsin
36$:
	incl	r0		|* Check

|*
|*	Teste de FSINCON
|*
	fsincos
37$:
	incl	r0		|* Check

|*
|*	Teste de FSQRT
|*
	fsqrt
38$:
	incl	r0		|* Check

|*
|*	Teste de FSTENV
|*
	fstenv	(r0)
39$:
	incl	r0		|* Check

|*
|*	Teste de FUCOM
|*
	fucom
	fucom	f0
	fucom	f1
	fucom	f7
	fucomp
	fucomp	f0
	fucomp	f1
	fucomp	f7
	fucompp	
	fucompp	f1
40$:
	incl	r0		|* Check

|*
|*	Teste de FXAM
|*
	fxam
41$:
	incl	r0		|* Check

|*
|*	Teste de FXCH
|*
	fxch
	fxch	f0
	fxch	f1
	fxch	f7
42$:
	incl	r0		|* Check

|*
|*	Teste de FXTRACT
|*
	fxtract
43$:
	incl	r0		|* Check

|*
|*	Teste de FYL2X
|*
	fyl2x
	fyl2xp1
44$:
	incl	r0		|* Check

|*
|*	Teste de DOUBLE
|*
	.const
	.double	1.0
	.double	-1.5
	.float	1.0
99$:
	incl	r0		|* Check

	.text

