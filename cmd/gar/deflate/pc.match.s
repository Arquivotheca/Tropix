|*
|****************************************************************
|*								*
|*			pc.match.s				*
|*								*
|*	Busca de padrão otimizado em "assembly" (INTEL 486)	*
|*								*
|*	Versão	3.0.00, de 22.05.95				*
|*		3.1.06, de 04.05.97				*
|*								*
|*	Módulo: GZIP						*
|*		Utilitários de compressão/descompressão		*
|*		Categoria B					*
|*								*
|*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
|*		Copyright © 2000 NCE/UFRJ - tecle "man licença"	*
|*		Baseado em software homônimo do GNU		*
|*								*
|****************************************************************
|*

|*
|*	Constantes
|*
MAX_MATCH	= 258
MAX_MATCH2	= 128		|* MAX_MATCH/2-1
MIN_MATCH	= 3
WSIZE		= 32768
MAX_DIST	= WSIZE - MAX_MATCH - MIN_MATCH - 1

|*
|*	int	longest_match (cur_match)
|*
|*	Versão para alocação dinâmica das tabelas!
|*
	.global _longest_match
_longest_match:

CUR_MATCH	=	20		|* sp+20
	|* return address		|* sp+16
	pushl	fp			|* sp+12
	pushl	r5			|* sp+8
	pushl	r4			|* sp+4
	pushl	r3			|* sp
LIMIT		=	-4		|* sp-4
TMP		=	-8		|* sp-8

|*
|*	match		== r4
|*	scan		== r5
|*	chain_length	== fp
|*	best_len	== r3
|*	window		== r2
|*
	up
	movl	_window,r2
	movl	CUR_MATCH(sp),r4
	movl	_max_chain_length,fp	|* chain_length = max_chain_length */
	movl	_strstart,r5
	movl	r5,LIMIT(sp)
	subl	#MAX_DIST,LIMIT(sp)	|* limit = strstart-MAX_DIST */
	jhs	limit_ok
	clrl	LIMIT(sp)		|* limit = NIL */
limit_ok:
	addl	r2,r5
	addl	#2,r5			|* r5 = offset(window+strstart+2) */
	movl	_prev_length,r3		|* best_len = prev_length */
	movw	-3(r3,r5),r0		|* r0 = scan[best_len-1..best_len] */
	movw	-2(r5),r1		|* r1 = scan[0..1] */
	cmpl	_good_match,r3		|* do we have a good match already? */
	jlo	do_scan
	lsrl	#2,fp			|* chain_length >>= 2 */
	jmp	do_scan

|*
|*	At this point, r5 == scan+2, r4 == cur_match */
|*
long_loop:
	movw	-3(r3,r5),r0		|* r0 = scan[best_len-1..best_len] */
	movw	-2(r5),r1		|* r1 = scan[0..1] */

|*
|*	At this point, r5 == scan+2, r4 == cur_match,
|*	r0 = scan[best_len-1..best_len] and r1 = scan[0..1]
|*
short_loop:
	andl	#WSIZE-1,r4
	lsll	#1,r4
	addl	_prev,r4
	movzwl	(r4),r4			|* cur_match = prev[cur_match] */
					|* top word of r4 is still 0 */
	cmpl	LIMIT(sp),r4		|* cur_match <= limit ? */
	jls	the_end
	decl	fp			|* --chain_length */
	jz	the_end

do_scan:
	movl	r1,TMP(sp)
	leal	(r3,r4),r1
	cmpw	-1(r1,r2),r0		|* check match at best_len-1 */
	movl	TMP(sp),r1
	jne	short_loop

	cmpw	(r2,r4),r1		|* check min_match_length match */
	jne	short_loop

	addl	r2,r4
	addl	#2,r4			|* r4 = match */
	movl	r5,r0			|* r0 = scan+2 */
	movl	#MAX_MATCH2,r1		|* scan for at most MAX_MATCH bytes */
	rep
	cmpsw				|* loop until mismatch */
	jeq	maxmatch		|* match of length MAX_MATCH? */

mismatch:
	movb	-2(r5),r1		|* mismatch on first or second byte? */
	subb	-2(r4),r1		|* cl = 0 if first bytes equal */
	xchgl	r5,r0			|* r5 = scan+2, r0 = end of scan */
	subl	r5,r0			|* r0 = len */
	subl	r0,r4			|* r4 = cur_match + 2 + offset(window) */
	subl	r2,r4
	subl	#2,r4			|* r4 = cur_match */
	subb	#1,r1			|* set carry if cl == 0 (cannot use DEC) */
	adcl	#0,r0			|* r0 = carry ? len+1 : len */
	cmpl	r3,r0			|* len > best_len ? */
	jle	long_loop
	movl	r4,_match_start		|* match_start = cur_match */
	movl	r0,r3			|* r3 = best_len = len */
	cmpl	_nice_match,r0		|* len >= nice_match ? */
	jlt	long_loop

the_end:
	movl	r3,r0			|* result = r0 = best_len */
	popl	r3
	popl	r4
	popl	r5
	popl	fp
	rts

maxmatch:
	cmpsb
	jmp	mismatch
