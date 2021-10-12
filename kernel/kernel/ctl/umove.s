|*
|****************************************************************
|*								*
|*			umove.s					*
|*								*
|*	Movimentação de memória de/para o espaço do usuário	*
|*								*
|*	Versão	3.0.0, de 03.12.94				*
|*		4.3.0, de 02.09.02				*
|*								*
|*	Módulo: Núcleo						*
|*		NÚCLEO do TROPIX para PC			*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2002 NCE/UFRJ - tecle "man licença"	*
|*								*
|****************************************************************
|*

|*
|****************************************************************
|*	Cópias entre espaços USER/SUPER				*
|****************************************************************
|*
|*	int	unimove (void *dst, void *src, int count, int cpflag);
|*
|*	cpflag:	SS: 0 => supervisor para supervisor
|*		SU: 1 => supervisor para usuário
|*		US: 2 => usuário    para supervisor
|*
	.global	_unimove
_unimove:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5		|* "dst"
	movl	12(fp),r4		|* "src"
	movl	16(fp),r1		|* "count"

	decl	20(fp)			|* Desvia se for SS
	jlt	memmove

|******	Cópia SUPER/USER ****************************************

	jgt	3$			|* Desvia se for US

	cmpb	#0,_mmu_dirty+CPUID	|* Atualiza a MMU
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
	movw	#USER_DS,r0
	movw	r0,es

	pushl	#unimove_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	up				|* Cópia de baixo para cima 
	lsrl	#2,r1
	rep
	movsl

	movl	16(fp),r1		|* Copia os 3 bytes restantes
	andl	#3,r1
	rep
	movsb

	movw	#KERNEL_DS,r0		|* Restaura o reg. de segmentação
	movw	r0,es

	popl	r0			|* Retira o  #bus_error da pilha
	clrl	u.u_dontpanic		|* Restaura "don't panic"

	jmp	memmoveret

|******	Cópia USER/SUPER ****************************************
3$:
	cmpb	#0,_mmu_dirty+CPUID	|* Atualiza a MMU
	jeq	4$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
4$:
   |***	movw	#USER_DS,r0
   |***	movw	r0,fs

	pushl	#unimove_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	up				|* Cópia de baixo para cima 
	lsrl	#2,r1
	rep
	seg	fs
	movsl

	movl	16(fp),r1		|* Copia os 3 bytes restantes
	andl	#3,r1
	rep
	seg	fs
	movsb

	popl	r0			|* Retira o  #bus_error da pilha
	clrl	u.u_dontpanic		|* Restaura "don't panic"

	jmp	memmoveret

|****** Em caso de BUS ERROR ************************************

unimove_bus_error:
	movw	#KERNEL_DS,r0
	movw	r0,es

	popl	r4
	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Função de cópia de áreas de memória			*
|****************************************************************
|*
|*	int	memmove (void *dst, void *src, int count);
|*
	.global	_memmove, _memcpy
_memmove:
_memcpy:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5	|* "dst"
	movl	12(fp),r4	|* "src"
	movl	16(fp),r1	|* "count"
memmove:
	cmpl	r4,r5		|* Verifica o sentido da cópia
	jhi	5$

	up			|* Cópia de baixo para cima 
	lsrl	#2,r1
	rep
	movsl

	movl	16(fp),r1	|* Copia os 3 bytes restantes
	andl	#3,r1
	rep
	movsb

	jmp	memmoveret
5$:
	down			|* Cópia de cima para baixo
	movl	r1,r0
	decl	r0
	addl	r0,r4
	addl	r0,r5

	andl	#3,r1		|* Copia inicialmente 3 bytes
	rep
	movsb

	movl	16(fp),r1	|* Copia os longos restantes
	lsrl	#2,r1
	subl	#3,r4
	subl	#3,r5

	rep
	movsl
memmoveret:
	clrl	r0		|* Retorna 0

	popl	r4
	popl	r5
	unlk
	rts

|*
|****************************************************************
|*	Obtem um byte do usuário				*
|****************************************************************
|*
|*	int	gubyte (char *uaddr);
|*
|*	Esta rotina é controlada pela MMU;
|*	Retorna (-1) se teve erro de endereçamento
|*
	.global	_gubyte
_gubyte:
	cmpb	#0,_mmu_dirty+CPUID
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
	movl	4(sp),r1		|* Obtem o endereço

	pushl	#get_put_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	seg	fs			|* Le o byte
	movzbl	(r1),r0

	popl	r1			|* Restaura "don't panic"
	clrl	u.u_dontpanic

	rts

|*
|****************************************************************
|*	Obtem um longo do usuário				*
|****************************************************************
|*
|*	long	gulong (long *uaddr);
|*
|*	Esta rotina é controlada pela MMU;
|*	Retorna (-1) se teve erro de endereçamento
|*
	.global	_gulong
_gulong:
	cmpb	#0,_mmu_dirty+CPUID
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
	movl	4(sp),r1		|* Obtem o endereço

	pushl	#get_put_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	seg	fs			|* Le o longo
	movl	(r1),r0

	popl	r1			|* Restaura "don't panic"
	clrl	u.u_dontpanic

	rts

|*
|****************************************************************
|*	Armazena um byte no usuário				*
|****************************************************************
|*
|*	int	pubyte (char *uaddr, char value);
|*
|*	Esta rotina é controlada pela MMU;
|*	Retorna (-1) se teve erro de endereçamento
|*
	.global	_pubyte
_pubyte:
	cmpb	#0,_mmu_dirty+CPUID
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
	movl	4(sp),r1		|* Obtem o endereço

	pushl	#get_put_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	movb	8+4(sp),r0		|* Armazena o byte
	seg	fs
	movb	r0,(r1)		

	popl	r1			|* Restaura "don't panic"
	clrl	u.u_dontpanic

	clrl	r0			|* Retorna zero
	rts

|*
|****************************************************************
|*	Armazena um longo no usuário				*
|****************************************************************
|*
|*	int	pulong (long *uaddr, long value);
|*
|*	Esta rotina é controlada pela MMU;
|*	Retorna (-1) se teve erro de endereçamento
|*
	.global	_pulong
_pulong:
	cmpb	#0,_mmu_dirty+CPUID
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
	movl	4(sp),r1		|* Obtem o endereço

	pushl	#get_put_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	movl	8+4(sp),r0		|* Armazena o longo
	seg	fs
	movl	r0,(r1)		

	popl	r1			|* Restaura "don't panic"
	clrl	u.u_dontpanic

	clrl	r0
	rts

|*
|****************************************************************
|	Em caso de BUS ERROR					* 
|****************************************************************
|*
get_put_bus_error:
	rts

|*
|****************************************************************
|*	Obtem uma cadeia de caracteres do usuário		*
|****************************************************************
|*
|*	int	get_user_str (char *dst, const char *src, int count);
|*
|*	Retorna o número de caracteres copiados (sem o NULO)
|*	Se "dst" e "count" forem NULOs, apenas conta o número de caracteres
|*
|*	Esta rotina é controlada pela MMU;
|*	Retorna (-1) se teve erro de endereçamento
|*
	.global	_get_user_str
_get_user_str:
	link	#0
	pushl	r5
	pushl	r4

	movl	8(fp),r5		|* "dst"
	movl	12(fp),r4		|* "src"
	movl	16(fp),r1		|* "count"

	cmpb	#0,_mmu_dirty+CPUID
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
   |***	movw	#USER_DS,r0
   |***	movw	r0,fs

	pushl	#unimove_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	up				|* Cópia de baixo para cima 

	tstl	r5			|* Desvia se for só "strlen"
	jz	5$

|****** "strcpy" ************************************************

	clrb	-1(r5,r1)		|* Garante o final NULO
3$:
	decl	r1
	jlt	9$

	seg	fs			|* Le o byte
	lodsb
   |***	movb	(r4)+,r0

	stosb
   |***	movb	r0,(r5)+

	tstb	r0
	jnz	3$

	jmp	9$

|****** "strlen" ************************************************

5$:
	seg	fs			|* Le o byte
	lodsb
   |***	movb	(r4)+,r0

	tstb	r0
	jnz	5$

|****** Epílogo *************************************************

