;;*
;;***************************************************************
;;*								*
;;*			$GEN.gen/m68k.md			*
;;*								*
;;*	Machine description for GNU compiler			*
;;*	Motorola 68000 Version					*
;;*								*
;;*	Versão	3.0.0, de 09.01.94				*
;;*		3.0.0, de 09.01.94				*
;;*								*
;;*	Módulo: GCC						*
;;*		Compilador "C"					*
;;*		Categoria B					*
;;*								*
;;*	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
;;*		Copyright (c) 1993 UNITEC Informática Ltda	*
;;* 		Baseado em software homônimo do GNU		*
;;*								*
;;***************************************************************
;;*

;;- instruction definitions

;;- @@The original PO technology requires these to be ordered by speed,
;;- @@    so that assigner will pick the fastest.

;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.

;;- When naming insn's (operand 0 of define_insn) be careful about using
;;- names from other targets machine descriptions.

;;- cpp macro #define NOTICE_UPDATE_CC in file tm.h handles condition code
;;- updates for most instructions.

;;- Operand classes for the register allocator:
;;- 'a' one of the address registers can be used.
;;- 'd' one of the data registers can be used.
;;- 'f' one of the m68881 registers can be used
;;- 'r' either a data or an address register can be used.
;;- 'x' if one of the Sun FPA registers                    
;;- 'y' if one of the Low Sun FPA registers (fpa0-fpa15).

;;- Immediate Floating point operator constraints
;;- 'G' a floating point constant that is *NOT* one of the standard
;;   68881 constant values (to force calling output_move_const_double
;;   to get it from rom if it is a 68881 constant).
;;- 'H' one of the standard FPA constant values
;;
;;   See the functions standard_XXX_constant_p in output-m68k.c for more
;; info.

;;- Immediate integer operand constraints:
;;- 'I'  1 .. 8
;;- 'J'  -32768 .. 32767
;;- 'K'  -128 .. 127
;;- 'L'  -8 .. -1

;;- 		FPA port explanation:

;;-		Usage of the Sun FPA and the 68881 together

;;- The current port of gcc to the sun fpa disallows use of the m68881
;;- instructions completely if code is targetted for the fpa.  This is
;;- for the following reasons:

;;- 1) Expressing the preference hierarchy (ie. use the fpa if you
;;- can, the 68881 otherwise, and data registers only if you are
;;- forced to it) is a bitch with the current constraint scheme,
;;- especially since it would have to work for any combination of
;;- -mfpa, -m68881.

;;- 2) There are no instructions to move between the two types of
;;- registers; the stack must be used as an intermediary.

;;- It could indeed be done; I think the best way would be to have
;;- seperate patterns for TARGET_FPA (which implies a 68881),
;;- TARGET_68881, and no floating point co-processor.  Use
;;- define_expands for all of the named instruction patterns, and
;;- include code in the FPA instruction to deal with the 68881 with
;;- preferences specifically set to favor the fpa.  Some of this has
;;- already been done:
;;-
;;- 	1) Separation of most of the patterns out into a TARGET_FPA
;;- case and a TARGET_68881 case (the exceptions are the patterns
;;- which would need one define_expand and three define_insn's under
;;- it (with alot of duplicate code between them) to replace the
;;- current single define_insn.  These are mov{[ds]f,[ds]i} and the
;;- first two patterns in the md.
;;-
;;- Some would still have to be done:
;;-
;;-	1) Add code to the fpa patterns which correspond to 68881
;;- patterns to deal with the 68881 case (including preferences!).
;;- What you might actually do here is combine the fpa and 68881 code
;;- back together into one pattern for those instructions where it's
;;- absolutely necessary and save yourself some duplicate code.  I'm
;;- not completely sure as to whether you could get away with doing
;;- this only for the mov* insns, or if you'd have to do it for all
;;- named insns.
;;- 	2) Add code to the mov{[ds]f,[ds]i} instructions to handle
;;- moving between fpa regs and 68881 regs.

;;- Since the fpa is more powerful than the 68881 and also has more
;;- registers, and since I think the reultant md would be medium ugly
;;- (lot's of duplicate code, ugly constraint strings), I elected not
;;- to do this change.

;;- Another reason why someone *might* want to do the change is to
;;- control which register classes are accessed in a slightly cleaner
;;- way than I have.  See the blurb on CONDITIONAL_REGISTER_USAGE in
;;- the internals manual.

;;- Yet another reason why someone might want to do this change is to
;;- allow use of some of the 68881 insns which have no equivalent on
;;- the fpa.  The sqrt instruction comes fairly quickly to mind.

;;- If this is ever done, don't forget to change tm-sun3.h so that
;;- it *will* define __HAVE_68881__ when the FPA is in use.

;;-		Condition code hack

;;- When a floating point compare is done in the fpa, the resulting
;;- condition codes are left in the fpastatus register.  The values in
;;- this register must be moved into the 68000 cc register before any
;;- jump is executed.  Once this has been done, regular jump
;;- instructions are fine (ie. floating point jumps are not necessary.
;;- They are only done if the cc is in the 68881).

;;- The instructions that move the fpastatus register to the 68000
;;- register clobber a data register (the move cannot be done direct).
;;- These instructions might be bundled either with the compare
;;- instruction, or the branch instruction.  If we were using both the
;;- fpa and the 68881 together, we would wish to only mark the
;;- register clobbered if we were doing the compare in the fpa, but I
;;- think that that decision (whether to clobber the register or not)
;;- must be done before register allocation (makes sense) and hence we
;;- can't know if the floating point compare will be done in the fpa
;;- or the fp.  So whenever we are asked for code that uses the fpa,
;;- we will mark a data register as clobbered.  This is reasonable, as
;;- almost all floating point compare operations done with fpa code
;;- enabled will be done in the fpa.  It's even more reasonable since
;;- we decided to make the 68881 and the fpa mutually exclusive.

;;- We place to code to move the fpastatus register inside of a
;;- define_expand so that we can do it conditionally based on whether
;;- we are tagetting an fpa or not.

;;- This still leaves us with the question of where we wish to put the
;;- code to move the fpastatus reg.  If we put it in the compare
;;- instruction, we can restrict the clobbering of the register to
;;- floating point compares, but we can't take advantage of floating
;;- point subtracts & etc. that alter the fpastatus register.  If we
;;- put it in the branch instruction, all branches compiled with fpa
;;- code enabled will clobber a data register, but we will be able to
;;- take advantage of fpa subtracts.  This balance favors putting the
;;- code in with the compare instruction.

;;- Note that if some enterprising hacker should decide to switch
;;- this, he'll need to modify the code in NOTICE_UPDATE_CC.

;;-		Usage of the top 16 fpa registers

;;- The only locations which we may transfer fpa registers 16-31 from
;;- or to are the fpa registers 0-15.  (68000 registers and memory
;;- locations are impossible).  This causes problems in gcc, which
;;- assumes that mov?? instructions require no additional registers
;;- (see section 11.7) and since floating point moves *must* be
;;- supported into general registers (see section 12.3 under
;;- HARD_REGNO_OK_FOR_MODE_P) from anywhere.

;;- My solution was to reserve fpa0 for moves into or out of these top
;;- 16 registers and to disparage the choice to reload into or out of
;;- these registers as much as I could.  That alternative is always
;;- last in the list, so it will not be used unless all else fails.  I
;;- will note that according to my current information, sun's compiler
;;- doesn't use these top 16 registers at all.

;;- There is another possible way to do it.  I *believe* that if you
;;- make absolutely sure that the code will not be exectued in the
;;- reload pass, you can support the mov?? names with define_expands
;;- which require new registers.  This may be possible by the
;;- appropriate juggling of constraints.  I may come back to this later.

;;- 		Usage of constant RAM

;;- This has been handled correctly (I believe) but the way I've done
;;- it could use a little explanation.  The constant RAM can only be
;;- accessed when the instruction is in "command register" mode.
;;- "command register" mode means that no accessing of memory or the
;;- 68000 registers is being done.  This can be expressed easily in
;;- constraints, so generally the mode of the instruction is
;;- determined by a branch off of which_alternative.  In outputing
;;- instructions, a 'w' means to output an access to the constant ram
;;- (if the arg is CONST_DOUBLE and is one of the available
;;- constants), and 'x' means to output a register pair (if the arg is
;;- a 68000 register) and a 'y' is the combination of the above two
;;- processies.  You use a 'y' in two operand DF instructions where you
;;- *know* the other operand is an fpa register, you use an 'x' in DF
;;- instructions where the arg might be a 68000 register and the
;;- instruction is *not* in "command register" mode, and you use a 'w'
;;- in two situations: 1) The instruction *is* in command register
;;- mode (and hence won't be accessing 68000 registers), or 2) The
;;- instruction is a two operand SF instruction where you know the
;;- other operand is an fpa register.

;;-		Optimization issues

;;- I actually think that I've included all of the fpa instructions
;;- that should be included.  Note that if someone is interested in
;;- doing serious floating point work on the sun fpa, I would advise
;;- the use of the "asm" instruction in gcc to allow you to use the
;;- sin, cos, and exponential functions on the fpa board.

;;- END FPA Explanation Section.


;;- Some of these insn's are composites of several m68000 op codes.
;;- The assembler (or final @@??) insures that the appropriate one is
;;- selected.

(define_insn ""
  [(set (match_operand:DF 0 "push_operand" "=m")
	(match_operand:DF 1 "general_operand" "ro<>fyF"))]
  ""
  "*
{
  if (FP_REG_P (operands[1]))
    return \"fmovd\\t%f1,%0\";
  if (FPA_REG_P (operands[1]))
    return \"fpmovd\\t%1,%x0\";
  return output_move_double (operands);
}")

(define_insn ""
  [(set (match_operand:DI 0 "push_operand" "=m")
	(match_operand:DI 1 "general_operand" "ro<>Fy"))]
  ""
  "*
{
  return output_move_double (operands);
}")

;; Put tstsi first among test insns so it matches a CONST_INT operand.

(define_insn "tstsi"
  [(set (cc0)
	(match_operand:SI 0 "general_operand" "rm"))]
  ""
  "*
{
  if (TARGET_68020 || ! ADDRESS_REG_P (operands[0]))
    return \"tstl\\t%0\";
  /* If you think that the 68020 does not support tstl a0,
     reread page B-167 of the 68020 manual more carefully.  */
  /* On an address reg, cmpw may replace cmpl.  */
  return \"cmpw\\t%#0,%0\";
}")

(define_insn "tsthi"
  [(set (cc0)
	(match_operand:HI 0 "general_operand" "rm"))]
  ""
  "*
{
  if (TARGET_68020 || ! ADDRESS_REG_P (operands[0]))
    return \"tstw\\t%0\";
  return \"cmpw\\t%#0,%0\";
}")

(define_insn "tstqi"
  [(set (cc0)
	(match_operand:QI 0 "general_operand" "dm"))]
  ""
  "tstb\\t%0")
  
(define_expand "tstsf"
  [(set (cc0)
	(match_operand:SF 0 "general_operand" ""))]
  "TARGET_68881 || TARGET_FPA"
  "
{
  if (TARGET_FPA)
    {
      emit_insn (gen_rtx (PARALLEL, VOIDmode,
		          gen_rtvec (2,
				     gen_rtx (SET, VOIDmode,
					      cc0_rtx, operands[0]),
				     gen_rtx (CLOBBER, VOIDmode,
					      gen_reg_rtx (SImode)))));
      DONE;
    }
}")

(define_insn ""
  [(set (cc0)
	(match_operand:SF 0 "general_operand" "xmdF"))
   (clobber (match_operand:SI 1 "general_operand" "d"))]
  "TARGET_FPA"
  "fptsts\\t%x0\;fpmove\\tfpastatus,%1\;movw\\t%1,cc")

(define_insn ""
  [(set (cc0)
	(match_operand:SF 0 "general_operand" "fdm"))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    return \"ftstx\\t%0\";
  return \"ftsts\\t%0\";
}")

(define_expand "tstdf"
  [(set (cc0)
	(match_operand:DF 0 "general_operand" ""))]
  "TARGET_68881 || TARGET_FPA"
  "
{
  if (TARGET_FPA)
    {
      emit_insn (gen_rtx (PARALLEL, VOIDmode,
			  gen_rtvec (2, gen_rtx (SET, VOIDmode,
						 cc0_rtx, operands[0]),
				     gen_rtx (CLOBBER, VOIDmode,
					      gen_reg_rtx (SImode)))));
      DONE;
    }
}")

(define_insn ""
  [(set (cc0)
	(match_operand:DF 0 "general_operand" "xrmF"))
   (clobber (match_operand:SI 1 "general_operand" "d"))]
  "TARGET_FPA"
  "fptstd\\t%x0\;fpmove\\tfpastatus,%1\;movw\\t%1,cc")

(define_insn ""
  [(set (cc0)
	(match_operand:DF 0 "general_operand" "fm"))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    return \"ftstx\\t%0\";
  return \"ftstd\\t%0\";
}")

;; compare instructions.

;; Put cmpsi first among compare insns so it matches two CONST_INT operands.

;; A composite of the cmp, cmpa, & cmpi m68000 op codes.
(define_insn "cmpsi"
  [(set (cc0)
	(compare (match_operand:SI 0 "general_operand" "rKs,mr,>")
		 (match_operand:SI 1 "general_operand" "mr,Ksr,>")))]
  ""
  "*
{
  if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
    return \"cmpml\\t%1,%0\";
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED;
      return \"cmpl\\t%d0,%d1\"; 
    }
  return \"cmpl\\t%d1,%d0\";
}")

(define_insn "cmphi"
  [(set (cc0)
	(compare (match_operand:HI 0 "general_operand" "rnm,d,n,m")
		 (match_operand:HI 1 "general_operand" "d,rnm,m,n")))]
  ""
  "*
{
  if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
    return \"cmpmw\\t%1,%0\";
  if ((REG_P (operands[1]) && !ADDRESS_REG_P (operands[1]))
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED;
      return \"cmpw\\t%d0,%d1\"; 
    }
  return \"cmpw\\t%d1,%d0\";
}")

(define_insn "cmpqi"
  [(set (cc0)
	(compare (match_operand:QI 0 "general_operand" "dn,md,>")
		 (match_operand:QI 1 "general_operand" "dm,nd,>")))]
  ""
  "*
{
  if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
    return \"cmpmb\\t%1,%0\";
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED;
      return \"cmpb\\t%d0,%d1\";
    }
  return \"cmpb\\t%d1,%d0\";
}")

(define_expand "cmpdf"
  [(set (cc0)
	(compare (match_operand:DF 0 "general_operand" "")
		 (match_operand:DF 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "
{
  if (TARGET_FPA)
    {
      rtx set = gen_rtx (SET, VOIDmode, cc0_rtx,
			 gen_rtx (COMPARE, VOIDmode, operands[0], operands[1]));
      emit_insn (gen_rtx (PARALLEL, VOIDmode,
		          gen_rtvec (2, set,
				     gen_rtx (CLOBBER, VOIDmode,
					      gen_reg_rtx (SImode)))));
      DONE;
    }
}")

(define_insn ""
  [(set (cc0)
	(compare (match_operand:DF 0 "general_operand" "x,y")
		 (match_operand:DF 1 "general_operand" "xH,rmF")))
   (clobber (match_operand:SI 2 "general_operand" "d,d"))]
  "TARGET_FPA"
  "fpcmpd\\t%y1,%0\;fpmove\\tfpastatus,%2\;movw\\t%2,cc")

(define_insn ""
  [(set (cc0)
	(compare (match_operand:DF 0 "general_operand" "f,mG")
		 (match_operand:DF 1 "general_operand" "fmG,f")))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (REG_P (operands[0]))
    {
      if (REG_P (operands[1]))
	return \"fcmpx\\t%1,%0\";
      else
        return \"fcmpd\\t%f1,%0\";
    }
  cc_status.flags |= CC_REVERSED;
  return \"fcmpd\\t%f0,%1\";
}")

(define_expand "cmpsf"
 [(set (cc0)
       (compare (match_operand:SF 0 "general_operand" "")
		(match_operand:SF 1 "general_operand" "")))]
 "TARGET_68881 || TARGET_FPA"
 "
{
  if (TARGET_FPA)
    {
      rtx set = gen_rtx (SET, VOIDmode, cc0_rtx,
			 gen_rtx (COMPARE, VOIDmode, operands[0], operands[1]));
      emit_insn (gen_rtx (PARALLEL, VOIDmode,
			  gen_rtvec (2, set,
				     gen_rtx (CLOBBER, VOIDmode,
					      gen_reg_rtx(SImode)))));
      DONE;
    }
}")

(define_insn ""
  [(set (cc0)
	(compare (match_operand:SF 0 "general_operand" "x,y")
		 (match_operand:SF 1 "general_operand" "xH,rmF")))
   (clobber (match_operand:SI 2 "general_operand" "d,d"))]
  "TARGET_FPA"
  "fpcmps\\t%w1,%x0\;fpmove\\tfpastatus,%2\;movw\\t%2,cc")

(define_insn ""
  [(set (cc0)
	(compare (match_operand:SF 0 "general_operand" "f,mdG")
		 (match_operand:SF 1 "general_operand" "fmdG,f")))]
  "TARGET_68881"
  "*
{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fcmpx\\t%1,%0\";
      else
        return \"fcmps\\t%f1,%0\";
    }
  cc_status.flags |= CC_REVERSED;
  return \"fcmps\\t%f0,%1\";
}")

;; Recognizers for btst instructions.

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:QI 0 "nonimmediate_operand" "do")
			    (const_int 1)
			    (minus:SI (const_int 7)
				      (match_operand:SI 1 "general_operand" "di"))))]
  ""
  "* { return output_btst (operands, operands[1], operands[0], insn, 7); }")

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:SI 0 "nonimmediate_operand" "d")
			    (const_int 1)
			    (minus:SI (const_int 31)
				      (match_operand:SI 1 "general_operand" "di"))))]
  ""
  "* { return output_btst (operands, operands[1], operands[0], insn, 31); }")

;; The following two patterns are like the previous two
;; except that they use the fact that bit-number operands
;; are automatically masked to 3 or 5 bits.

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:QI 0 "nonimmediate_operand" "do")
			    (const_int 1)
			    (minus:SI (const_int 7)
				      (and:SI
				       (match_operand:SI 1 "general_operand" "d")
				       (const_int 7)))))]
  ""
  "* { return output_btst (operands, operands[1], operands[0], insn, 7); }")

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:SI 0 "nonimmediate_operand" "d")
			    (const_int 1)
			    (minus:SI (const_int 31)
				      (and:SI
				       (match_operand:SI 1 "general_operand" "d")
				       (const_int 31)))))]
  ""
  "* { return output_btst (operands, operands[1], operands[0], insn, 31); }")