9$:
	movl	r4,r0			|* Calcula "len" (sem o NULO)
	subl	12(fp),r0
	decl	r0

	popl	r1			|* Restaura "don't panic"
	clrl	u.u_dontpanic

	popl	r4
	popl	r5
	unlk
	rts

.if 0	|*******************************************************|
|*
|****************************************************************
|*	Obtem um caminho do usuário				*
|****************************************************************
|*
|*	void	init_user_path (char *dst, const char *src, int count, int *next);
|*
|*	int	get_user_path (void);
|*
|*	Retorna:
|*		 < 0:	Barra inicial
|*		== 0:	Final de cadeia (EOF)
|*		 > 0:	Comprimento do nome em "dst" (sem o NULO)
|*
|*	Esta rotina é controlada pela MMU;
|*	Retorna (-1) se teve erro de endereçamento
|*
	.text
	.global	_init_user_path
_init_user_path:
	movl	4(sp),r0
	movl	r0,user_path_dst

	movl	8(sp),r0
	movl	r0,user_path_src

	movl	12(sp),r0
	movl	r0,user_path_cnt

	movl	16(sp),r0
	movl	r0,user_path_next

	rts

	.text
	.global	_get_user_path
_get_user_path:
	link	#0
	pushl	r5
	pushl	r4

	movl	user_path_dst,r5	|* "dst"
	movl	user_path_src,r4	|* "src"
   |***	movl	user_path_cnt,r1	|* "cnt"

	cmpb	#0,_mmu_dirty+CPUID
	jeq	1$

	movl	u.u_mmu_dir,r0
	andl	#~SYS_ADDR,r0
	movl	r0,cr3	

	clrb	_mmu_dirty+CPUID
1$:
   |***	movw	#USER_DS,r0
   |***	movw	r0,fs

	pushl	#unimove_bus_error	|* Prepara "don't panic"
	movl	sp,u.u_dontpanic

	up				|* Cópia de baixo para cima 

|****** Verifica se começa com '/' ou EOF ***********************

10$:
	seg	fs			|* Le um byte
	lodsb
   |***	movb	(r4)+,r0

	clrl	r1			|* Atual == EOF, next = EOF
	clrl	r2

	tstb	r0			|* Desvia com r1 == 0 se EOF
	jz	40$

	decl	r1

	cmpb	#'/',r0			|* Desvia com r1 < 0 se '/'
	jeq	36$

|****** "strcpy" ************************************************

	movl	user_path_cnt,r1	|* r1 == "cnt" - 1
	decl	r1
20$:
	decl	r1
	jlt	22$

	stosb				|* Armazena um caractere
   |***	movb	r0,(r5)+
22$:
	seg	fs			|* Le um byte
	lodsb
   |***	movb	(r4)+,r0

	tstb	r0
	jz	32$

	cmpb	#'/',r0
	jne	20$

	clrb	r0			|* Coloca o NULO final
32$:
	movl	r5,r1			|* Calcula o tamanho
	subl	user_path_dst,r1

	stosb
   |***	movb	r0,(r5)+

|****** Pula as barras ao final *********************************

	seg	fs			|* Desvia se NÃO foi um '/'
	cmpb	#0,-1(r4)
	jz	38$
36$:
	seg	fs			|* Le um byte
	lodsb
   |***	movb	(r4)+,r0

	cmpb	#'/',r0
	jeq	36$
38$:
	decl	r4			|* Próximo caractere

	clrl	r2			|* Calcula o "next": EOF ou 1

	tstb	r0
	jz	40$

	incl	r2			|* "next": 1 (componente)

|****** Epílogo *************************************************

40$:
	movl	r4,user_path_src	|* "src"

	popl	r0			|* Restaura "don't panic"
	clrl	u.u_dontpanic

	movl	user_path_next,r0	|* Armazena o "next"
	movl	r2,(r0)

	movl	r1,r0			|* Guarda o valor de retorno

	popl	r4
	popl	r5
	unlk
	rts

|****** Área de Dados *******************************************

	.data

user_path_dst:
	.long	0

user_path_src:
	.long	0

user_path_cnt:
	.long	0

user_path_next:
	.long	0
.endif	|*******************************************************|