;; Nonoffsettable mem refs are ok in this one pattern
;; since we don't try to adjust them.
(define_insn ""
  [(set (cc0) (zero_extract (match_operand:QI 0 "nonimmediate_operand" "md")
			    (const_int 1)
			    (match_operand:SI 1 "general_operand" "i")))]
  "GET_CODE (operands[1]) == CONST_INT
   && (unsigned) INTVAL (operands[1]) < 8"
  "*
{
  operands[1] = gen_rtx (CONST_INT, VOIDmode, 7 - INTVAL (operands[1]));
  return output_btst (operands, operands[1], operands[0], insn, 7);
}")

(define_insn ""
  ;; The constraint "o,d" here means that a nonoffsettable memref
  ;; will match the first alternative, and its address will be reloaded.
  ;; Copying the memory contents into a reg would be incorrect if the
  ;; bit position is over 7.
  [(set (cc0) (zero_extract (match_operand:HI 0 "nonimmediate_operand" "o,d")
			    (const_int 1)
			    (match_operand:SI 1 "general_operand" "i,i")))]
  "GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (GET_CODE (operands[0]) == MEM)
    {
      operands[0] = adj_offsettable_operand (operands[0],
					     INTVAL (operands[1]) / 8);
      operands[1] = gen_rtx (CONST_INT, VOIDmode, 
			     7 - INTVAL (operands[1]) % 8);
      return output_btst (operands, operands[1], operands[0], insn, 7);
    }
  operands[1] = gen_rtx (CONST_INT, VOIDmode,
			 15 - INTVAL (operands[1]));
  return output_btst (operands, operands[1], operands[0], insn, 15);
}")

(define_insn ""
  [(set (cc0) (zero_extract (match_operand:SI 0 "nonimmediate_operand" "do")
			    (const_int 1)
			    (match_operand:SI 1 "general_operand" "i")))]
  "GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (GET_CODE (operands[0]) == MEM)
    {
      operands[0] = adj_offsettable_operand (operands[0],
					     INTVAL (operands[1]) / 8);
      operands[1] = gen_rtx (CONST_INT, VOIDmode, 
			     7 - INTVAL (operands[1]) % 8);
      return output_btst (operands, operands[1], operands[0], insn, 7);
    }
  operands[1] = gen_rtx (CONST_INT, VOIDmode,
			 31 - INTVAL (operands[1]));
  return output_btst (operands, operands[1], operands[0], insn, 31);
}")

(define_insn ""
  [(set (cc0) (subreg:SI (lshiftrt:QI (match_operand:QI 0 "nonimmediate_operand" "dm")
				      (const_int 7))
			 0))]
  ""
  "*
{
  cc_status.flags = CC_Z_IN_NOT_N | CC_NOT_NEGATIVE;
  return \"tstb\\t%0\";
}")

(define_insn ""
  [(set (cc0) (and:SI (sign_extend:SI (sign_extend:HI (match_operand:QI 0 "nonimmediate_operand" "dm")))
		      (match_operand:SI 1 "general_operand" "i")))]
  "(GET_CODE (operands[1]) == CONST_INT
    && (unsigned) INTVAL (operands[1]) < 0x100
    && exact_log2 (INTVAL (operands[1])) >= 0)"
  "*
{ register int log = exact_log2 (INTVAL (operands[1]));
  operands[1] = gen_rtx (CONST_INT, VOIDmode, log);
  return output_btst (operands, operands[1], operands[0], insn, 7);
}")

;; move instructions

;; A special case in which it is not desirable
;; to reload the constant into a data register.
(define_insn ""
  [(set (match_operand:SI 0 "push_operand" "=m")
	(match_operand:SI 1 "general_operand" "J"))]
  "GET_CODE (operands[1]) == CONST_INT
   && INTVAL (operands[1]) >= -0x8000
   && INTVAL (operands[1]) < 0x8000"
  "*
{
  if (operands[1] == const0_rtx)
    return \"clrl\\t%0\";
  return \"pea\\t%a1\";
}")

;This is never used.
;(define_insn "swapsi"
;  [(set (match_operand:SI 0 "general_operand" "r")
;	(match_operand:SI 1 "general_operand" "r"))
;   (set (match_dup 1) (match_dup 0))]
;  ""
;  "exg\\t%1,%0")

;; Special case of fullword move when source is zero.
;; The reason this is special is to avoid loading a zero
;; into a data reg with moveq in order to store it elsewhere.
   
(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=g")
	(const_int 0))]
  ;; clr insns on 68000 read before writing.
  ;; This isn't so on the 68010, but we have no alternative for it.
  "(TARGET_68020
    || !(GET_CODE (operands[0]) == MEM && MEM_VOLATILE_P (operands[0])))"
  "*
{
  if (ADDRESS_REG_P (operands[0]))
    return \"subl\\t%0,%0\";
  /* moveq is faster on the 68000.  */
  if (DATA_REG_P (operands[0]) && !TARGET_68020)
    return \"moveq\\t%#0,%0\";
  return \"clrl\\t%0\";
}")

;; General case of fullword move.  The register constraints
;; force integer constants in range for a moveq to be reloaded
;; if they are headed for memory.
(define_insn "movsi"
  ;; Notes: make sure no alternative allows g vs g.
  ;; We don't allow f-regs since fixed point cannot go in them.
  ;; We do allow y and x regs since fixed point is allowed in them.
  [(set (match_operand:SI 0 "general_operand" "=g,da,y,!*x*r*m")
	(match_operand:SI 1 "general_operand" "daymKs,i,g,*x*r*m"))]
  ""
  "*
{
  if (which_alternative == 3)
    return \"fpmovl\\t%x1,fpa0\;fpmovl\\tfpa0,%x0\";	
  if (FPA_REG_P (operands[1]) || FPA_REG_P (operands[0]))
    return \"fpmovl\\t%x1,%x0\";
  if (GET_CODE (operands[1]) == CONST_INT)
    {
      if (operands[1] == const0_rtx
	  && (DATA_REG_P (operands[0])
	      || GET_CODE (operands[0]) == MEM)
	  /* clr insns on 68000 read before writing.
	     This isn't so on the 68010, but we have no alternative for it.  */
	  && (TARGET_68020
	      || !(GET_CODE (operands[0]) == MEM
		   && MEM_VOLATILE_P (operands[0]))))
	return \"clrl\\t%0\";
      else if (DATA_REG_P (operands[0])
	       && INTVAL (operands[1]) < 128
	       && INTVAL (operands[1]) >= -128)
        {
	  return \"moveq\\t%1,%0\";
	}
      else if (ADDRESS_REG_P (operands[0])
	       && INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
	return \"movw\\t%1,%0\";
      else if (push_operand (operands[0], SImode)
	       && INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
        return \"pea\\t%a1\";
    }
  else if ((GET_CODE (operands[1]) == SYMBOL_REF
	    || GET_CODE (operands[1]) == CONST)
	   && push_operand (operands[0], SImode))
    return \"pea\\t%a1\";
  else if ((GET_CODE (operands[1]) == SYMBOL_REF
	    || GET_CODE (operands[1]) == CONST)
	   && ADDRESS_REG_P (operands[0]))
    return \"lea\\t%a1,%0\";
  return \"movl\\t%1,%0\";
}")

(define_insn "movhi"
  [(set (match_operand:HI 0 "general_operand" "=g")
	(match_operand:HI 1 "general_operand" "g"))]
  ""
  "*
{
  if (GET_CODE (operands[1]) == CONST_INT)
    {
      if (operands[1] == const0_rtx
	  && (DATA_REG_P (operands[0])
	      || GET_CODE (operands[0]) == MEM)
	  /* clr insns on 68000 read before writing.
	     This isn't so on the 68010, but we have no alternative for it.  */
	  && (TARGET_68020
	      || !(GET_CODE (operands[0]) == MEM
		   && MEM_VOLATILE_P (operands[0]))))
	return \"clrw\\t%0\";
      else if (DATA_REG_P (operands[0])
	       && INTVAL (operands[1]) < 128
	       && INTVAL (operands[1]) >= -128)
        {
	  return \"moveq\\t%1,%0\";
	}
      else if (INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
	return \"movw\\t%1,%0\";
    }
  else if (CONSTANT_P (operands[1]))
    return \"movl\\t%1,%0\";

  /* Recognize the insn before a tablejump, one that refers
     to a table of offsets.  Such an insn will need to refer
     to a label on the insn.  So output one.  Use the label-number
     of the table of offsets to generate this label.  */
  if (GET_CODE (operands[1]) == MEM
      && GET_CODE (XEXP (operands[1], 0)) == PLUS
      && (GET_CODE (XEXP (XEXP (operands[1], 0), 0)) == LABEL_REF
	  || GET_CODE (XEXP (XEXP (operands[1], 0), 1)) == LABEL_REF)
      && GET_CODE (XEXP (XEXP (operands[1], 0), 0)) != PLUS
      && GET_CODE (XEXP (XEXP (operands[1], 0), 1)) != PLUS)
    {
      rtx labelref;
      if (GET_CODE (XEXP (XEXP (operands[1], 0), 0)) == LABEL_REF)
	labelref = XEXP (XEXP (operands[1], 0), 0);
      else
	labelref = XEXP (XEXP (operands[1], 0), 1);
      ASM_OUTPUT_INTERNAL_LABEL (asm_out_file, \"LI\",
				 CODE_LABEL_NUMBER (XEXP (labelref, 0)));
      /* For sake of 3b1, set flag saying we need to define the symbol
         LD%n (with value L%n-LI%n) at the end of the switch table.  */
      RTX_INTEGRATED_P (next_real_insn (XEXP (labelref, 0))) = 1;
    }

  return \"movw\\t%1,%0\";
}")

(define_insn "movstricthi"
  [(set (strict_low_part (match_operand:HI 0 "general_operand" "+dm"))
	(match_operand:HI 1 "general_operand" "rmn"))]
  ""
  "*
{
  if (GET_CODE (operands[1]) == CONST_INT)
    {
      if (operands[1] == const0_rtx
	  && (DATA_REG_P (operands[0])
	      || GET_CODE (operands[0]) == MEM)
	  /* clr insns on 68000 read before writing.
	     This isn't so on the 68010, but we have no alternative for it.  */
	  && (TARGET_68020
	      || !(GET_CODE (operands[0]) == MEM
		   && MEM_VOLATILE_P (operands[0]))))
	return \"clrw\\t%0\";
    }
  return \"movw\\t%1,%0\";
}")

(define_insn "movqi"
  [(set (match_operand:QI 0 "general_operand" "=d,*a,m,m,?*a")
	(match_operand:QI 1 "general_operand" "dmi*a,d*a,dmi,?*a,m"))]
  ""
  "*
{
  rtx xoperands[4];
  if (ADDRESS_REG_P (operands[0]) && GET_CODE (operands[1]) == MEM)
    {
      xoperands[1] = operands[1];
      xoperands[2]
        = gen_rtx (MEM, QImode,
		   gen_rtx (PLUS, VOIDmode, stack_pointer_rtx, const1_rtx));
      xoperands[3] = stack_pointer_rtx;
      /* Just pushing a byte puts it in the high byte of the halfword.  */
      /* We must put it in the low half, the second byte.  */
      output_asm_insn (\"subqw\\t%#2,%3\;movb	%1,%2\", xoperands);
      return \"movw\\t%+,%0\";
    }
  if (ADDRESS_REG_P (operands[1]) && GET_CODE (operands[0]) == MEM)
    {
      xoperands[0] = operands[0];
      xoperands[1] = operands[1];
      xoperands[2]
        = gen_rtx (MEM, QImode,
		   gen_rtx (PLUS, VOIDmode, stack_pointer_rtx, const1_rtx));
      xoperands[3] = stack_pointer_rtx;
      output_asm_insn (\"movw\\t%1,%-\;movb	%2,%0\;addqw\\t%#2,%3\", xoperands);
      return \"\";
    }
  /* clr and st insns on 68000 read before writing.
     This isn't so on the 68010, but we have no alternative for it.  */
  if (TARGET_68020
      || !(GET_CODE (operands[0]) == MEM && MEM_VOLATILE_P (operands[0])))
    {
      if (operands[1] == const0_rtx)
	return \"clrb\\t%0\";
      if (GET_CODE (operands[1]) == CONST_INT
	  && INTVAL (operands[1]) == -1)
	{
	  CC_STATUS_INIT;
	  return \"st\\t%0\";
	}
    }
  if (GET_CODE (operands[1]) != CONST_INT && CONSTANT_P (operands[1]))
    return \"movl\\t%1,%0\";
  if (ADDRESS_REG_P (operands[0]) || ADDRESS_REG_P (operands[1]))
    return \"movw\\t%1,%0\";
  return \"movb\\t%1,%0\";
}")

(define_insn "movstrictqi"
  [(set (strict_low_part (match_operand:QI 0 "general_operand" "+dm"))
	(match_operand:QI 1 "general_operand" "dmn"))]
  ""
  "*
{
  if (operands[1] == const0_rtx
      /* clr insns on 68000 read before writing.
         This isn't so on the 68010, but we have no alternative for it.  */
      && (TARGET_68020
          || !(GET_CODE (operands[0]) == MEM && MEM_VOLATILE_P (operands[0]))))
    return \"clrb\\t%0\";
  return \"movb\\t%1,%0\";
}")

(define_insn "movsf"
  [(set (match_operand:SF 0 "general_operand" "=rmf,x,y,rm,!x,!rm")
	(match_operand:SF 1 "general_operand" "rmfF,xH,rmF,y,rm,x"))]
;  [(set (match_operand:SF 0 "general_operand" "=rmf")
;	(match_operand:SF 1 "general_operand" "rmfF"))]
  ""
  "*
{
  if (which_alternative >= 4)
    return \"fpmovs %1,fpa0\;fpmovs fpa0,%0\";
  if (FPA_REG_P (operands[0]))
    {
      if (FPA_REG_P (operands[1]))
	return \"fpmovs\\t%x1,%x0\";
      else if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_single (operands);
      else if (FP_REG_P (operands[1]))
        return \"fmovs\\t%1,-(sp)\;fpmovd\\t(sp)+,%0\";
      return \"fpmovs\\t%x1,%x0\";
    }
  if (FPA_REG_P (operands[1]))
    {
      if (FP_REG_P (operands[0]))
	return \"fpmovs\\t%x1,-(sp)\;fmovs\\t(sp)+,%0\";
      else
	return \"fpmovs\\t%x1,%x0\";
    }
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fmovx\\t%1,%0\";
      else if (ADDRESS_REG_P (operands[1]))
	return \"movl\\t%1,%-\;fmovs\\t%+,%0\";
      else if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_single (operands);
      return \"fmovs\\t%f1,%0\";
    }
  if (FP_REG_P (operands[1]))
    {
      if (ADDRESS_REG_P (operands[0]))
	return \"fmovs\\t%1,%-\;movl\\t%+,%0\";
      return \"fmovs %f1,%0\";
    }
  return \"movl\\t%1,%0\";
}")

(define_insn "movdf"
  [(set (match_operand:DF 0 "general_operand" "=rm,&rf,&rof<>,y,rm,x,!x,!rm")
	(match_operand:DF 1 "general_operand" "rf,m,rofF<>,rmF,y,xH,rm,x"))]
;  [(set (match_operand:DF 0 "general_operand" "=rm,&rf,&rof<>")
;	(match_operand:DF 1 "general_operand" "rf,m,rofF<>"))]
  ""
  "*
{
  if (which_alternative == 6)
    return \"fpmovd\\t%x1,fpa0\;fpmovd\\tfpa0,%x0\";
  if (FPA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      if (FP_REG_P (operands[1]))
        return \"fmovd\\t%1,-(sp)\;fpmovd\\t(sp)+,%x0\";
      return \"fpmovd\\t%x1,%x0\";
    }
  else if (FPA_REG_P (operands[1]))
    {
      if (FP_REG_P(operands[0]))
        return \"fpmovd\\t%x1,-(sp)\;fmoved\\t(sp)+,%0\";
      else
        return \"fpmovd\\t%x1,%x0\";
    }
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fmovx\\t%1,%0\";
      if (REG_P (operands[1]))
	{
	  rtx xoperands[2];
	  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
	  output_asm_insn (\"movl\\t%1,%-\", xoperands);
	  output_asm_insn (\"movl\\t%1,%-\", operands);
	  return \"fmovd\\t%+,%0\";
	}
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      return \"fmovd\\t%f1,%0\";
    }
  else if (FP_REG_P (operands[1]))
    {
      if (REG_P (operands[0]))
	{
	  output_asm_insn (\"fmovd\\t%f1,%-\;movl\\t%+,%0\", operands);
	  operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
	  return \"movl\\t%+,%0\";
	}
      else
        return \"fmovd\\t%f1,%0\";
    }
  return output_move_double (operands);
}
")

;; movdi can apply to fp regs in some cases
(define_insn "movdi"
  ;; Let's see if it really still needs to handle fp regs, and, if so, why.
  [(set (match_operand:DI 0 "general_operand" "=rm,&r,&ro<>,y,rm,!*x,!rm")
	(match_operand:DI 1 "general_operand" "rF,m,roi<>F,rmiF,y,rmF,*x"))]
;  [(set (match_operand:DI 0 "general_operand" "=rm,&r,&ro<>,!&rm,!&f,y,rm,x,!x,!rm")
;	(match_operand:DI 1 "general_operand" "r,m,roi<>,fF,rfmF,rmi,y,rm,x"))]
;  [(set (match_operand:DI 0 "general_operand" "=rm,&rf,&ro<>,!&rm,!&f")
;	(match_operand:DI 1 "general_operand" "r,m,roi<>,fF,rfF"))]
  ""
  "*
{
  if (which_alternative == 8)
    return \"fpmovd\\t%x1,fpa0\;fpmovd\\tfpa0,%x0\";
  if (FPA_REG_P (operands[0]) || FPA_REG_P (operands[1]))
    return \"fpmovd\\t%x1,%x0\";
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return \"fmovx\\t%1,%0\";
      if (REG_P (operands[1]))
	{
	  rtx xoperands[2];
	  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
	  output_asm_insn (\"movl\\t%1,%-\", xoperands);
	  output_asm_insn (\"movl\\t%1,%-\", operands);
	  return \"fmovd\\t%+,%0\";
	}
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      return \"fmovd\\t%f1,%0\";
    }
  else if (FP_REG_P (operands[1]))
    {
      if (REG_P (operands[0]))
	{
	  output_asm_insn (\"fmovd\\t%f1,%-\;movl\\t%+,%0\", operands);
	  operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
	  return \"movl\\t%+,%0\";
	}
      else
        return \"fmovd\\t%f1,%0\";
    }
  return output_move_double (operands);
}
")

;; Thus goes after the move instructions
;; because the move instructions are better (require no spilling)
;; when they can apply.  It goes before the add/sub insns
;; so we will prefer it to them.

(define_insn "pushasi"
  [(set (match_operand:SI 0 "push_operand" "=m")
	(match_operand:SI 1 "address_operand" "p"))]
  ""
  "pea\\t%a1")

;; truncation instructions
(define_insn "truncsiqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm,d")
	(truncate:QI
	 (match_operand:SI 1 "general_operand" "doJ,i")))]
  ""
  "*
{
  if (GET_CODE (operands[0]) == REG)
    {
      /* Must clear condition codes, since the mov.l bases them on
	 the entire 32 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return \"movl\\t%1,%0\";
    }
  if (GET_CODE (operands[1]) == MEM)
    operands[1] = adj_offsettable_operand (operands[1], 3);
  return \"movb\\t%1,%0\";
}")

(define_insn "trunchiqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm,d")
	(truncate:QI
	 (match_operand:HI 1 "general_operand" "doJ,i")))]
  ""
  "*
{
  if (GET_CODE (operands[0]) == REG
      && (GET_CODE (operands[1]) == MEM
	  || GET_CODE (operands[1]) == CONST_INT))
    {
      /* Must clear condition codes, since the mov.w bases them on
	 the entire 16 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return \"movw\\t%1,%0\";
    }
  if (GET_CODE (operands[0]) == REG)
    {
      /* Must clear condition codes, since the mov.l bases them on
	 the entire 32 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return \"movl\\t%1,%0\";
    }
  if (GET_CODE (operands[1]) == MEM)
    operands[1] = adj_offsettable_operand (operands[1], 1);
  return \"movb\\t%1,%0\";
}")

(define_insn "truncsihi2"
  [(set (match_operand:HI 0 "general_operand" "=dm,d")
	(truncate:HI
	 (match_operand:SI 1 "general_operand" "roJ,i")))]
  ""
  "*
{
  if (GET_CODE (operands[0]) == REG)
    {
      /* Must clear condition codes, since the mov.l bases them on
	 the entire 32 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return \"movl\\t%1,%0\";
    }
  if (GET_CODE (operands[1]) == MEM)
    operands[1] = adj_offsettable_operand (operands[1], 2);
  return \"movw\\t%1,%0\";
}")

;; zero extension instructions

(define_expand "zero_extendhisi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(const_int 0))
   (set (strict_low_part (subreg:HI (match_dup 0) 0))
	(match_operand:HI 1 "general_operand" ""))]
  ""
  "operands[1] = make_safe_from (operands[1], operands[0]);")

(define_expand "zero_extendqihi2"
  [(set (match_operand:HI 0 "register_operand" "")
	(const_int 0))
   (set (strict_low_part (subreg:QI (match_dup 0) 0))
	(match_operand:QI 1 "general_operand" ""))]
  ""
  "operands[1] = make_safe_from (operands[1], operands[0]);")

(define_expand "zero_extendqisi2"
  [(set (match_operand:SI 0 "register_operand" "")
	(const_int 0))
   (set (strict_low_part (subreg:QI (match_dup 0) 0))
	(match_operand:QI 1 "general_operand" ""))]
  ""
  " operands[1] = make_safe_from (operands[1], operands[0]); ")

;; Patterns to recognize zero-extend insns produced by the combiner.

;; Note that the one starting from HImode comes before those for QImode
;; so that a constant operand will match HImode, not QImode.
(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=do<>")
	(zero_extend:SI
	 (match_operand:HI 1 "general_operand" "rmn")))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return \"andl\\t%#0xFFFF,%0\";
      if (reg_mentioned_p (operands[0], operands[1]))
        return \"movw\\t%1,%0\;andl\\t%#0xFFFF,%0\";
      return \"clrl\\t%0\;movw\\t%1,%0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    return \"movw\\t%1,%0\;clrw\\t%0\";
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    return \"clrw\\t%0\;movw\\t%1,%0\";
  else
    {
      output_asm_insn (\"clrw\\t%0\", operands);
      operands[0] = adj_offsettable_operand (operands[0], 2);
      return \"movw\\t%1,%0\";
    }
}")

(define_insn ""
  [(set (match_operand:HI 0 "general_operand" "=do<>")
	(zero_extend:HI
	 (match_operand:QI 1 "general_operand" "dmn")))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return \"andw\\t%#0xFF,%0\";
      if (reg_mentioned_p (operands[0], operands[1]))
        return \"movb\\t%1,%0\;andw\\t%#0xFF,%0\";
      return \"clrw\\t%0\;movb\\t%1,%0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    {
      if (REGNO (XEXP (XEXP (operands[0], 0), 0)) == STACK_POINTER_REGNUM)
	{
	  output_asm_insn (\"clrw\\t%-\", operands);
	  operands[0] = gen_rtx (MEM, GET_MODE (operands[0]),
				 plus_constant (stack_pointer_rtx, 1));
	  return \"movb\\t%1,%0\";
	}
      else
	return \"movb\\t%1,%0\;clrb\\t%0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    return \"clrb\\t%0\;movb\\t%1,%0\";
  else
    {
      output_asm_insn (\"clrb\\t%0\", operands);
      operands[0] = adj_offsettable_operand (operands[0], 1);
      return \"movb\\t%1,%0\";
    }
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=do<>")
	(zero_extend:SI
	 (match_operand:QI 1 "general_operand" "dmn")))]
  ""
  "*
{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return \"andl\\t%#0xFF,%0\";
      if (reg_mentioned_p (operands[0], operands[1]))
        return \"movb\\t%1,%0\;andl\\t%#0xFF,%0\";
      return \"clrl\\t%0\;movb\\t%1,%0\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    {
      operands[0] = XEXP (XEXP (operands[0], 0), 0);
      return \"clrl\\t-(%0)\;moveb\\t%1,3(%0)\";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    {
      operands[0] = XEXP (XEXP (operands[0], 0), 0);
      return \"clrl\\t(%0)+\;moveb\\t%1,-1(%0)\";
    }
  else
    {
      output_asm_insn (\"clrl\\t%0\", operands);
      operands[0] = adj_offsettable_operand (operands[0], 3);
      return \"movb\\t%1,%0\";
    }
}")

;; sign extension instructions
;; Note that the one starting from HImode comes before those for QImode
;; so that a constant operand will match HImode, not QImode.

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "general_operand" "=*d,a")
	(sign_extend:SI
	 (match_operand:HI 1 "general_operand" "0,rmn")))]
  ""
  "*
{
  if (ADDRESS_REG_P (operands[0]))
    return \"movw\\t%1,%0\";
  return \"extl\\t%0\";
}")

(define_insn "extendqihi2"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(sign_extend:HI
	 (match_operand:QI 1 "general_operand" "0")))]
  ""
  "extw\\t%0")

(define_insn "extendqisi2"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(sign_extend:SI
	 (match_operand:QI 1 "general_operand" "0")))]
  "TARGET_68020"
  "extbl\\t%0")

;; Conversions between float and double.

(define_expand "extendsfdf2"
  [(set (match_operand:DF 0 "general_operand" "")
	(float_extend:DF
	 (match_operand:SF 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=x,y")
	(float_extend:DF
	 (match_operand:SF 1 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "fpstod\\t%w1,%0")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=*fdm,f")
	(float_extend:DF
	  (match_operand:SF 1 "general_operand" "f,dmF")))]
  "TARGET_68881"
  "*
{
  if (FP_REG_P (operands[0]) && FP_REG_P (operands[1]))
    {
      if (REGNO (operands[0]) == REGNO (operands[1]))
	{
	  /* Extending float to double in an fp-reg is a no-op.
	     NOTICE_UPDATE_CC has already assumed that the
	     cc will be set.  So cancel what it did.  */
	  cc_status = cc_prev_status;
	  return \"\";
	}
      return \"fmovx\\t%1,%0\";
    }
  if (FP_REG_P (operands[0]))
    return \"fmovs\\t%f1,%0\";
  if (DATA_REG_P (operands[0]) && FP_REG_P (operands[1]))
    {
      output_asm_insn (\"fmovd\\t%f1,%-\;movl\\t%+,%0\", operands);
      operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
      return \"movl\\t%+,%0\";
    }
  return \"fmovd\\t%f1,%0\";
}")

;; This cannot output into an f-reg because there is no way to be
;; sure of truncating in that case.
;; But on the Sun FPA, we can be sure.
(define_expand "truncdfsf2"
  [(set (match_operand:SF 0 "general_operand" "")
	(float_truncate:SF
	  (match_operand:DF 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=x,y")
	(float_truncate:SF
	  (match_operand:DF 1 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "fpdtos\\t%y1,%0")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=dm")
	(float_truncate:SF
	  (match_operand:DF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovs\\t%f1,%0")

;; Conversion between fixed point and floating point.
;; Note that among the fix-to-float insns
;; the ones that start with SImode come first.
;; That is so that an operand that is a CONST_INT
;; (and therefore lacks a specific machine mode).
;; will be recognized as SImode (which is always valid)
;; rather than as QImode or HImode.

(define_expand "floatsisf2"
  [(set (match_operand:SF 0 "general_operand" "")
	(float:SF (match_operand:SI 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=y,x")
	(float:SF (match_operand:SI 1 "general_operand" "rmi,x")))]
  "TARGET_FPA"
  "fpltos\\t%1,%0")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=f")
	(float:SF (match_operand:SI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmovl\\t%1,%0")

(define_expand "floatsidf2"
  [(set (match_operand:DF 0 "general_operand" "")
	(float:DF (match_operand:SI 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=y,x")
	(float:DF (match_operand:SI 1 "general_operand" "rmi,x")))]
  "TARGET_FPA"
  "fpltod\\t%1,%0")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=f")
	(float:DF (match_operand:SI 1 "general_operand" "dmi")))]
  "TARGET_68881"
  "fmovl\\t%1,%0")

(define_insn "floathisf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(float:SF (match_operand:HI 1 "general_operand" "dmn")))]
  "TARGET_68881"
  "fmovw\\t%1,%0")

(define_insn "floathidf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(float:DF (match_operand:HI 1 "general_operand" "dmn")))]
  "TARGET_68881"
  "fmovw\\t%1,%0")

(define_insn "floatqisf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(float:SF (match_operand:QI 1 "general_operand" "dmn")))]
  "TARGET_68881"
  "fmovb\\t%1,%0")

(define_insn "floatqidf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(float:DF (match_operand:QI 1 "general_operand" "dmn")))]
  "TARGET_68881"
  "fmovb\\t%1,%0")

;; Convert a float to a float whose value is an integer.
;; This is the first stage of converting it to an integer type.

(define_insn "ftruncdf2"
  [(set (match_operand:DF 0 "general_operand" "=f")
	(fix:DF (match_operand:DF 1 "general_operand" "fFm")))]
  "TARGET_68881"
  "*
{
  if (FP_REG_P (operands[1]))
    return \"fintrzx\\t%f1,%0\";
  return \"fintrzd\\t%f1,%0\";
}")

(define_insn "ftruncsf2"
  [(set (match_operand:SF 0 "general_operand" "=f")
	(fix:SF (match_operand:SF 1 "general_operand" "dfFm")))]
  "TARGET_68881"
  "*
{
  if (FP_REG_P (operands[1]))
    return \"fintrzx\\t%f1,%0\";
  return \"fintrzs\\t%f1,%0\";
}")

;; Convert a float whose value is an integer
;; to an actual integer.  Second stage of converting float to integer type.
(define_insn "fixsfqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(fix:QI (match_operand:SF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovb\\t%1,%0")

(define_insn "fixsfhi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(fix:HI (match_operand:SF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovw\\t%1,%0")

(define_insn "fixsfsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(fix:SI (match_operand:SF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovl\\t%1,%0")

(define_insn "fixdfqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(fix:QI (match_operand:DF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovb\\t%1,%0")

(define_insn "fixdfhi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(fix:HI (match_operand:DF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovw\\t%1,%0")

(define_insn "fixdfsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(fix:SI (match_operand:DF 1 "general_operand" "f")))]
  "TARGET_68881"
  "fmovl\\t%1,%0")

;; Convert a float to an integer.
;; On the Sun FPA, this is done in one step.

(define_insn "fix_truncsfsi2"
  [(set (match_operand:SI 0 "general_operand" "=x,y")
	(fix:SI (fix:SF (match_operand:SF 1 "general_operand" "xH,rmF"))))]
  "TARGET_FPA"
  "fpstol\\t%w1,%0")

(define_insn "fix_truncdfsi2"
  [(set (match_operand:SI 0 "general_operand" "=x,y")
	(fix:SI (fix:DF (match_operand:DF 1 "general_operand" "xH,rmF"))))]
  "TARGET_FPA"
  "fpdtol\\t%y1,%0")

;; add instructions

;; Note that the last two alternatives are near-duplicates
;; in order to handle insns generated by reload.
;; This is needed since they are not themselves reloaded,
;; so commutativity won't apply to them.
(define_insn "addsi3"
  [(set (match_operand:SI 0 "general_operand" "=m,r,!a,!a")
	(plus:SI (match_operand:SI 1 "general_operand" "%0,0,a,rJK")
		 (match_operand:SI 2 "general_operand" "dIKLs,mrIKLs,rJK,a")))]
  ""
  "*
{
  if (! operands_match_p (operands[0], operands[1]))
    {
      if (!ADDRESS_REG_P (operands[1]))
	{
	  rtx tmp = operands[1];

	  operands[1] = operands[2];
	  operands[2] = tmp;
	}

      /* These insns can result from reloads to access
	 stack slots over 64k from the frame pointer.  */
      if (GET_CODE (operands[2]) == CONST_INT
	  && INTVAL (operands[2]) + 0x8000 >= (unsigned) 0x10000)
        return \"movl\\t%2,%0\;addl\\t%1,%0\";
      if (GET_CODE (operands[2]) == REG)
	return \"lea\\t0(%1,%2),%0\";
      else
	return \"lea\\t%c2(%1),%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
#ifndef NO_ADDSUB_Q
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return (ADDRESS_REG_P (operands[0])
		? \"addqw\\t%2,%0\"
		: \"addql\\t%2,%0\");
      if (INTVAL (operands[2]) < 0
	  && INTVAL (operands[2]) >= -8)
        {
	  operands[2] = gen_rtx (CONST_INT, VOIDmode,
			         - INTVAL (operands[2]));
	  return (ADDRESS_REG_P (operands[0])
		  ? \"subqw\\t%2,%0\"
		  : \"subql\\t%2,%0\");
	}
#endif
      if (ADDRESS_REG_P (operands[0])
	  && INTVAL (operands[2]) >= -0x8000
	  && INTVAL (operands[2]) < 0x8000)
	return \"addw\\t%2,%0\";
    }
  return \"addl\\t%2,%0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=a")
	(plus:SI (match_operand:SI 1 "general_operand" "0")
		 (sign_extend:SI (match_operand:HI 2 "general_operand" "rmn"))))]
  ""
  "addw\\t%2,%0")

(define_insn "addhi3"
  [(set (match_operand:HI 0 "general_operand" "=m,r")
	(plus:HI (match_operand:HI 1 "general_operand" "%0,0")
		 (match_operand:HI 2 "general_operand" "dn,rmn")))]
  ""
  "*
{
#ifndef NO_ADDSUB_Q
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return \"addqw\\t%2,%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) < 0
	  && INTVAL (operands[2]) >= -8)
	{
	  operands[2] = gen_rtx (CONST_INT, VOIDmode,
			         - INTVAL (operands[2]));
	  return \"subqw\\t%2,%0\";
	}
    }
#endif
  return \"addw\\t%2,%0\";
}")

(define_insn ""
  [(set (strict_low_part (match_operand:HI 0 "general_operand" "+m,d"))
	(plus:HI (match_dup 0)
		 (match_operand:HI 1 "general_operand" "dn,rmn")))]
  ""
  "addw\\t%1,%0")

(define_insn "addqi3"
  [(set (match_operand:QI 0 "general_operand" "=m,d")
	(plus:QI (match_operand:QI 1 "general_operand" "%0,0")
		 (match_operand:QI 2 "general_operand" "dn,dmn")))]
  ""
  "*
{
#ifndef NO_ADDSUB_Q
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return \"addqb\\t%2,%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) < 0 && INTVAL (operands[2]) >= -8)
       {
	 operands[2] = gen_rtx (CONST_INT, VOIDmode, - INTVAL (operands[2]));
	 return \"subqb\\t%2,%0\";
       }
    }
#endif
  return \"addb\\t%2,%0\";
}")

(define_insn ""
  [(set (strict_low_part (match_operand:QI 0 "general_operand" "+m,d"))
	(plus:QI (match_dup 0)
		 (match_operand:QI 1 "general_operand" "dn,dmn")))]
  ""
  "addb\\t%1,%0")

(define_expand "adddf3"
  [(set (match_operand:DF 0 "general_operand" "")
	(plus:DF (match_operand:DF 1 "general_operand" "")
		 (match_operand:DF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=x,y")
	(plus:DF (match_operand:DF 1 "general_operand" "%xH,y")
		 (match_operand:DF 2 "general_operand" "xH,dmF")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpaddd\\t%y2,%0\";
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fpaddd\\t%y1,%0\";
  if (which_alternative == 0)
    return \"fpadd3d\\t%w2,%w1,%0\";
  return \"fpadd3d\\t%x2,%x1,%0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=f")
	(plus:DF (match_operand:DF 1 "general_operand" "%0")
		 (match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"faddx\\t%2,%0\";
  return \"faddd\\t%f2,%0\";
}")

(define_expand "addsf3"
  [(set (match_operand:SF 0 "general_operand" "")
	(plus:SF (match_operand:SF 1 "general_operand" "")
		 (match_operand:SF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=x,y")
	(plus:SF (match_operand:SF 1 "general_operand" "%xH,y")
		 (match_operand:SF 2 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpadds\\t%w2,%0\";
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fpadds\\t%w1,%0\";
  if (which_alternative == 0)
    return \"fpadd3s\\t%w2,%w1,%0\";
  return \"fpadd3s\\t%2,%1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=f")
	(plus:SF (match_operand:SF 1 "general_operand" "%0")
		 (match_operand:SF 2 "general_operand" "fdmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"faddx\\t%2,%0\";
  return \"fadds\\t%f2,%0\";
}")

;; subtract instructions

(define_insn "subsi3"
  [(set (match_operand:SI 0 "general_operand" "=m,r,!a,?d")
	(minus:SI (match_operand:SI 1 "general_operand" "0,0,a,mrIKs")
		  (match_operand:SI 2 "general_operand" "dIKs,mrIKs,J,0")))]
  ""
  "*
{
  if (! operands_match_p (operands[0], operands[1]))
    {
      if (operands_match_p (operands[0], operands[2]))
	{
#ifndef NO_ADDSUB_Q
	  if (GET_CODE (operands[1]) == CONST_INT)
	    {
	      if (INTVAL (operands[1]) > 0
		  && INTVAL (operands[1]) <= 8)
		return \"subql\\t%1,%0\;negl\\t%0\";
	    }
#endif
	  return \"subl\\t%1,%0\;negl\\t%0\";
	}
      /* This case is matched by J, but negating -0x8000
         in an lea would give an invalid displacement.
	 So do this specially.  */
      if (INTVAL (operands[2]) == -0x8000)
	return \"movl\\t%1,%0\;subl\\t%2,%0\";
      return \"lea\\t%n2(%1),%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
#ifndef NO_ADDSUB_Q
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return \"subql\\t%2,%0\";
#endif
      if (ADDRESS_REG_P (operands[0])
	  && INTVAL (operands[2]) >= -0x8000
	  && INTVAL (operands[2]) < 0x8000)
	return \"subw\\t%2,%0\";
    }
  return \"subl\\t%2,%0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=a")
	(minus:SI (match_operand:SI 1 "general_operand" "0")
		  (sign_extend:SI (match_operand:HI 2 "general_operand" "rmn"))))]
  ""
  "subw\\t%2,%0")

(define_insn "subhi3"
  [(set (match_operand:HI 0 "general_operand" "=m,r")
	(minus:HI (match_operand:HI 1 "general_operand" "0,0")
		  (match_operand:HI 2 "general_operand" "dn,rmn")))]
  ""
  "subw\\t%2,%0")

(define_insn ""
  [(set (strict_low_part (match_operand:HI 0 "general_operand" "+m,d"))
	(minus:HI (match_dup 0)
		  (match_operand:HI 1 "general_operand" "dn,rmn")))]
  ""
  "subw\\t%1,%0")

(define_insn "subqi3"
  [(set (match_operand:QI 0 "general_operand" "=m,d")
	(minus:QI (match_operand:QI 1 "general_operand" "0,0")
		  (match_operand:QI 2 "general_operand" "dn,dmn")))]
  ""
  "subb\\t%2,%0")

(define_insn ""
  [(set (strict_low_part (match_operand:QI 0 "general_operand" "+m,d"))
	(minus:QI (match_dup 0)
		  (match_operand:QI 1 "general_operand" "dn,dmn")))]
  ""
  "subb\\t%1,%0")

(define_expand "subdf3"
  [(set (match_operand:DF 0 "general_operand" "")
	(minus:DF (match_operand:DF 1 "general_operand" "")
		  (match_operand:DF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=x,y,y")
	(minus:DF (match_operand:DF 1 "general_operand" "xH,y,dmF")
		  (match_operand:DF 2 "general_operand" "xH,dmF,0")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fprsubd\\t%y1,%0\";
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpsubd\\t%y2,%0\";
  if (which_alternative == 0)
    return \"fpsub3d\\t%w2,%w1,%0\";
  return \"fpsub3d\\t%x2,%x1,%0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=f")
	(minus:DF (match_operand:DF 1 "general_operand" "0")
		  (match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"fsubx\\t%2,%0\";
  return \"fsubd\\t%f2,%0\";
}")

(define_expand "subsf3"
  [(set (match_operand:SF 0 "general_operand" "")
	(minus:SF (match_operand:SF 1 "general_operand" "")
		  (match_operand:SF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=x,y,y")
	(minus:SF (match_operand:SF 1 "general_operand" "xH,y,rmF")
		  (match_operand:SF 2 "general_operand" "xH,rmF,0")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fprsubs\\t%w1,%0\";
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpsubs\\t%w2,%0\";
  if (which_alternative == 0)
    return \"fpsub3s\\t%w2,%w1,%0\";
  return \"fpsub3s\\t%2,%1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=f")
	(minus:SF (match_operand:SF 1 "general_operand" "0")
		  (match_operand:SF 2 "general_operand" "fdmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"fsubx\\t%2,%0\";
  return \"fsubs\\t%f2,%0\";
}")

;; multiply instructions

(define_insn "mulhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(mult:HI (match_operand:HI 1 "general_operand" "%0")
		 (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"muls\\t%2,%0\";
}")

(define_insn "mulhisi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(mult:SI (match_operand:HI 1 "general_operand" "%0")
		 (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"muls\\t%2,%0\";
}")

(define_insn "mulsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(mult:SI (match_operand:SI 1 "general_operand" "%0")
		 (match_operand:SI 2 "general_operand" "dmsK")))]
  "TARGET_68020"
  "mulsl\\t%2,%0")

(define_insn "umulhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(umult:HI (match_operand:HI 1 "general_operand" "%0")
		  (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"mulu\\t%2,%0\";
}")

(define_insn "umulhisi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(umult:SI (match_operand:HI 1 "general_operand" "%0")
		  (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"mulu\\t%2,%0\";
}")

(define_insn "umulsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(umult:SI (match_operand:SI 1 "general_operand" "%0")
		  (match_operand:SI 2 "general_operand" "dmsK")))]
  "TARGET_68020"
  "mulul\\t%2,%0")

(define_expand "muldf3"
  [(set (match_operand:DF 0 "general_operand" "")
	(mult:DF (match_operand:DF 1 "general_operand" "")
		 (match_operand:DF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=x,y")
	(mult:DF (match_operand:DF 1 "general_operand" "%xH,y")
		 (match_operand:DF 2 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[1], operands[2]))
    return \"fpsqrd\\t%y1,%0\";
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpmuld\\t%y2,%0\";
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fpmuld\\t%y1,%0\";
  if (which_alternative == 0)
    return \"fpmul3d\\t%w2,%w1,%0\"; 
  return \"fpmul3d\\t%x2,%x1,%0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=f")
	(mult:DF (match_operand:DF 1 "general_operand" "%0")
		 (match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"fmulx\\t%2,%0\";
  return \"fmuld\\t%f2,%0\";
}")

(define_expand "mulsf3"
  [(set (match_operand:SF 0 "general_operand" "")
	(mult:SF (match_operand:SF 1 "general_operand" "")
		 (match_operand:SF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=x,y")
	(mult:SF (match_operand:SF 1 "general_operand" "%xH,y")
		 (match_operand:SF 2 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[1], operands[2]))
    return \"fpsqrs\\t%w1,%0\";
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpmuls\\t%w2,%0\";
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fpmuls\\t%w1,%0\";
  if (which_alternative == 0)
    return \"fpmul3s\\t%w2,%w1,%0\";
  return \"fpmul3s\\t%2,%1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=f")
	(mult:SF (match_operand:SF 1 "general_operand" "%0")
		 (match_operand:SF 2 "general_operand" "fdmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"fsglmulx\\t%2,%0\";
  return \"fsglmuls\\t%f2,%0\";
}")

;; divide instructions

(define_insn "divhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(div:HI (match_operand:HI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"extl\\t%0\;divs\\t%2,%0\";
}")

(define_insn "divhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(div:HI (match_operand:SI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"divs\\t%2,%0\";
}")

(define_insn "divsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(div:SI (match_operand:SI 1 "general_operand" "0")
		(match_operand:SI 2 "general_operand" "dmsK")))]
  "TARGET_68020"
  "divsl\\t%2,%0")

(define_insn "udivhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(udiv:HI (match_operand:HI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"andl\\t%#0xFFFF,%0\;divu\\t%2,%0\";
}")

(define_insn "udivhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(udiv:HI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  return \"divu\\t%2,%0\";
}")

(define_insn "udivsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(udiv:SI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:SI 2 "general_operand" "dmsK")))]
  "TARGET_68020"
  "divul\\t%2,%0")

(define_expand "divdf3"
  [(set (match_operand:DF 0 "general_operand" "")
	(div:DF (match_operand:DF 1 "general_operand" "")
		(match_operand:DF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=x,y,y")
	(div:DF (match_operand:DF 1 "general_operand" "xH,y,rmF")
		(match_operand:DF 2 "general_operand" "xH,rmF,0")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fprdivd\\t%y1,%0\";
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpdivd\\t%y2,%0\";
  if (which_alternative == 0)
    return \"fpdiv3d\\t%w2,%w1,%0\";
  return \"fpdiv3d\\t%x2,%x1,%x0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=f")
	(div:DF (match_operand:DF 1 "general_operand" "0")
		(match_operand:DF 2 "general_operand" "fmG")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]))
    return \"fdivx\\t%2,%0\";
  return \"fdivd\\t%f2,%0\";
}")

(define_expand "divsf3"
  [(set (match_operand:SF 0 "general_operand" "")
	(div:SF (match_operand:SF 1 "general_operand" "")
		(match_operand:SF 2 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=x,y,y")
	(div:SF (match_operand:SF 1 "general_operand" "xH,y,rmF")
		(match_operand:SF 2 "general_operand" "xH,rmF,0")))]
  "TARGET_FPA"
  "*
{
  if (rtx_equal_p (operands[0], operands[1]))
    return \"fpdivs\\t%w2,%0\";
  if (rtx_equal_p (operands[0], operands[2]))
    return \"fprdivs\\t%w1,%0\";
  if (which_alternative == 0)
    return \"fpdiv3s\\t%w2,%w1,%0\";
  return \"fpdiv3s\\t%2,%1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=f")
	(div:SF (match_operand:SF 1 "general_operand" "0")
		(match_operand:SF 2 "general_operand" "fdmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return \"fsgldivx\\t%2,%0\";
  return \"fsgldivs\\t%f2,%0\";
}")

;; Remainder instructions.

(define_insn "modhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(mod:HI (match_operand:HI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return \"extl %0\;divs %2,%0\;swap\\t%0\";
}")

(define_insn "modhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(mod:HI (match_operand:SI 1 "general_operand" "0")
		(match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return \"divs\\t%2,%0\;swap\\t%0\";
}")

(define_insn "umodhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(umod:HI (match_operand:HI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return \"andl\\t%#0xFFFF,%0\;divu\\t%2,%0\;swap\\t%0\";
}")

(define_insn "umodhisi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(umod:HI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:HI 2 "general_operand" "dmn")))]
  ""
  "*
{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return \"divu\\t%2,%0\;swap\\t%0\";
}")

(define_insn "divmodsi4"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(div:SI (match_operand:SI 1 "general_operand" "0")
		(match_operand:SI 2 "general_operand" "dmsK")))
   (set (match_operand:SI 3 "general_operand" "=d")
	(mod:SI (match_dup 1) (match_dup 2)))]
  "TARGET_68020"
  "divsll\\t%2,%3:%0")

(define_insn "udivmodsi4"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(udiv:SI (match_operand:SI 1 "general_operand" "0")
		 (match_operand:SI 2 "general_operand" "dmsK")))
   (set (match_operand:SI 3 "general_operand" "=d")
	(umod:SI (match_dup 1) (match_dup 2)))]
  "TARGET_68020"
  "divull\\t%2,%3:%0")

;; logical-and instructions

(define_insn "andsi3"
  [(set (match_operand:SI 0 "general_operand" "=m,d")
	(and:SI (match_operand:SI 1 "general_operand" "%0,0")
		(match_operand:SI 2 "general_operand" "dKs,dmKs")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT
      && (INTVAL (operands[2]) | 0xffff) == 0xffffffff
      && (DATA_REG_P (operands[0])
	  || offsettable_memref_p (operands[0])))
    { 
      if (GET_CODE (operands[0]) != REG)
        operands[0] = adj_offsettable_operand (operands[0], 2);
      operands[2] = gen_rtx (CONST_INT, VOIDmode,
			     INTVAL (operands[2]) & 0xffff);
      /* Do not delete a following tstl %0 insn; that would be incorrect.  */
      CC_STATUS_INIT;
      if (operands[2] == const0_rtx)
        return \"clrw\\t%0\";
      return \"andw\\t%2,%0\";
    }
  return \"andl\\t%2,%0\";
}")

(define_insn "andhi3"
  [(set (match_operand:HI 0 "general_operand" "=m,d")
	(and:HI (match_operand:HI 1 "general_operand" "%0,0")
		(match_operand:HI 2 "general_operand" "dn,dmn")))]
  ""
  "andw\\t%2,%0")

(define_insn "andqi3"
  [(set (match_operand:QI 0 "general_operand" "=m,d")
	(and:QI (match_operand:QI 1 "general_operand" "%0,0")
		(match_operand:QI 2 "general_operand" "dn,dmn")))]
  ""
  "andb\\t%2,%0")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(and:SI (zero_extend:SI (match_operand:HI 1 "general_operand" "dm"))
		(match_operand:SI 2 "general_operand" "0")))]
  "GET_CODE (operands[2]) == CONST_INT
   && (unsigned int) INTVAL (operands[2]) < (1 << GET_MODE_BITSIZE (HImode))"
  "andw\\t%1,%0")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(and:SI (zero_extend:SI (match_operand:QI 1 "general_operand" "dm"))
		(match_operand:SI 2 "general_operand" "0")))]
  "GET_CODE (operands[2]) == CONST_INT
   && (unsigned int) INTVAL (operands[2]) < (1 << GET_MODE_BITSIZE (QImode))"
  "andb\\t%1,%0")

;; inclusive-or instructions

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "general_operand" "=m,d")
	(ior:SI (match_operand:SI 1 "general_operand" "%0,0")
		(match_operand:SI 2 "general_operand" "dKs,dmKs")))]
  ""
  "*
{
  register int logval;
  if (GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[2]) >> 16 == 0
      && (DATA_REG_P (operands[0])
	  || offsettable_memref_p (operands[0])))
    { 
      if (GET_CODE (operands[0]) != REG)
        operands[0] = adj_offsettable_operand (operands[0], 2);
      /* Do not delete a following tstl %0 insn; that would be incorrect.  */
      CC_STATUS_INIT;
      return \"orw\\t%2,%0\";
    }
  if (GET_CODE (operands[2]) == CONST_INT
      && (logval = exact_log2 (INTVAL (operands[2]))) >= 0
      && (DATA_REG_P (operands[0])
	  || offsettable_memref_p (operands[0])))
    { 
      if (DATA_REG_P (operands[0]))
	{
	  operands[1] = gen_rtx (CONST_INT, VOIDmode, logval);
	}
      else
        {
	  operands[0] = adj_offsettable_operand (operands[0], 3 - (logval / 8));
	  operands[1] = gen_rtx (CONST_INT, VOIDmode, logval % 8);
	}
      return \"bset\\t%1,%0\";
    }
  return \"orl\\t%2,%0\";
}")

(define_insn "iorhi3"
  [(set (match_operand:HI 0 "general_operand" "=m,d")
	(ior:HI (match_operand:HI 1 "general_operand" "%0,0")
		(match_operand:HI 2 "general_operand" "dn,dmn")))]
  ""
  "orw\\t%2,%0")

(define_insn "iorqi3"
  [(set (match_operand:QI 0 "general_operand" "=m,d")
	(ior:QI (match_operand:QI 1 "general_operand" "%0,0")
		(match_operand:QI 2 "general_operand" "dn,dmn")))]
  ""
  "orb\\t%2,%0")

;; xor instructions

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "general_operand" "=do,m")
	(xor:SI (match_operand:SI 1 "general_operand" "%0,0")
		(match_operand:SI 2 "general_operand" "di,dKs")))]
  ""
  "*
{
  if (GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[2]) >> 16 == 0
      && (offsettable_memref_p (operands[0]) || DATA_REG_P (operands[0])))
    { 
      if (! DATA_REG_P (operands[0]))
	operands[0] = adj_offsettable_operand (operands[0], 2);
      /* Do not delete a following tstl %0 insn; that would be incorrect.  */
      CC_STATUS_INIT;
      return \"eorw\\t%2,%0\";
    }
  return \"eorl\\t%2,%0\";
}")

(define_insn "xorhi3"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(xor:HI (match_operand:HI 1 "general_operand" "%0")
		(match_operand:HI 2 "general_operand" "dn")))]
  ""
  "eorw\\t%2,%0")

(define_insn "xorqi3"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(xor:QI (match_operand:QI 1 "general_operand" "%0")
		(match_operand:QI 2 "general_operand" "dn")))]
  ""
  "eorb\\t%2,%0")

;; negation instructions

(define_insn "negsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(neg:SI (match_operand:SI 1 "general_operand" "0")))]
  ""
  "negl\\t%0")

(define_insn "neghi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(neg:HI (match_operand:HI 1 "general_operand" "0")))]
  ""
  "negw\\t%0")

(define_insn "negqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(neg:QI (match_operand:QI 1 "general_operand" "0")))]
  ""
  "negb\\t%0")

(define_expand "negsf2"
  [(set (match_operand:SF 0 "general_operand" "")
	(neg:SF (match_operand:SF 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=x,y")
	(neg:SF (match_operand:SF 1 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "fpnegs\\t%w1,%0")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=f")
	(neg:SF (match_operand:SF 1 "general_operand" "fdmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fnegx\\t%1,%0\";
  return \"fnegs\\t%f1,%0\";
}")

(define_expand "negdf2"
  [(set (match_operand:DF 0 "general_operand" "")
	(neg:DF (match_operand:DF 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=x,y")
	(neg:DF (match_operand:DF 1 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "fpnegd\\t%y1,%0")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=f")
	(neg:DF (match_operand:DF 1 "general_operand" "fmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fnegx\\t%1,%0\";
  return \"fnegd\\t%f1,%0\";
}")

;; Absolute value instructions

(define_expand "abssf2"
  [(set (match_operand:SF 0 "general_operand" "")
	(abs:SF (match_operand:SF 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=x,y")
	(abs:SF (match_operand:SF 1 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "fpabss\\t%y1,%0")

(define_insn ""
  [(set (match_operand:SF 0 "general_operand" "=f")
	(abs:SF (match_operand:SF 1 "general_operand" "fdmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fabsx\\t%1,%0\";
  return \"fabss\\t%f1,%0\";
}")

(define_expand "absdf2"
  [(set (match_operand:DF 0 "general_operand" "")
	(abs:DF (match_operand:DF 1 "general_operand" "")))]
  "TARGET_68881 || TARGET_FPA"
  "")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=x,y")
	(abs:DF (match_operand:DF 1 "general_operand" "xH,rmF")))]
  "TARGET_FPA"
  "fpabsd\\t%y1,%0")

(define_insn ""
  [(set (match_operand:DF 0 "general_operand" "=f")
	(abs:DF (match_operand:DF 1 "general_operand" "fmF")))]
  "TARGET_68881"
  "*
{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return \"fabsx\\t%1,%0\";
  return \"fabsd\\t%f1,%0\";
}")

;; one complement instructions

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "general_operand" "=dm")
	(not:SI (match_operand:SI 1 "general_operand" "0")))]
  ""
  "notl\\t%0")

(define_insn "one_cmplhi2"
  [(set (match_operand:HI 0 "general_operand" "=dm")
	(not:HI (match_operand:HI 1 "general_operand" "0")))]
  ""
  "notw\\t%0")

(define_insn "one_cmplqi2"
  [(set (match_operand:QI 0 "general_operand" "=dm")
	(not:QI (match_operand:QI 1 "general_operand" "0")))]
  ""
  "notb\\t%0")

;; Optimized special case of shifting.
;; Must precede the general case.

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ashiftrt:SI (match_operand:SI 1 "memory_operand" "m")
		     (const_int 24)))]
  "GET_CODE (XEXP (operands[1], 0)) != POST_INC
   && GET_CODE (XEXP (operands[1], 0)) != PRE_DEC"
  "*
{
  if (TARGET_68020)
    return \"movb\\t%1,%0\;extbl\\t%0\";
  return \"movb\\t%1,%0\;extw\\t%0\;extl\\t%0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(lshiftrt:SI (match_operand:SI 1 "memory_operand" "m")
		     (const_int 24)))]
  "GET_CODE (XEXP (operands[1], 0)) != POST_INC
   && GET_CODE (XEXP (operands[1], 0)) != PRE_DEC"
  "*
{
  if (reg_mentioned_p (operands[0], operands[1]))
    return \"movb\\t%1,%0\;andl\\t%#0xFF,%0\";
  return \"clrl\\t%0\;movb\\t%1,%0\";
}")

(define_insn ""
  [(set (cc0) (compare (match_operand:QI 0 "general_operand" "i")
		       (lshiftrt:SI (match_operand:SI 1 "memory_operand" "m")
				    (const_int 24))))]
  "(GET_CODE (operands[0]) == CONST_INT
    && (INTVAL (operands[0]) & ~0xff) == 0
    && GET_CODE (XEXP (operands[1], 0)) != POST_INC
    && GET_CODE (XEXP (operands[1], 0)) != PRE_DEC)"
  "* cc_status.flags |= CC_REVERSED;
  return \"cmpb\\t%0,%1\";
")

(define_insn ""
  [(set (cc0) (compare (lshiftrt:SI (match_operand:SI 0 "memory_operand" "m")
				    (const_int 24))
		       (match_operand:QI 1 "general_operand" "i")))]
  "(GET_CODE (operands[1]) == CONST_INT
    && (INTVAL (operands[1]) & ~0xff) == 0
    && GET_CODE (XEXP (operands[0], 0)) != POST_INC
    && GET_CODE (XEXP (operands[0], 0)) != PRE_DEC)"
  "*
  return \"cmpb\\t%1,%0\";
")

(define_insn ""
  [(set (cc0) (compare (match_operand:QI 0 "general_operand" "i")
		       (ashiftrt:SI (match_operand:SI 1 "memory_operand" "m")
				    (const_int 24))))]
  "(GET_CODE (operands[0]) == CONST_INT
    && ((INTVAL (operands[0]) + 0x80) & ~0xff) == 0
    && GET_CODE (XEXP (operands[1], 0)) != POST_INC
    && GET_CODE (XEXP (operands[1], 0)) != PRE_DEC)"
  "* cc_status.flags |= CC_REVERSED;
  return \"cmpb\\t%0,%1\";
")

(define_insn ""
  [(set (cc0) (compare (ashiftrt:SI (match_operand:SI 0 "memory_operand" "m")
				    (const_int 24))
		       (match_operand:QI 1 "general_operand" "i")))]
  "(GET_CODE (operands[1]) == CONST_INT
    && ((INTVAL (operands[1]) + 0x80) & ~0xff) == 0
    && GET_CODE (XEXP (operands[0], 0)) != POST_INC
    && GET_CODE (XEXP (operands[0], 0)) != PRE_DEC)"
  "*
  return \"cmpb\\t%1,%0\";
")

;; arithmetic shift instructions
;; We don't need the shift memory by 1 bit instruction

(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ashift:SI (match_operand:SI 1 "general_operand" "0")
		   (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "asll\\t%2,%0")

(define_insn "ashlhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(ashift:HI (match_operand:HI 1 "general_operand" "0")
		   (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "aslw\\t%2,%0")

(define_insn "ashlqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(ashift:QI (match_operand:QI 1 "general_operand" "0")
		   (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "aslb\\t%2,%0")

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(ashiftrt:SI (match_operand:SI 1 "general_operand" "0")
		     (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "asrl\\t%2,%0")

(define_insn "ashrhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(ashiftrt:HI (match_operand:HI 1 "general_operand" "0")
		     (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "asrw\\t%2,%0")

(define_insn "ashrqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(ashiftrt:QI (match_operand:QI 1 "general_operand" "0")
		     (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "asrb\\t%2,%0")

;; logical shift instructions

(define_insn "lshlsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(lshift:SI (match_operand:SI 1 "general_operand" "0")
		   (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "lsll\\t%2,%0")

(define_insn "lshlhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(lshift:HI (match_operand:HI 1 "general_operand" "0")
		   (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "lslw\\t%2,%0")

(define_insn "lshlqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(lshift:QI (match_operand:QI 1 "general_operand" "0")
		   (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "lslb\\t%2,%0")

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(lshiftrt:SI (match_operand:SI 1 "general_operand" "0")
		     (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "lsrl\\t%2,%0")

(define_insn "lshrhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(lshiftrt:HI (match_operand:HI 1 "general_operand" "0")
		     (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "lsrw\\t%2,%0")

(define_insn "lshrqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(lshiftrt:QI (match_operand:QI 1 "general_operand" "0")
		     (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "lsrb\\t%2,%0")

;; rotate instructions

(define_insn "rotlsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(rotate:SI (match_operand:SI 1 "general_operand" "0")
		   (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "roll\\t%2,%0")

(define_insn "rotlhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(rotate:HI (match_operand:HI 1 "general_operand" "0")
		   (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "rolw\\t%2,%0")

(define_insn "rotlqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(rotate:QI (match_operand:QI 1 "general_operand" "0")
		   (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "rolb\\t%2,%0")

(define_insn "rotrsi3"
  [(set (match_operand:SI 0 "general_operand" "=d")
	(rotatert:SI (match_operand:SI 1 "general_operand" "0")
		     (match_operand:SI 2 "general_operand" "dI")))]
  ""
  "rorl\\t%2,%0")

(define_insn "rotrhi3"
  [(set (match_operand:HI 0 "general_operand" "=d")
	(rotatert:HI (match_operand:HI 1 "general_operand" "0")
		     (match_operand:HI 2 "general_operand" "dI")))]
  ""
  "rorw\\t%2,%0")

(define_insn "rotrqi3"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(rotatert:QI (match_operand:QI 1 "general_operand" "0")
		     (match_operand:QI 2 "general_operand" "dI")))]
  ""
  "rorb\\t%2,%0")

;; Special cases of bit-field insns which we should
;; recognize in preference to the general case.
;; These handle aligned 8-bit and 16-bit fields,
;; which can usually be done with move instructions.

(define_insn ""
  [(set (zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "+do")
			 (match_operand:SI 1 "immediate_operand" "i")
			 (match_operand:SI 2 "immediate_operand" "i"))
	(match_operand:SI 3 "general_operand" "d"))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT
   && (INTVAL (operands[1]) == 8 || INTVAL (operands[1]) == 16)
   && GET_CODE (operands[2]) == CONST_INT
   && INTVAL (operands[2]) % INTVAL (operands[1]) == 0
   && (GET_CODE (operands[0]) == REG
       || ! mode_dependent_address_p (XEXP (operands[0], 0)))"
  "*
{
  if (REG_P (operands[0]))
    {
      if (INTVAL (operands[1]) + INTVAL (operands[2]) != 32)
        return \"bfins\\t%3,%0{%b2:%b1}\";
    }
  else
    operands[0]
      = adj_offsettable_operand (operands[0], INTVAL (operands[2]) / 8);

  if (GET_CODE (operands[3]) == MEM)
    operands[3] = adj_offsettable_operand (operands[3],
					   (32 - INTVAL (operands[1])) / 8);
  if (INTVAL (operands[1]) == 8)
    return \"movb\\t%3,%0\";
  return \"movw\\t%3,%0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=&d")
	(zero_extract:SI (match_operand:SI 1 "nonimmediate_operand" "do")
			 (match_operand:SI 2 "immediate_operand" "i")
			 (match_operand:SI 3 "immediate_operand" "i")))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[2]) == CONST_INT
   && (INTVAL (operands[2]) == 8 || INTVAL (operands[2]) == 16)
   && GET_CODE (operands[3]) == CONST_INT
   && INTVAL (operands[3]) % INTVAL (operands[2]) == 0
   && (GET_CODE (operands[1]) == REG
       || ! mode_dependent_address_p (XEXP (operands[1], 0)))"
  "*
{
  cc_status.flags |= CC_NOT_NEGATIVE;
  if (REG_P (operands[1]))
    {
      if (INTVAL (operands[2]) + INTVAL (operands[3]) != 32)
	return \"bfextu\\t%1{%b3:%b2},%0\";
    }
  else
    operands[1]
      = adj_offsettable_operand (operands[1], INTVAL (operands[3]) / 8);

  output_asm_insn (\"clrl\\t%0\", operands);
  if (GET_CODE (operands[0]) == MEM)
    operands[0] = adj_offsettable_operand (operands[0],
					   (32 - INTVAL (operands[1])) / 8);
  if (INTVAL (operands[2]) == 8)
    return \"movb\\t%1,%0\";
  return \"movw\\t%1,%0\";
}")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(sign_extract:SI (match_operand:SI 1 "nonimmediate_operand" "do")
			 (match_operand:SI 2 "immediate_operand" "i")
			 (match_operand:SI 3 "immediate_operand" "i")))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[2]) == CONST_INT
   && (INTVAL (operands[2]) == 8 || INTVAL (operands[2]) == 16)
   && GET_CODE (operands[3]) == CONST_INT
   && INTVAL (operands[3]) % INTVAL (operands[2]) == 0
   && (GET_CODE (operands[1]) == REG
       || ! mode_dependent_address_p (XEXP (operands[1], 0)))"
  "*
{
  if (REG_P (operands[1]))
    {
      if (INTVAL (operands[2]) + INTVAL (operands[3]) != 32)
	return \"bfexts\\t%1{%b3:%b2},%0\";
    }
  else
    operands[1]
      = adj_offsettable_operand (operands[1], INTVAL (operands[3]) / 8);

  if (INTVAL (operands[2]) == 8)
    return \"movb\\t%1,%0\;extbl\\t%0\";
  return \"movw\\t%1,%0\;extl\\t%0\";
}")

;; Bit field instructions, general cases.
;; "o,d" constraint causes a nonoffsettable memref to match the "o"
;; so that its address is reloaded.

(define_insn "extv"
  [(set (match_operand:SI 0 "general_operand" "=d,d")
	(sign_extract:SI (match_operand:QI 1 "nonimmediate_operand" "o,d")
			 (match_operand:SI 2 "general_operand" "di,di")
			 (match_operand:SI 3 "general_operand" "di,di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfexts\\t%1{%b3:%b2},%0")

(define_insn "extzv"
  [(set (match_operand:SI 0 "general_operand" "=d,d")
	(zero_extract:SI (match_operand:QI 1 "nonimmediate_operand" "o,d")
			 (match_operand:SI 2 "general_operand" "di,di")
			 (match_operand:SI 3 "general_operand" "di,di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
  cc_status.flags |= CC_NOT_NEGATIVE;
  return \"bfextu\\t%1{%b3:%b2},%0\";
}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:QI 0 "nonimmediate_operand" "+o,d")
			 (match_operand:SI 1 "general_operand" "di,di")
			 (match_operand:SI 2 "general_operand" "di,di"))
        (xor:SI (zero_extract:SI (match_dup 0) (match_dup 1) (match_dup 2))
		(match_operand 3 "immediate_operand" "i,i")))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[3]) == CONST_INT
   && (INTVAL (operands[3]) == -1
       || (GET_CODE (operands[1]) == CONST_INT
           && (~ INTVAL (operands[3]) & ((1 << INTVAL (operands[1]))- 1)) == 0))"
  "*
{
  CC_STATUS_INIT;
  return \"bfchg\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:QI 0 "nonimmediate_operand" "+o,d")
			 (match_operand:SI 1 "general_operand" "di,di")
			 (match_operand:SI 2 "general_operand" "di,di"))
	(const_int 0))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
  CC_STATUS_INIT;
  return \"bfclr\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:QI 0 "nonimmediate_operand" "+o,d")
			 (match_operand:SI 1 "general_operand" "di,di")
			 (match_operand:SI 2 "general_operand" "di,di"))
	(const_int -1))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
  CC_STATUS_INIT;
  return \"bfset\\t%0{%b2:%b1}\";
}")

(define_insn "insv"
  [(set (zero_extract:SI (match_operand:QI 0 "nonimmediate_operand" "+o,d")
			 (match_operand:SI 1 "general_operand" "di,di")
			 (match_operand:SI 2 "general_operand" "di,di"))
	(match_operand:SI 3 "general_operand" "d,d"))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfins\\t%3,%0{%b2:%b1}")

;; Now recognize bit field insns that operate on registers
;; (or at least were intended to do so).

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(sign_extract:SI (match_operand:SI 1 "nonimmediate_operand" "d")
			 (match_operand:SI 2 "general_operand" "di")
			 (match_operand:SI 3 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "bfexts\\t%1{%b3:%b2},%0")

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=d")
	(zero_extract:SI (match_operand:SI 1 "nonimmediate_operand" "d")
			 (match_operand:SI 2 "general_operand" "di")
			 (match_operand:SI 3 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
  cc_status.flags |= CC_NOT_NEGATIVE;
  return \"bfextu\\t%1{%b3:%b2},%0\";
}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "+d")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(const_int 0))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
  CC_STATUS_INIT;
  return \"bfclr\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "+d")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(const_int -1))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
  CC_STATUS_INIT;
  return \"bfset\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "+d")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di"))
	(match_operand:SI 3 "general_operand" "d"))]
  "TARGET_68020 && TARGET_BITFIELD"
  "*
{
#if 0	/* --------------------------------------------------- */
  /* These special cases are now recognized by a specific pattern.  */
  if (GET_CODE (operands[1]) == CONST_INT && GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[1]) == 16 && INTVAL (operands[2]) == 16)
    return \"movw\\t%3,%0\";
  if (GET_CODE (operands[1]) == CONST_INT && GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[1]) == 24 && INTVAL (operands[2]) == 8)
    return \"movb\\t%3,%0\";
#endif	/* --------------------------------------------------- */
  return \"bfins\\t%3,%0{%b2:%b1}\";
}")

;; Special patterns for optimizing bit-field instructions.

(define_insn ""
  [(set (cc0)
	(zero_extract:SI (match_operand:QI 0 "memory_operand" "o")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (cc0)
	(subreg:QI
	 (zero_extract:SI (match_operand:QI 0 "memory_operand" "o")
			  (match_operand:SI 1 "general_operand" "di")
			  (match_operand:SI 2 "general_operand" "di"))
	 0))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (cc0)
	(subreg:HI
	 (zero_extract:SI (match_operand:QI 0 "memory_operand" "o")
			  (match_operand:SI 1 "general_operand" "di")
			  (match_operand:SI 2 "general_operand" "di"))
	 0))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst\\t%0{%b2:%b1}\";
}")
  
;;; now handle the register cases
(define_insn ""
  [(set (cc0)
	(zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "d")
			 (match_operand:SI 1 "general_operand" "di")
			 (match_operand:SI 2 "general_operand" "di")))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (cc0)
	(subreg:QI
	 (zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "d")
			  (match_operand:SI 1 "general_operand" "di")
			  (match_operand:SI 2 "general_operand" "di"))
	 0))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst\\t%0{%b2:%b1}\";
}")

(define_insn ""
  [(set (cc0)
	(subreg:HI
	 (zero_extract:SI (match_operand:SI 0 "nonimmediate_operand" "d")
			  (match_operand:SI 1 "general_operand" "di")
			  (match_operand:SI 2 "general_operand" "di"))
	 0))]
  "TARGET_68020 && TARGET_BITFIELD
   && GET_CODE (operands[1]) == CONST_INT"
  "*
{
  if (operands[1] == const1_rtx
      && GET_CODE (operands[2]) == CONST_INT)
    {    
      int width = GET_CODE (operands[0]) == REG ? 31 : 7;
      return output_btst (operands,
			  gen_rtx (CONST_INT, VOIDmode,
				   width - INTVAL (operands[2])),
			  operands[0],
			  insn, 1000);
      /* Pass 1000 as SIGNPOS argument so that btst will
         not think we are testing the sign bit for an `and'
	 and assume that nonzero implies a negative result.  */
    }
  if (INTVAL (operands[1]) != 32)
    cc_status.flags = CC_NOT_NEGATIVE;
  return \"bftst\\t%0{%b2:%b1}\";
}")

(define_insn "seq"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(eq (cc0) (const_int 0)))]
  ""
  "*
  cc_status = cc_prev_status;
  OUTPUT_JUMP (\"seq\\t%0\", \"fseq\\t%0\", \"seq\\t%0\");
")

(define_insn "sne"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(ne (cc0) (const_int 0)))]
  ""
  "*
  cc_status = cc_prev_status;
  OUTPUT_JUMP (\"sne\\t%0\", \"fsne\\t%0\", \"sne\\t%0\");
")

(define_insn "sgt"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(gt (cc0) (const_int 0)))]
  ""
  "*
  cc_status = cc_prev_status;
  OUTPUT_JUMP (\"sgt\\t%0\", \"fsgt\\t%0\", 0);
")

(define_insn "sgtu"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(gtu (cc0) (const_int 0)))]
  ""
  "* cc_status = cc_prev_status;
     return \"shi\\t%0\"; ")

(define_insn "slt"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(lt (cc0) (const_int 0)))]
  ""
  "* cc_status = cc_prev_status;
     OUTPUT_JUMP (\"slt\\t%0\", \"fslt\\t%0\", \"smi\\t%0\"); ")

(define_insn "sltu"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(ltu (cc0) (const_int 0)))]
  ""
  "* cc_status = cc_prev_status;
     return \"scs\\t%0\"; ")

(define_insn "sge"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(ge (cc0) (const_int 0)))]
  ""
  "* cc_status = cc_prev_status;
     OUTPUT_JUMP (\"sge\\t%0\", \"fsge\\t%0\", \"spl\\t%0\"); ")

(define_insn "sgeu"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(geu (cc0) (const_int 0)))]
  ""
  "* cc_status = cc_prev_status;
     return \"scc\\t%0\"; ")

(define_insn "sle"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(le (cc0) (const_int 0)))]
  ""
  "*
  cc_status = cc_prev_status;
  OUTPUT_JUMP (\"sle\\t%0\", \"fsle\\t%0\", 0);
")

(define_insn "sleu"
  [(set (match_operand:QI 0 "general_operand" "=d")
	(leu (cc0) (const_int 0)))]
  ""
  "* cc_status = cc_prev_status;
     return \"sls\\t%0\"; ")

;; Basic conditional jump instructions.

(define_insn "beq"
  [(set (pc)
	(if_then_else (eq (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
{
  OUTPUT_JUMP (\"jeq\\t%l0\", \"fjeq\\t%l0\", \"jeq\\t%l0\");
}")

(define_insn "bne"
  [(set (pc)
	(if_then_else (ne (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
{
  OUTPUT_JUMP (\"jne\\t%l0\", \"fjne\\t%l0\", \"jne\\t%l0\");
}")

(define_insn "bgt"
  [(set (pc)
	(if_then_else (gt (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  OUTPUT_JUMP (\"jgt\\t%l0\", \"fjgt\\t%l0\", 0);
")

(define_insn "bgtu"
  [(set (pc)
	(if_then_else (gtu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  return \"jhi\\t%l0\";
")

(define_insn "blt"
  [(set (pc)
	(if_then_else (lt (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  OUTPUT_JUMP (\"jlt\\t%l0\", \"fjlt\\t%l0\", \"jmi\\t%l0\");
")

(define_insn "bltu"
  [(set (pc)
	(if_then_else (ltu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  return \"jcs\\t%l0\";
")

(define_insn "bge"
  [(set (pc)
	(if_then_else (ge (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  OUTPUT_JUMP (\"jge\\t%l0\", \"fjge\\t%l0\", \"jpl\\t%l0\");
")

(define_insn "bgeu"
  [(set (pc)
	(if_then_else (geu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  return \"jcc\\t%l0\";
")

(define_insn "ble"
  [(set (pc)
	(if_then_else (le (cc0)
			  (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  OUTPUT_JUMP (\"jle\\t%l0\", \"fjle\\t%l0\", 0);
")

(define_insn "bleu"
  [(set (pc)
	(if_then_else (leu (cc0)
			   (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  "*
  return \"jls\\t%l0\";
")

;; Negated conditional jump instructions.

(define_insn ""
  [(set (pc)
	(if_then_else (eq (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
{
  OUTPUT_JUMP (\"jne\\t%l0\", \"fjne\\t%l0\", \"jne\\t%l0\");
}")

(define_insn ""
  [(set (pc)
	(if_then_else (ne (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
{
  OUTPUT_JUMP (\"jeq\\t%l0\", \"fjeq\\t%l0\", \"jeq\\t%l0\");
}")

(define_insn ""
  [(set (pc)
	(if_then_else (gt (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  OUTPUT_JUMP (\"jle\\t%l0\", \"fjngt\\t%l0\", 0);
")

(define_insn ""
  [(set (pc)
	(if_then_else (gtu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  return \"jls\\t%l0\";
")

(define_insn ""
  [(set (pc)
	(if_then_else (lt (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  OUTPUT_JUMP (\"jge\\t%l0\", \"fjnlt\\t%l0\", \"jpl\\t%l0\");
")

(define_insn ""
  [(set (pc)
	(if_then_else (ltu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  return \"jcc\\t%l0\";
")

(define_insn ""
  [(set (pc)
	(if_then_else (ge (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  OUTPUT_JUMP (\"jlt\\t%l0\", \"fjnge\\t%l0\", \"jmi\\t%l0\");
")

(define_insn ""
  [(set (pc)
	(if_then_else (geu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  return \"jcs\\t%l0\";
")

(define_insn ""
  [(set (pc)
	(if_then_else (le (cc0)
			  (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  OUTPUT_JUMP (\"jgt\\t%l0\", \"fjnle\\t%l0\", 0);
")

(define_insn ""
  [(set (pc)
	(if_then_else (leu (cc0)
			   (const_int 0))
		      (pc)
		      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
  return \"jhi\\t%l0\";
")

;; Subroutines of "casesi".

(define_expand "casesi_1"
  [(set (match_operand:SI 3 "general_operand" "")
	(plus:SI (match_operand:SI 0 "general_operand" "")
		 ;; Note operand 1 has been negated!
		 (match_operand:SI 1 "immediate_operand" "")))
   (set (cc0) (compare (match_operand:SI 2 "general_operand" "")
		       (match_dup 3)))
   (set (pc) (if_then_else (ltu (cc0) (const_int 0))
			   (label_ref (match_operand 4 "" "")) (pc)))]
  ""
  "")

(define_expand "casesi_2"
  [(set (match_operand:SI 0 "" "") (mem:HI (match_operand:SI 1 "" "")))
   ;; The USE here is so that at least one jump-insn will refer to the label,
   ;; to keep it alive in jump_optimize.
   (parallel [(set (pc)
		   (plus:SI (pc) (match_dup 0)))
	      (use (label_ref (match_operand 2 "" "")))])]
  ""
  "")

;; Operand 0 is index (in bytes); operand 1 is minimum, operand 2 themaximum;
;; operand 3 is CODE_LABEL for the table;
;; operand 4 is the CODE_LABEL to go to if index out of range.
(define_expand "casesi"
  ;; We don't use these for generating the RTL, but we must describe
  ;; the operands here.
  [(match_operand:SI 0 "general_operand" "")
   (match_operand:SI 1 "immediate_operand" "")
   (match_operand:SI 2 "general_operand" "")
   (match_operand 3 "" "")
   (match_operand 4 "" "")]
  ""
  "
{
  rtx table_elt_addr;
  rtx index_diff;

  operands[1] = negate_rtx (SImode, operands[1]);
  index_diff = gen_reg_rtx (SImode);
  /* Emit the first few insns.  */
  emit_insn (gen_casesi_1 (operands[0], operands[1], operands[2],
			   index_diff, operands[4]));
  /* Construct a memory address.  This may emit some insns.  */
  table_elt_addr
    = memory_address_noforce
        (HImode,
	 gen_rtx (PLUS, Pmode,
		  gen_rtx (MULT, Pmode, index_diff,
			   gen_rtx (CONST_INT, VOIDmode, 2)),
		  gen_rtx (LABEL_REF, VOIDmode, operands[3])));
  /* Emit the last few insns.  */
  emit_insn (gen_casesi_2 (gen_reg_rtx (HImode), table_elt_addr, operands[3]));
  DONE;
}")

;; Recognize one of the insns resulting from casesi_2.
(define_insn ""
  [(set (pc)
	(plus:SI (pc) (match_operand:HI 0 "general_operand" "r")))
   (use (label_ref (match_operand 1 "" "")))]
  ""
  "*
{
#ifdef ASM_RETURN_CASE_JUMP
 ASM_RETURN_CASE_JUMP;
#else
  return \"jmp\\t2(pc,%0:w)\";
#endif /* no ASM_RETURN_CASE_JUMP */
}")

;; Unconditional and other jump instructions
(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  "*
  return \"jra\\t%l0\";
")

(define_insn ""
  [(set (pc)
	(if_then_else
	 (ne (compare (plus:HI (match_operand:HI 0 "general_operand" "=g")
			       (const_int -1))
		      (const_int -1))
	     (const_int 0))
	 (label_ref (match_operand 1 "" ""))
	 (pc)))
   (set (match_dup 0)
	(plus:HI (match_dup 0)
		 (const_int -1)))]
  ""
  "*
{
  CC_STATUS_INIT;
  if (DATA_REG_P (operands[0]))
    return \"dbra\\t%0,%l1\";
  if (GET_CODE (operands[0]) == MEM)
    {
      return \"subqw\\t%#1,%0\;jcc\\t%l1\";
    }
  return \"subqw\\t%#1,%0\;cmpw\\t%#-1,%0\;jne\\t%l1\";
}")

(define_insn ""
  [(set (pc)
	(if_then_else
	 (ne (compare (plus:SI (match_operand:SI 0 "general_operand" "=g")
			       (const_int -1))
		      (const_int -1))
	     (const_int 0))
	 (label_ref (match_operand 1 "" ""))
	 (pc)))
   (set (match_dup 0)
	(plus:SI (match_dup 0)
		 (const_int -1)))]
  ""
  "*
{
  CC_STATUS_INIT;
  if (DATA_REG_P (operands[0]))
    return \"dbra\\t%0,%l1\;clrw\\t%0\;subql\\t%#1,%0\;jcc\\t%l1\";
  if (GET_CODE (operands[0]) == MEM)
    return \"subql\\t%#1,%0\;jcc\\t%l1\";
  return \"subql\\t%#1,%0\;cmpl\\t%#-1,%0\;jne\\t%l1\";
}")

;; dbra patterns that use REG_NOTES info generated by strength_reduce.

(define_insn ""
  [(set (pc)
	(if_then_else
	  (ge (plus:SI (match_operand:SI 0 "general_operand" "=g")
			(const_int -1))
	      (const_int 0))
	  (label_ref (match_operand 1 "" ""))
	  (pc)))
   (set (match_dup 0)
	(plus:SI (match_dup 0)
		 (const_int -1)))]
  "find_reg_note (insn, REG_NONNEG, 0)"
  "*
{
  CC_STATUS_INIT;
  if (DATA_REG_P (operands[0]))
    return \"dbra\\t%0,%l1\;clrw\\t%0\;subql\\t%#1,%0\;jcc\\t%l1\";
  if (GET_CODE (operands[0]) == MEM)
    return \"subql\\t%#1,%0\;jcc\\t%l1\";
  return \"subql\\t%#1,%0\;cmpl\\t%#-1,%0\;jne\\t%l1\";
}")

;; Call subroutine with no return value.
(define_insn "call"
  [(call (match_operand:QI 0 "general_operand" "o")
	 (match_operand:SI 1 "general_operand" "g"))]
  ;; Operand 1 not really used on the m68000.

  ""
  "*
  return \"jbsr\\t%0\";
")

;; Call subroutine, returning value in operand 0
;; (which must be a hard register).
(define_insn "call_value"
  [(set (match_operand 0 "" "=rf")
	(call (match_operand:QI 1 "general_operand" "o")
	      (match_operand:SI 2 "general_operand" "g")))]
  ;; Operand 2 not really used on the m68000.
  ""
  "*
  return \"jbsr\\t%1\";
")

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

;; This should not be used unless the add/sub insns can't be.

(define_insn ""
  [(set (match_operand:SI 0 "general_operand" "=a")
	(match_operand:QI 1 "address_operand" "p"))]
  ""
  "lea\\t%a1,%0")

;; This is the first machine-dependent peephole optimization.
;; It is useful when a floating value is returned from a function call
;; and then is moved into an FP register.
;; But it is mainly intended to test the support for these optimizations.

(define_peephole
  [(set (reg:SI 15) (plus:SI (reg:SI 15) (const_int 4)))
   (set (match_operand:DF 0 "register_operand" "f")
	(match_operand:DF 1 "register_operand" "ad"))]
  "FP_REG_P (operands[0]) && ! FP_REG_P (operands[1])"
  "*
{
  rtx xoperands[2];
  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
  output_asm_insn (\"movl\\t%1,%@\", xoperands);
  output_asm_insn (\"movl\\t%1,%-\", operands);
  return \"fmovd\\t%+,%0\";
}
")

;; FPA multiply and add.
(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(plus:DF (mult:DF (match_operand:DF 1 "general_operand" "%x,dmF,y")
			  (match_operand:DF 2 "general_operand" "xH,y,y"))
		 (match_operand:DF 3 "general_operand" "xH,y,dmF")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpmad\\t%1,%w2,%w3,%0\";
  return \"fpmad\\t%x1,%x2,%x3,%0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(plus:DF (match_operand:DF 1 "general_operand" "xH,y,dmF")
		 (mult:DF (match_operand:DF 2 "general_operand" "%x,dmF,y")
			  (match_operand:DF 3 "general_operand" "xH,y,y"))))]
   "TARGET_FPA"
   "*
{
  if (which_alternative == 0)
    return \"fpmad\\t%2,%w3,%w1,%0\";
  return \"fpmad\\t%x2,%x3,%x1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(plus:SF (mult:SF (match_operand:SF 1 "general_operand" "%x,ydmF,y")
			  (match_operand:SF 2 "general_operand" "xH,y,ydmF"))
		 (match_operand:SF 3 "general_operand" "xH,ydmF,ydmF")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpmas\\t%1,%w2,%w3,%0\";
  return \"fpmas\\t%1,%2,%3,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(plus:SF (match_operand:SF 1 "general_operand" "xH,ydmF,ydmF")
		 (mult:SF (match_operand:SF 2 "general_operand" "%x,ydmF,y")
			  (match_operand:SF 3 "general_operand" "xH,y,ydmF"))))]
   "TARGET_FPA"
   "*
{
  if (which_alternative == 0)
    return \"fpmas\\t%2,%w3,%w1,%0\";
  return \"fpmas\\t%2,%3,%1,%0\";
}")

;; FPA Multiply and subtract
(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(minus:DF (match_operand:DF 1 "general_operand" "xH,rmF,y")
		  (mult:DF (match_operand:DF 2 "register_operand" "%xH,y,y")
			   (match_operand:DF 3 "general_operand" "x,y,rmF"))))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpmsd\\t%3,%w2,%w1,%0\";
  return \"fpmsd\\t%x3,%2,%x1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(minus:SF (match_operand:SF 1 "general_operand" "xH,rmF,yrmF")
		  (mult:SF (match_operand:SF 2 "register_operand" "%xH,rmF,y")
			   (match_operand:SF 3 "general_operand" "x,y,yrmF"))))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpmss\\t%3,%w2,%w1,%0\";
  return \"fpmss\\t%3,%2,%1,%0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(minus:DF (mult:DF (match_operand:DF 1 "register_operand" "%xH,y,y")
			   (match_operand:DF 2 "general_operand" "x,y,rmF"))
		  (match_operand:DF 3 "general_operand" "xH,rmF,y")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpmrd\\t%2,%w1,%w3,%0\";
  return \"fpmrd\\t%x2,%1,%x3,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(minus:SF (mult:SF (match_operand:SF 1 "register_operand" "%xH,rmF,y")
			   (match_operand:SF 2 "general_operand" "x,y,yrmF"))
		  (match_operand:SF 3 "general_operand" "xH,rmF,yrmF")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpmrs\\t%2,%w1,%w3,%0\";
  return \"fpmrs\\t%x2,%1,%x3,%0\";
}")

;; FPA Add and multiply
(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(mult:DF (plus:DF (match_operand:DF 1 "register_operand" "%xH,y,y")
			  (match_operand:DF 2 "general_operand" "x,y,rmF"))
		 (match_operand:DF 3 "general_operand" "xH,rmF,y")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpamd\\t%2,%w1,%w3,%0\";
  return \"fpamd\\t%x2,%1,%x3,%0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(mult:DF (match_operand:DF 1 "general_operand" "xH,rmF,y")
		 (plus:DF (match_operand:DF 2 "register_operand" "%xH,y,y")
			  (match_operand:DF 3 "general_operand" "x,y,rmF"))))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpamd\\t%3,%w2,%w1,%0\";
  return \"fpamd\\t%x3,%2,%x1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(mult:SF (plus:SF (match_operand:SF 1 "register_operand" "%xH,rmF,y")
			  (match_operand:SF 2 "general_operand" "x,y,yrmF"))
		 (match_operand:SF 3 "general_operand" "xH,rmF,yrmF")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpams\\t%2,%w1,%w3,%0\";
  return \"fpams\\t%x2,%1,%x3,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(mult:SF (match_operand:SF 1 "general_operand" "xH,rmF,yrmF")
		 (plus:SF (match_operand:SF 2 "register_operand" "%xH,rmF,y")
			  (match_operand:SF 3 "general_operand" "x,y,yrmF"))))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpams\\t%3,%w2,%w1,%0\";
  return \"fpams\\t%x3,%2,%x1,%0\";
}")

;;FPA Subtract and multiply
(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(mult:DF (minus:DF (match_operand:DF 1 "register_operand" "xH,y,y")
			   (match_operand:DF 2 "general_operand" "x,y,rmF"))
		 (match_operand:DF 3 "general_operand" "xH,rmF,y")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpsmd\\t%2,%w1,%w3,%0\";
  return \"fpsmd\\t%x2,%1,%x3,%0\";
}")

(define_insn ""
  [(set (match_operand:DF 0 "register_operand" "=x,y,y")
	(mult:DF (match_operand:DF 1 "general_operand" "xH,rmF,y")
		 (minus:DF (match_operand:DF 2 "register_operand" "xH,y,y")
			   (match_operand:DF 3 "general_operand" "x,y,rmF"))))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpsmd\\t%3,%w2,%w1,%0\";
  return \"fpsmd\\t%x3,%2,%x1,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(mult:SF (minus:SF (match_operand:SF 1 "register_operand" "xH,rmF,y")
			   (match_operand:SF 2 "general_operand" "x,y,yrmF"))
		 (match_operand:SF 3 "general_operand" "xH,rmF,yrmF")))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpsms\\t%2,%w1,%w3,%0\";
  return \"fpsms\\t%x2,%1,%x3,%0\";
}")

(define_insn ""
  [(set (match_operand:SF 0 "register_operand" "=x,y,y")
	(mult:SF (match_operand:SF 1 "general_operand" "xH,rmF,yrmF")
		 (minus:SF (match_operand:SF 2 "register_operand" "xH,rmF,y")
			   (match_operand:SF 3 "general_operand" "x,y,yrmF"))))]
  "TARGET_FPA"
  "*
{
  if (which_alternative == 0)
    return \"fpsms\\t%3,%w2,%w1,%0\";
  return \"fpsms\\t%x3,%2,%x1,%0\";
}")


;;- Local variables:
;;- mode:emacs-lisp
;;- comment-start: ";;- "
;;- comment-start-skip: ";+- *"
;;- eval: (set-syntax-table (copy-sequence (syntax-table)))
;;- eval: (modify-syntax-entry ?[ "(]")
;;- eval: (modify-syntax-entry ?] ")[")
;;- eval: (modify-syntax-entry ?{ "(}")
;;- eval: (modify-syntax-entry ?} "){")
;;- End:
