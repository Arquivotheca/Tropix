/* Generated automatically by the program `genoutput'
from the machine description file `md'.	*/

#include "config.h"
#include "../h/rtl.h"
#include "../h/regs.h"
#include "../h/hard-reg-set.h"
#include "../h/real.h"
#include "../h/conditions.h"
#include "insn-flags.h"
#include "insn-config.h"

#ifndef __STDC__
#define const
#endif

#include "../h/output.h"
#include "aux-output.c"

#ifndef INSN_MACHINE_INFO
#define INSN_MACHINE_INFO struct dummy1 {int i;}
#endif


static char *
output_0 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[1]))
    return "fmovd\t%f1,%0";
  if (FPA_REG_P (operands[1]))
    return "fpmovd\t%1,%x0";
  return output_move_double (operands);
}
}

static char *
output_1 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return output_move_double (operands);
}
}

static char *
output_2 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (TARGET_68020 || ! ADDRESS_REG_P (operands[0]))
    return "tstl\t%0";
  /* If you think that the 68020 does not support tstl a0,
     reread page B-167 of the 68020 manual more carefully.  */
  /* On an address reg, cmpw may replace cmpl.  */
  return "cmpw\t%#0,%0";
}
}

static char *
output_3 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (TARGET_68020 || ! ADDRESS_REG_P (operands[0]))
    return "tstw\t%0";
  return "cmpw\t%#0,%0";
}
}

static char *
output_7 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    return "ftstx\t%0";
  return "ftsts\t%0";
}
}

static char *
output_10 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    return "ftstx\t%0";
  return "ftstd\t%0";
}
}

static char *
output_11 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
    return "cmpml\t%1,%0";
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED;
      return "cmpl\t%d0,%d1"; 
    }
  return "cmpl\t%d1,%d0";
}
}

static char *
output_12 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
    return "cmpmw\t%1,%0";
  if ((REG_P (operands[1]) && !ADDRESS_REG_P (operands[1]))
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED;
      return "cmpw\t%d0,%d1"; 
    }
  return "cmpw\t%d1,%d0";
}
}

static char *
output_13 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[0]) == MEM && GET_CODE (operands[1]) == MEM)
    return "cmpmb\t%1,%0";
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    { cc_status.flags |= CC_REVERSED;
      return "cmpb\t%d0,%d1";
    }
  return "cmpb\t%d1,%d0";
}
}

static char *
output_16 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags = CC_IN_68881;
  if (REG_P (operands[0]))
    {
      if (REG_P (operands[1]))
	return "fcmpx\t%1,%0";
      else
        return "fcmpd\t%f1,%0";
    }
  cc_status.flags |= CC_REVERSED;
  return "fcmpd\t%f0,%1";
}
}

static char *
output_19 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags = CC_IN_68881;
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return "fcmpx\t%1,%0";
      else
        return "fcmps\t%f1,%0";
    }
  cc_status.flags |= CC_REVERSED;
  return "fcmps\t%f0,%1";
}
}

static char *
output_20 (operands, insn)
		rtx *operands;
		rtx insn;
{
 { return output_btst (operands, operands[1], operands[0], insn, 7); }
}

static char *
output_21 (operands, insn)
		rtx *operands;
		rtx insn;
{
 { return output_btst (operands, operands[1], operands[0], insn, 31); }
}

static char *
output_22 (operands, insn)
		rtx *operands;
		rtx insn;
{
 { return output_btst (operands, operands[1], operands[0], insn, 7); }
}

static char *
output_23 (operands, insn)
		rtx *operands;
		rtx insn;
{
 { return output_btst (operands, operands[1], operands[0], insn, 31); }
}

static char *
output_24 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  operands[1] = gen_rtx (CONST_INT, VOIDmode, 7 - INTVAL (operands[1]));
  return output_btst (operands, operands[1], operands[0], insn, 7);
}
}

static char *
output_25 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
}
}

static char *
output_26 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
}
}

static char *
output_27 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags = CC_Z_IN_NOT_N | CC_NOT_NEGATIVE;
  return "tstb\t%0";
}
}

static char *
output_28 (operands, insn)
		rtx *operands;
		rtx insn;
{

{ register int log = exact_log2 (INTVAL (operands[1]));
  operands[1] = gen_rtx (CONST_INT, VOIDmode, log);
  return output_btst (operands, operands[1], operands[0], insn, 7);
}
}

static char *
output_29 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (operands[1] == const0_rtx)
    return "clrl\t%0";
  return "pea\t%a1";
}
}

static char *
output_30 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (ADDRESS_REG_P (operands[0]))
    return "subl\t%0,%0";
  /* moveq is faster on the 68000.  */
  if (DATA_REG_P (operands[0]) && !TARGET_68020)
    return "moveq\t%#0,%0";
  return "clrl\t%0";
}
}

static char *
output_31 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 3)
    return "fpmovl\t%x1,fpa0\n\tfpmovl\tfpa0,%x0";	
  if (FPA_REG_P (operands[1]) || FPA_REG_P (operands[0]))
    return "fpmovl\t%x1,%x0";
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
	return "clrl\t%0";
      else if (DATA_REG_P (operands[0])
	       && INTVAL (operands[1]) < 128
	       && INTVAL (operands[1]) >= -128)
        {
	  return "moveq\t%1,%0";
	}
      else if (ADDRESS_REG_P (operands[0])
	       && INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
	return "movw\t%1,%0";
      else if (push_operand (operands[0], SImode)
	       && INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
        return "pea\t%a1";
    }
  else if ((GET_CODE (operands[1]) == SYMBOL_REF
	    || GET_CODE (operands[1]) == CONST)
	   && push_operand (operands[0], SImode))
    return "pea\t%a1";
  else if ((GET_CODE (operands[1]) == SYMBOL_REF
	    || GET_CODE (operands[1]) == CONST)
	   && ADDRESS_REG_P (operands[0]))
    return "lea\t%a1,%0";
  return "movl\t%1,%0";
}
}

static char *
output_32 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
	return "clrw\t%0";
      else if (DATA_REG_P (operands[0])
	       && INTVAL (operands[1]) < 128
	       && INTVAL (operands[1]) >= -128)
        {
	  return "moveq\t%1,%0";
	}
      else if (INTVAL (operands[1]) < 0x8000
	       && INTVAL (operands[1]) >= -0x8000)
	return "movw\t%1,%0";
    }
  else if (CONSTANT_P (operands[1]))
    return "movl\t%1,%0";

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
      ASM_OUTPUT_INTERNAL_LABEL (asm_out_file, "LI",
				 CODE_LABEL_NUMBER (XEXP (labelref, 0)));
      /* For sake of 3b1, set flag saying we need to define the symbol
         LD%n (with value L%n-LI%n) at the end of the switch table.  */
      RTX_INTEGRATED_P (next_real_insn (XEXP (labelref, 0))) = 1;
    }

  return "movw\t%1,%0";
}
}

static char *
output_33 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
	return "clrw\t%0";
    }
  return "movw\t%1,%0";
}
}

static char *
output_34 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
      output_asm_insn ("subqw\t%#2,%3\n\tmovb	%1,%2", xoperands);
      return "movw\t%+,%0";
    }
  if (ADDRESS_REG_P (operands[1]) && GET_CODE (operands[0]) == MEM)
    {
      xoperands[0] = operands[0];
      xoperands[1] = operands[1];
      xoperands[2]
        = gen_rtx (MEM, QImode,
		   gen_rtx (PLUS, VOIDmode, stack_pointer_rtx, const1_rtx));
      xoperands[3] = stack_pointer_rtx;
      output_asm_insn ("movw\t%1,%-\n\tmovb	%2,%0\n\taddqw\t%#2,%3", xoperands);
      return "";
    }
  /* clr and st insns on 68000 read before writing.
     This isn't so on the 68010, but we have no alternative for it.  */
  if (TARGET_68020
      || !(GET_CODE (operands[0]) == MEM && MEM_VOLATILE_P (operands[0])))
    {
      if (operands[1] == const0_rtx)
	return "clrb\t%0";
      if (GET_CODE (operands[1]) == CONST_INT
	  && INTVAL (operands[1]) == -1)
	{
	  CC_STATUS_INIT;
	  return "st\t%0";
	}
    }
  if (GET_CODE (operands[1]) != CONST_INT && CONSTANT_P (operands[1]))
    return "movl\t%1,%0";
  if (ADDRESS_REG_P (operands[0]) || ADDRESS_REG_P (operands[1]))
    return "movw\t%1,%0";
  return "movb\t%1,%0";
}
}

static char *
output_35 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (operands[1] == const0_rtx
      /* clr insns on 68000 read before writing.
         This isn't so on the 68010, but we have no alternative for it.  */
      && (TARGET_68020
          || !(GET_CODE (operands[0]) == MEM && MEM_VOLATILE_P (operands[0]))))
    return "clrb\t%0";
  return "movb\t%1,%0";
}
}

static char *
output_36 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative >= 4)
    return "fpmovs %1,fpa0\n\tfpmovs fpa0,%0";
  if (FPA_REG_P (operands[0]))
    {
      if (FPA_REG_P (operands[1]))
	return "fpmovs\t%x1,%x0";
      else if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_single (operands);
      else if (FP_REG_P (operands[1]))
        return "fmovs\t%1,-(sp)\n\tfpmovd\t(sp)+,%0";
      return "fpmovs\t%x1,%x0";
    }
  if (FPA_REG_P (operands[1]))
    {
      if (FP_REG_P (operands[0]))
	return "fpmovs\t%x1,-(sp)\n\tfmovs\t(sp)+,%0";
      else
	return "fpmovs\t%x1,%x0";
    }
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return "fmovx\t%1,%0";
      else if (ADDRESS_REG_P (operands[1]))
	return "movl\t%1,%-\n\tfmovs\t%+,%0";
      else if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_single (operands);
      return "fmovs\t%f1,%0";
    }
  if (FP_REG_P (operands[1]))
    {
      if (ADDRESS_REG_P (operands[0]))
	return "fmovs\t%1,%-\n\tmovl\t%+,%0";
      return "fmovs %f1,%0";
    }
  return "movl\t%1,%0";
}
}

static char *
output_37 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 6)
    return "fpmovd\t%x1,fpa0\n\tfpmovd\tfpa0,%x0";
  if (FPA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      if (FP_REG_P (operands[1]))
        return "fmovd\t%1,-(sp)\n\tfpmovd\t(sp)+,%x0";
      return "fpmovd\t%x1,%x0";
    }
  else if (FPA_REG_P (operands[1]))
    {
      if (FP_REG_P(operands[0]))
        return "fpmovd\t%x1,-(sp)\n\tfmoved\t(sp)+,%0";
      else
        return "fpmovd\t%x1,%x0";
    }
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return "fmovx\t%1,%0";
      if (REG_P (operands[1]))
	{
	  rtx xoperands[2];
	  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
	  output_asm_insn ("movl\t%1,%-", xoperands);
	  output_asm_insn ("movl\t%1,%-", operands);
	  return "fmovd\t%+,%0";
	}
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      return "fmovd\t%f1,%0";
    }
  else if (FP_REG_P (operands[1]))
    {
      if (REG_P (operands[0]))
	{
	  output_asm_insn ("fmovd\t%f1,%-\n\tmovl\t%+,%0", operands);
	  operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
	  return "movl\t%+,%0";
	}
      else
        return "fmovd\t%f1,%0";
    }
  return output_move_double (operands);
}

}

static char *
output_38 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 8)
    return "fpmovd\t%x1,fpa0\n\tfpmovd\tfpa0,%x0";
  if (FPA_REG_P (operands[0]) || FPA_REG_P (operands[1]))
    return "fpmovd\t%x1,%x0";
  if (FP_REG_P (operands[0]))
    {
      if (FP_REG_P (operands[1]))
	return "fmovx\t%1,%0";
      if (REG_P (operands[1]))
	{
	  rtx xoperands[2];
	  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
	  output_asm_insn ("movl\t%1,%-", xoperands);
	  output_asm_insn ("movl\t%1,%-", operands);
	  return "fmovd\t%+,%0";
	}
      if (GET_CODE (operands[1]) == CONST_DOUBLE)
	return output_move_const_double (operands);
      return "fmovd\t%f1,%0";
    }
  else if (FP_REG_P (operands[1]))
    {
      if (REG_P (operands[0]))
	{
	  output_asm_insn ("fmovd\t%f1,%-\n\tmovl\t%+,%0", operands);
	  operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
	  return "movl\t%+,%0";
	}
      else
        return "fmovd\t%f1,%0";
    }
  return output_move_double (operands);
}

}

static char *
output_40 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[0]) == REG)
    {
      /* Must clear condition codes, since the mov.l bases them on
	 the entire 32 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return "movl\t%1,%0";
    }
  if (GET_CODE (operands[1]) == MEM)
    operands[1] = adj_offsettable_operand (operands[1], 3);
  return "movb\t%1,%0";
}
}

static char *
output_41 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[0]) == REG
      && (GET_CODE (operands[1]) == MEM
	  || GET_CODE (operands[1]) == CONST_INT))
    {
      /* Must clear condition codes, since the mov.w bases them on
	 the entire 16 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return "movw\t%1,%0";
    }
  if (GET_CODE (operands[0]) == REG)
    {
      /* Must clear condition codes, since the mov.l bases them on
	 the entire 32 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return "movl\t%1,%0";
    }
  if (GET_CODE (operands[1]) == MEM)
    operands[1] = adj_offsettable_operand (operands[1], 1);
  return "movb\t%1,%0";
}
}

static char *
output_42 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[0]) == REG)
    {
      /* Must clear condition codes, since the mov.l bases them on
	 the entire 32 bits, not just the desired 8 bits.  */
      CC_STATUS_INIT;
      return "movl\t%1,%0";
    }
  if (GET_CODE (operands[1]) == MEM)
    operands[1] = adj_offsettable_operand (operands[1], 2);
  return "movw\t%1,%0";
}
}

static char *
output_46 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return "andl\t%#0xFFFF,%0";
      if (reg_mentioned_p (operands[0], operands[1]))
        return "movw\t%1,%0\n\tandl\t%#0xFFFF,%0";
      return "clrl\t%0\n\tmovw\t%1,%0";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    return "movw\t%1,%0\n\tclrw\t%0";
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    return "clrw\t%0\n\tmovw\t%1,%0";
  else
    {
      output_asm_insn ("clrw\t%0", operands);
      operands[0] = adj_offsettable_operand (operands[0], 2);
      return "movw\t%1,%0";
    }
}
}

static char *
output_47 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return "andw\t%#0xFF,%0";
      if (reg_mentioned_p (operands[0], operands[1]))
        return "movb\t%1,%0\n\tandw\t%#0xFF,%0";
      return "clrw\t%0\n\tmovb\t%1,%0";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    {
      if (REGNO (XEXP (XEXP (operands[0], 0), 0)) == STACK_POINTER_REGNUM)
	{
	  output_asm_insn ("clrw\t%-", operands);
	  operands[0] = gen_rtx (MEM, GET_MODE (operands[0]),
				 plus_constant (stack_pointer_rtx, 1));
	  return "movb\t%1,%0";
	}
      else
	return "movb\t%1,%0\n\tclrb\t%0";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    return "clrb\t%0\n\tmovb\t%1,%0";
  else
    {
      output_asm_insn ("clrb\t%0", operands);
      operands[0] = adj_offsettable_operand (operands[0], 1);
      return "movb\t%1,%0";
    }
}
}

static char *
output_48 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (DATA_REG_P (operands[0]))
    {
      if (GET_CODE (operands[1]) == REG
	  && REGNO (operands[0]) == REGNO (operands[1]))
	return "andl\t%#0xFF,%0";
      if (reg_mentioned_p (operands[0], operands[1]))
        return "movb\t%1,%0\n\tandl\t%#0xFF,%0";
      return "clrl\t%0\n\tmovb\t%1,%0";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == PRE_DEC)
    {
      operands[0] = XEXP (XEXP (operands[0], 0), 0);
      return "clrl\t-(%0)\n\tmoveb\t%1,3(%0)";
    }
  else if (GET_CODE (operands[0]) == MEM
	   && GET_CODE (XEXP (operands[0], 0)) == POST_INC)
    {
      operands[0] = XEXP (XEXP (operands[0], 0), 0);
      return "clrl\t(%0)+\n\tmoveb\t%1,-1(%0)";
    }
  else
    {
      output_asm_insn ("clrl\t%0", operands);
      operands[0] = adj_offsettable_operand (operands[0], 3);
      return "movb\t%1,%0";
    }
}
}

static char *
output_49 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (ADDRESS_REG_P (operands[0]))
    return "movw\t%1,%0";
  return "extl\t%0";
}
}

static char *
output_54 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[0]) && FP_REG_P (operands[1]))
    {
      if (REGNO (operands[0]) == REGNO (operands[1]))
	{
	  /* Extending float to double in an fp-reg is a no-op.
	     NOTICE_UPDATE_CC has already assumed that the
	     cc will be set.  So cancel what it did.  */
	  cc_status = cc_prev_status;
	  return "";
	}
      return "fmovx\t%1,%0";
    }
  if (FP_REG_P (operands[0]))
    return "fmovs\t%f1,%0";
  if (DATA_REG_P (operands[0]) && FP_REG_P (operands[1]))
    {
      output_asm_insn ("fmovd\t%f1,%-\n\tmovl\t%+,%0", operands);
      operands[0] = gen_rtx (REG, SImode, REGNO (operands[0]) + 1);
      return "movl\t%+,%0";
    }
  return "fmovd\t%f1,%0";
}
}

static char *
output_68 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[1]))
    return "fintrzx\t%f1,%0";
  return "fintrzd\t%f1,%0";
}
}

static char *
output_69 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[1]))
    return "fintrzx\t%f1,%0";
  return "fintrzs\t%f1,%0";
}
}

static char *
output_78 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
        return "movl\t%2,%0\n\taddl\t%1,%0";
      if (GET_CODE (operands[2]) == REG)
	return "lea\t0(%1,%2),%0";
      else
	return "lea\t%c2(%1),%0";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
#ifndef NO_ADDSUB_Q
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return (ADDRESS_REG_P (operands[0])
		? "addqw\t%2,%0"
		: "addql\t%2,%0");
      if (INTVAL (operands[2]) < 0
	  && INTVAL (operands[2]) >= -8)
        {
	  operands[2] = gen_rtx (CONST_INT, VOIDmode,
			         - INTVAL (operands[2]));
	  return (ADDRESS_REG_P (operands[0])
		  ? "subqw\t%2,%0"
		  : "subql\t%2,%0");
	}
#endif
      if (ADDRESS_REG_P (operands[0])
	  && INTVAL (operands[2]) >= -0x8000
	  && INTVAL (operands[2]) < 0x8000)
	return "addw\t%2,%0";
    }
  return "addl\t%2,%0";
}
}

static char *
output_80 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
#ifndef NO_ADDSUB_Q
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return "addqw\t%2,%0";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) < 0
	  && INTVAL (operands[2]) >= -8)
	{
	  operands[2] = gen_rtx (CONST_INT, VOIDmode,
			         - INTVAL (operands[2]));
	  return "subqw\t%2,%0";
	}
    }
#endif
  return "addw\t%2,%0";
}
}

static char *
output_82 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
#ifndef NO_ADDSUB_Q
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return "addqb\t%2,%0";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
      if (INTVAL (operands[2]) < 0 && INTVAL (operands[2]) >= -8)
       {
	 operands[2] = gen_rtx (CONST_INT, VOIDmode, - INTVAL (operands[2]));
	 return "subqb\t%2,%0";
       }
    }
#endif
  return "addb\t%2,%0";
}
}

static char *
output_85 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpaddd\t%y2,%0";
  if (rtx_equal_p (operands[0], operands[2]))
    return "fpaddd\t%y1,%0";
  if (which_alternative == 0)
    return "fpadd3d\t%w2,%w1,%0";
  return "fpadd3d\t%x2,%x1,%0";
}
}

static char *
output_86 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return "faddx\t%2,%0";
  return "faddd\t%f2,%0";
}
}

static char *
output_88 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpadds\t%w2,%0";
  if (rtx_equal_p (operands[0], operands[2]))
    return "fpadds\t%w1,%0";
  if (which_alternative == 0)
    return "fpadd3s\t%w2,%w1,%0";
  return "fpadd3s\t%2,%1,%0";
}
}

static char *
output_89 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return "faddx\t%2,%0";
  return "fadds\t%f2,%0";
}
}

static char *
output_90 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
		return "subql\t%1,%0\n\tnegl\t%0";
	    }
#endif
	  return "subl\t%1,%0\n\tnegl\t%0";
	}
      /* This case is matched by J, but negating -0x8000
         in an lea would give an invalid displacement.
	 So do this specially.  */
      if (INTVAL (operands[2]) == -0x8000)
	return "movl\t%1,%0\n\tsubl\t%2,%0";
      return "lea\t%n2(%1),%0";
    }
  if (GET_CODE (operands[2]) == CONST_INT)
    {
#ifndef NO_ADDSUB_Q
      if (INTVAL (operands[2]) > 0
	  && INTVAL (operands[2]) <= 8)
	return "subql\t%2,%0";
#endif
      if (ADDRESS_REG_P (operands[0])
	  && INTVAL (operands[2]) >= -0x8000
	  && INTVAL (operands[2]) < 0x8000)
	return "subw\t%2,%0";
    }
  return "subl\t%2,%0";
}
}

static char *
output_97 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[0], operands[2]))
    return "fprsubd\t%y1,%0";
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpsubd\t%y2,%0";
  if (which_alternative == 0)
    return "fpsub3d\t%w2,%w1,%0";
  return "fpsub3d\t%x2,%x1,%0";
}
}

static char *
output_98 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return "fsubx\t%2,%0";
  return "fsubd\t%f2,%0";
}
}

static char *
output_100 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[0], operands[2]))
    return "fprsubs\t%w1,%0";
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpsubs\t%w2,%0";
  if (which_alternative == 0)
    return "fpsub3s\t%w2,%w1,%0";
  return "fpsub3s\t%2,%1,%0";
}
}

static char *
output_101 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return "fsubx\t%2,%0";
  return "fsubs\t%f2,%0";
}
}

static char *
output_102 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "muls\t%2,%0";
}
}

static char *
output_103 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "muls\t%2,%0";
}
}

static char *
output_105 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "mulu\t%2,%0";
}
}

static char *
output_106 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "mulu\t%2,%0";
}
}

static char *
output_109 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[1], operands[2]))
    return "fpsqrd\t%y1,%0";
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpmuld\t%y2,%0";
  if (rtx_equal_p (operands[0], operands[2]))
    return "fpmuld\t%y1,%0";
  if (which_alternative == 0)
    return "fpmul3d\t%w2,%w1,%0"; 
  return "fpmul3d\t%x2,%x1,%0";
}
}

static char *
output_110 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return "fmulx\t%2,%0";
  return "fmuld\t%f2,%0";
}
}

static char *
output_112 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[1], operands[2]))
    return "fpsqrs\t%w1,%0";
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpmuls\t%w2,%0";
  if (rtx_equal_p (operands[0], operands[2]))
    return "fpmuls\t%w1,%0";
  if (which_alternative == 0)
    return "fpmul3s\t%w2,%w1,%0";
  return "fpmul3s\t%2,%1,%0";
}
}

static char *
output_113 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return "fsglmulx\t%2,%0";
  return "fsglmuls\t%f2,%0";
}
}

static char *
output_114 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "extl\t%0\n\tdivs\t%2,%0";
}
}

static char *
output_115 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "divs\t%2,%0";
}
}

static char *
output_117 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "andl\t%#0xFFFF,%0\n\tdivu\t%2,%0";
}
}

static char *
output_118 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return "divu\t%2,%0";
}
}

static char *
output_121 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[0], operands[2]))
    return "fprdivd\t%y1,%0";
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpdivd\t%y2,%0";
  if (which_alternative == 0)
    return "fpdiv3d\t%w2,%w1,%0";
  return "fpdiv3d\t%x2,%x1,%x0";
}
}

static char *
output_122 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return "fdivx\t%2,%0";
  return "fdivd\t%f2,%0";
}
}

static char *
output_124 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (rtx_equal_p (operands[0], operands[1]))
    return "fpdivs\t%w2,%0";
  if (rtx_equal_p (operands[0], operands[2]))
    return "fprdivs\t%w1,%0";
  if (which_alternative == 0)
    return "fpdiv3s\t%w2,%w1,%0";
  return "fpdiv3s\t%2,%1,%0";
}
}

static char *
output_125 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]) && ! DATA_REG_P (operands[2]))
    return "fsgldivx\t%2,%0";
  return "fsgldivs\t%f2,%0";
}
}

static char *
output_126 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return "extl %0\n\tdivs %2,%0\n\tswap\t%0";
}
}

static char *
output_127 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return "divs\t%2,%0\n\tswap\t%0";
}
}

static char *
output_128 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return "andl\t%#0xFFFF,%0\n\tdivu\t%2,%0\n\tswap\t%0";
}
}

static char *
output_129 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  /* The swap insn produces cc's that don't correspond to the result.  */
  CC_STATUS_INIT;
  return "divu\t%2,%0\n\tswap\t%0";
}
}

static char *
output_132 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
        return "clrw\t%0";
      return "andw\t%2,%0";
    }
  return "andl\t%2,%0";
}
}

static char *
output_137 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
      return "orw\t%2,%0";
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
      return "bset\t%1,%0";
    }
  return "orl\t%2,%0";
}
}

static char *
output_140 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[2]) >> 16 == 0
      && (offsettable_memref_p (operands[0]) || DATA_REG_P (operands[0])))
    { 
      if (! DATA_REG_P (operands[0]))
	operands[0] = adj_offsettable_operand (operands[0], 2);
      /* Do not delete a following tstl %0 insn; that would be incorrect.  */
      CC_STATUS_INIT;
      return "eorw\t%2,%0";
    }
  return "eorl\t%2,%0";
}
}

static char *
output_148 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return "fnegx\t%1,%0";
  return "fnegs\t%f1,%0";
}
}

static char *
output_151 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return "fnegx\t%1,%0";
  return "fnegd\t%f1,%0";
}
}

static char *
output_154 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return "fabsx\t%1,%0";
  return "fabss\t%f1,%0";
}
}

static char *
output_157 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1]) && ! DATA_REG_P (operands[1]))
    return "fabsx\t%1,%0";
  return "fabsd\t%f1,%0";
}
}

static char *
output_161 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (TARGET_68020)
    return "movb\t%1,%0\n\textbl\t%0";
  return "movb\t%1,%0\n\textw\t%0\n\textl\t%0";
}
}

static char *
output_162 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (reg_mentioned_p (operands[0], operands[1]))
    return "movb\t%1,%0\n\tandl\t%#0xFF,%0";
  return "clrl\t%0\n\tmovb\t%1,%0";
}
}

static char *
output_163 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status.flags |= CC_REVERSED;
  return "cmpb\t%0,%1";

}

static char *
output_164 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "cmpb\t%1,%0";

}

static char *
output_165 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status.flags |= CC_REVERSED;
  return "cmpb\t%0,%1";

}

static char *
output_166 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "cmpb\t%1,%0";

}

static char *
output_185 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[0]))
    {
      if (INTVAL (operands[1]) + INTVAL (operands[2]) != 32)
        return "bfins\t%3,%0{%b2:%b1}";
    }
  else
    operands[0]
      = adj_offsettable_operand (operands[0], INTVAL (operands[2]) / 8);

  if (GET_CODE (operands[3]) == MEM)
    operands[3] = adj_offsettable_operand (operands[3],
					   (32 - INTVAL (operands[1])) / 8);
  if (INTVAL (operands[1]) == 8)
    return "movb\t%3,%0";
  return "movw\t%3,%0";
}
}

static char *
output_186 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags |= CC_NOT_NEGATIVE;
  if (REG_P (operands[1]))
    {
      if (INTVAL (operands[2]) + INTVAL (operands[3]) != 32)
	return "bfextu\t%1{%b3:%b2},%0";
    }
  else
    operands[1]
      = adj_offsettable_operand (operands[1], INTVAL (operands[3]) / 8);

  output_asm_insn ("clrl\t%0", operands);
  if (GET_CODE (operands[0]) == MEM)
    operands[0] = adj_offsettable_operand (operands[0],
					   (32 - INTVAL (operands[1])) / 8);
  if (INTVAL (operands[2]) == 8)
    return "movb\t%1,%0";
  return "movw\t%1,%0";
}
}

static char *
output_187 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1]))
    {
      if (INTVAL (operands[2]) + INTVAL (operands[3]) != 32)
	return "bfexts\t%1{%b3:%b2},%0";
    }
  else
    operands[1]
      = adj_offsettable_operand (operands[1], INTVAL (operands[3]) / 8);

  if (INTVAL (operands[2]) == 8)
    return "movb\t%1,%0\n\textbl\t%0";
  return "movw\t%1,%0\n\textl\t%0";
}
}

static char *
output_189 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags |= CC_NOT_NEGATIVE;
  return "bfextu\t%1{%b3:%b2},%0";
}
}

static char *
output_190 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  return "bfchg\t%0{%b2:%b1}";
}
}

static char *
output_191 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  return "bfclr\t%0{%b2:%b1}";
}
}

static char *
output_192 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  return "bfset\t%0{%b2:%b1}";
}
}

static char *
output_195 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  cc_status.flags |= CC_NOT_NEGATIVE;
  return "bfextu\t%1{%b3:%b2},%0";
}
}

static char *
output_196 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  return "bfclr\t%0{%b2:%b1}";
}
}

static char *
output_197 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  return "bfset\t%0{%b2:%b1}";
}
}

static char *
output_198 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
#if 0	/* --------------------------------------------------- */
  /* These special cases are now recognized by a specific pattern.  */
  if (GET_CODE (operands[1]) == CONST_INT && GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[1]) == 16 && INTVAL (operands[2]) == 16)
    return "movw\t%3,%0";
  if (GET_CODE (operands[1]) == CONST_INT && GET_CODE (operands[2]) == CONST_INT
      && INTVAL (operands[1]) == 24 && INTVAL (operands[2]) == 8)
    return "movb\t%3,%0";
#endif	/* --------------------------------------------------- */
  return "bfins\t%3,%0{%b2:%b1}";
}
}

static char *
output_199 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
  return "bftst\t%0{%b2:%b1}";
}
}

static char *
output_200 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
  return "bftst\t%0{%b2:%b1}";
}
}

static char *
output_201 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
  return "bftst\t%0{%b2:%b1}";
}
}

static char *
output_202 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
  return "bftst\t%0{%b2:%b1}";
}
}

static char *
output_203 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
  return "bftst\t%0{%b2:%b1}";
}
}

static char *
output_204 (operands, insn)
		rtx *operands;
		rtx insn;
{

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
  return "bftst\t%0{%b2:%b1}";
}
}

static char *
output_205 (operands, insn)
		rtx *operands;
		rtx insn;
{

  cc_status = cc_prev_status;
  OUTPUT_JUMP ("seq\t%0", "fseq\t%0", "seq\t%0");

}

static char *
output_206 (operands, insn)
		rtx *operands;
		rtx insn;
{

  cc_status = cc_prev_status;
  OUTPUT_JUMP ("sne\t%0", "fsne\t%0", "sne\t%0");

}

static char *
output_207 (operands, insn)
		rtx *operands;
		rtx insn;
{

  cc_status = cc_prev_status;
  OUTPUT_JUMP ("sgt\t%0", "fsgt\t%0", 0);

}

static char *
output_208 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status = cc_prev_status;
     return "shi\t%0"; 
}

static char *
output_209 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status = cc_prev_status;
     OUTPUT_JUMP ("slt\t%0", "fslt\t%0", "smi\t%0"); 
}

static char *
output_210 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status = cc_prev_status;
     return "scs\t%0"; 
}

static char *
output_211 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status = cc_prev_status;
     OUTPUT_JUMP ("sge\t%0", "fsge\t%0", "spl\t%0"); 
}

static char *
output_212 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status = cc_prev_status;
     return "scc\t%0"; 
}

static char *
output_213 (operands, insn)
		rtx *operands;
		rtx insn;
{

  cc_status = cc_prev_status;
  OUTPUT_JUMP ("sle\t%0", "fsle\t%0", 0);

}

static char *
output_214 (operands, insn)
		rtx *operands;
		rtx insn;
{
 cc_status = cc_prev_status;
     return "sls\t%0"; 
}

static char *
output_215 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  OUTPUT_JUMP ("jeq\t%l0", "fjeq\t%l0", "jeq\t%l0");
}
}

static char *
output_216 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  OUTPUT_JUMP ("jne\t%l0", "fjne\t%l0", "jne\t%l0");
}
}

static char *
output_217 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jgt\t%l0", "fjgt\t%l0", 0);

}

static char *
output_218 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jhi\t%l0";

}

static char *
output_219 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jlt\t%l0", "fjlt\t%l0", "jmi\t%l0");

}

static char *
output_220 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jcs\t%l0";

}

static char *
output_221 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jge\t%l0", "fjge\t%l0", "jpl\t%l0");

}

static char *
output_222 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jcc\t%l0";

}

static char *
output_223 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jle\t%l0", "fjle\t%l0", 0);

}

static char *
output_224 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jls\t%l0";

}

static char *
output_225 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  OUTPUT_JUMP ("jne\t%l0", "fjne\t%l0", "jne\t%l0");
}
}

static char *
output_226 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  OUTPUT_JUMP ("jeq\t%l0", "fjeq\t%l0", "jeq\t%l0");
}
}

static char *
output_227 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jle\t%l0", "fjngt\t%l0", 0);

}

static char *
output_228 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jls\t%l0";

}

static char *
output_229 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jge\t%l0", "fjnlt\t%l0", "jpl\t%l0");

}

static char *
output_230 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jcc\t%l0";

}

static char *
output_231 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jlt\t%l0", "fjnge\t%l0", "jmi\t%l0");

}

static char *
output_232 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jcs\t%l0";

}

static char *
output_233 (operands, insn)
		rtx *operands;
		rtx insn;
{

  OUTPUT_JUMP ("jgt\t%l0", "fjnle\t%l0", 0);

}

static char *
output_234 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jhi\t%l0";

}

static char *
output_238 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
#ifdef ASM_RETURN_CASE_JUMP
 ASM_RETURN_CASE_JUMP;
#else
  return "jmp\t2(pc,%0:w)";
#endif /* no ASM_RETURN_CASE_JUMP */
}
}

static char *
output_239 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jra\t%l0";

}

static char *
output_240 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  if (DATA_REG_P (operands[0]))
    return "dbra\t%0,%l1";
  if (GET_CODE (operands[0]) == MEM)
    {
      return "subqw\t%#1,%0\n\tjcc\t%l1";
    }
  return "subqw\t%#1,%0\n\tcmpw\t%#-1,%0\n\tjne\t%l1";
}
}

static char *
output_241 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  if (DATA_REG_P (operands[0]))
    return "dbra\t%0,%l1\n\tclrw\t%0\n\tsubql\t%#1,%0\n\tjcc\t%l1";
  if (GET_CODE (operands[0]) == MEM)
    return "subql\t%#1,%0\n\tjcc\t%l1";
  return "subql\t%#1,%0\n\tcmpl\t%#-1,%0\n\tjne\t%l1";
}
}

static char *
output_242 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  if (DATA_REG_P (operands[0]))
    return "dbra\t%0,%l1\n\tclrw\t%0\n\tsubql\t%#1,%0\n\tjcc\t%l1";
  if (GET_CODE (operands[0]) == MEM)
    return "subql\t%#1,%0\n\tjcc\t%l1";
  return "subql\t%#1,%0\n\tcmpl\t%#-1,%0\n\tjne\t%l1";
}
}

static char *
output_243 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jbsr\t%0";

}

static char *
output_244 (operands, insn)
		rtx *operands;
		rtx insn;
{

  return "jbsr\t%1";

}

static char *
output_247 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  rtx xoperands[2];
  xoperands[1] = gen_rtx (REG, SImode, REGNO (operands[1]) + 1);
  output_asm_insn ("movl\t%1,%@", xoperands);
  output_asm_insn ("movl\t%1,%-", operands);
  return "fmovd\t%+,%0";
}

}

static char *
output_248 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmad\t%1,%w2,%w3,%0";
  return "fpmad\t%x1,%x2,%x3,%0";
}
}

static char *
output_249 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmad\t%2,%w3,%w1,%0";
  return "fpmad\t%x2,%x3,%x1,%0";
}
}

static char *
output_250 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmas\t%1,%w2,%w3,%0";
  return "fpmas\t%1,%2,%3,%0";
}
}

static char *
output_251 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmas\t%2,%w3,%w1,%0";
  return "fpmas\t%2,%3,%1,%0";
}
}

static char *
output_252 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmsd\t%3,%w2,%w1,%0";
  return "fpmsd\t%x3,%2,%x1,%0";
}
}

static char *
output_253 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmss\t%3,%w2,%w1,%0";
  return "fpmss\t%3,%2,%1,%0";
}
}

static char *
output_254 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmrd\t%2,%w1,%w3,%0";
  return "fpmrd\t%x2,%1,%x3,%0";
}
}

static char *
output_255 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpmrs\t%2,%w1,%w3,%0";
  return "fpmrs\t%x2,%1,%x3,%0";
}
}

static char *
output_256 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpamd\t%2,%w1,%w3,%0";
  return "fpamd\t%x2,%1,%x3,%0";
}
}

static char *
output_257 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpamd\t%3,%w2,%w1,%0";
  return "fpamd\t%x3,%2,%x1,%0";
}
}

static char *
output_258 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpams\t%2,%w1,%w3,%0";
  return "fpams\t%x2,%1,%x3,%0";
}
}

static char *
output_259 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpams\t%3,%w2,%w1,%0";
  return "fpams\t%x3,%2,%x1,%0";
}
}

static char *
output_260 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpsmd\t%2,%w1,%w3,%0";
  return "fpsmd\t%x2,%1,%x3,%0";
}
}

static char *
output_261 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpsmd\t%3,%w2,%w1,%0";
  return "fpsmd\t%x3,%2,%x1,%0";
}
}

static char *
output_262 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpsms\t%2,%w1,%w3,%0";
  return "fpsms\t%x2,%1,%x3,%0";
}
}

static char *
output_263 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (which_alternative == 0)
    return "fpsms\t%3,%w2,%w1,%0";
  return "fpsms\t%x3,%2,%x1,%0";
}
}

char * const insn_template[] =
	{
		0,
		0,
		0,
		0,
		"tstb\t%0",
		0,
		"fptsts\t%x0\n\tfpmove\tfpastatus,%1\n\tmovw\t%1,cc",
		0,
		0,
		"fptstd\t%x0\n\tfpmove\tfpastatus,%1\n\tmovw\t%1,cc",
		0,
		0,
		0,
		0,
		0,
		"fpcmpd\t%y1,%0\n\tfpmove\tfpastatus,%2\n\tmovw\t%2,cc",
		0,
		0,
		"fpcmps\t%w1,%x0\n\tfpmove\tfpastatus,%2\n\tmovw\t%2,cc",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"pea\t%a1",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"extw\t%0",
		"extbl\t%0",
		0,
		"fpstod\t%w1,%0",
		0,
		0,
		"fpdtos\t%y1,%0",
		"fmovs\t%f1,%0",
		0,
		"fpltos\t%1,%0",
		"fmovl\t%1,%0",
		0,
		"fpltod\t%1,%0",
		"fmovl\t%1,%0",
		"fmovw\t%1,%0",
		"fmovw\t%1,%0",
		"fmovb\t%1,%0",
		"fmovb\t%1,%0",
		0,
		0,
		"fmovb\t%1,%0",
		"fmovw\t%1,%0",
		"fmovl\t%1,%0",
		"fmovb\t%1,%0",
		"fmovw\t%1,%0",
		"fmovl\t%1,%0",
		"fpstol\t%w1,%0",
		"fpdtol\t%y1,%0",
		0,
		"addw\t%2,%0",
		0,
		"addw\t%1,%0",
		0,
		"addb\t%1,%0",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"subw\t%2,%0",
		"subw\t%2,%0",
		"subw\t%1,%0",
		"subb\t%2,%0",
		"subb\t%1,%0",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"mulsl\t%2,%0",
		0,
		0,
		"mulul\t%2,%0",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"divsl\t%2,%0",
		0,
		0,
		"divul\t%2,%0",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"divsll\t%2,%3:%0",
		"divull\t%2,%3:%0",
		0,
		"andw\t%2,%0",
		"andb\t%2,%0",
		"andw\t%1,%0",
		"andb\t%1,%0",
		0,
		"orw\t%2,%0",
		"orb\t%2,%0",
		0,
		"eorw\t%2,%0",
		"eorb\t%2,%0",
		"negl\t%0",
		"negw\t%0",
		"negb\t%0",
		0,
		"fpnegs\t%w1,%0",
		0,
		0,
		"fpnegd\t%y1,%0",
		0,
		0,
		"fpabss\t%y1,%0",
		0,
		0,
		"fpabsd\t%y1,%0",
		0,
		"notl\t%0",
		"notw\t%0",
		"notb\t%0",
		0,
		0,
		0,
		0,
		0,
		0,
		"asll\t%2,%0",
		"aslw\t%2,%0",
		"aslb\t%2,%0",
		"asrl\t%2,%0",
		"asrw\t%2,%0",
		"asrb\t%2,%0",
		"lsll\t%2,%0",
		"lslw\t%2,%0",
		"lslb\t%2,%0",
		"lsrl\t%2,%0",
		"lsrw\t%2,%0",
		"lsrb\t%2,%0",
		"roll\t%2,%0",
		"rolw\t%2,%0",
		"rolb\t%2,%0",
		"rorl\t%2,%0",
		"rorw\t%2,%0",
		"rorb\t%2,%0",
		0,
		0,
		0,
		"bfexts\t%1{%b3:%b2},%0",
		0,
		0,
		0,
		0,
		"bfins\t%3,%0{%b2:%b1}",
		"bfexts\t%1{%b3:%b2},%0",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"nop",
		"lea\t%a1,%0",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	};

char *(*const insn_outfun[])() =
	{
		output_0,
		output_1,
		output_2,
		output_3,
		0,
		0,
		0,
		output_7,
		0,
		0,
		output_10,
		output_11,
		output_12,
		output_13,
		0,
		0,
		output_16,
		0,
		0,
		output_19,
		output_20,
		output_21,
		output_22,
		output_23,
		output_24,
		output_25,
		output_26,
		output_27,
		output_28,
		output_29,
		output_30,
		output_31,
		output_32,
		output_33,
		output_34,
		output_35,
		output_36,
		output_37,
		output_38,
		0,
		output_40,
		output_41,
		output_42,
		0,
		0,
		0,
		output_46,
		output_47,
		output_48,
		output_49,
		0,
		0,
		0,
		0,
		output_54,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		output_68,
		output_69,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		output_78,
		0,
		output_80,
		0,
		output_82,
		0,
		0,
		output_85,
		output_86,
		0,
		output_88,
		output_89,
		output_90,
		0,
		0,
		0,
		0,
		0,
		0,
		output_97,
		output_98,
		0,
		output_100,
		output_101,
		output_102,
		output_103,
		0,
		output_105,
		output_106,
		0,
		0,
		output_109,
		output_110,
		0,
		output_112,
		output_113,
		output_114,
		output_115,
		0,
		output_117,
		output_118,
		0,
		0,
		output_121,
		output_122,
		0,
		output_124,
		output_125,
		output_126,
		output_127,
		output_128,
		output_129,
		0,
		0,
		output_132,
		0,
		0,
		0,
		0,
		output_137,
		0,
		0,
		output_140,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		output_148,
		0,
		0,
		output_151,
		0,
		0,
		output_154,
		0,
		0,
		output_157,
		0,
		0,
		0,
		output_161,
		output_162,
		output_163,
		output_164,
		output_165,
		output_166,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		output_185,
		output_186,
		output_187,
		0,
		output_189,
		output_190,
		output_191,
		output_192,
		0,
		0,
		output_195,
		output_196,
		output_197,
		output_198,
		output_199,
		output_200,
		output_201,
		output_202,
		output_203,
		output_204,
		output_205,
		output_206,
		output_207,
		output_208,
		output_209,
		output_210,
		output_211,
		output_212,
		output_213,
		output_214,
		output_215,
		output_216,
		output_217,
		output_218,
		output_219,
		output_220,
		output_221,
		output_222,
		output_223,
		output_224,
		output_225,
		output_226,
		output_227,
		output_228,
		output_229,
		output_230,
		output_231,
		output_232,
		output_233,
		output_234,
		0,
		0,
		0,
		output_238,
		output_239,
		output_240,
		output_241,
		output_242,
		output_243,
		output_244,
		0,
		0,
		output_247,
		output_248,
		output_249,
		output_250,
		output_251,
		output_252,
		output_253,
		output_254,
		output_255,
		output_256,
		output_257,
		output_258,
		output_259,
		output_260,
		output_261,
		output_262,
		output_263,
	};

rtx (*const insn_gen_function[]) () =
	{
		0,
		0,
		gen_tstsi,
		gen_tsthi,
		gen_tstqi,
		gen_tstsf,
		0,
		0,
		gen_tstdf,
		0,
		0,
		gen_cmpsi,
		gen_cmphi,
		gen_cmpqi,
		gen_cmpdf,
		0,
		0,
		gen_cmpsf,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		gen_movsi,
		gen_movhi,
		gen_movstricthi,
		gen_movqi,
		gen_movstrictqi,
		gen_movsf,
		gen_movdf,
		gen_movdi,
		gen_pushasi,
		gen_truncsiqi2,
		gen_trunchiqi2,
		gen_truncsihi2,
		gen_zero_extendhisi2,
		gen_zero_extendqihi2,
		gen_zero_extendqisi2,
		0,
		0,
		0,
		gen_extendhisi2,
		gen_extendqihi2,
		gen_extendqisi2,
		gen_extendsfdf2,
		0,
		0,
		gen_truncdfsf2,
		0,
		0,
		gen_floatsisf2,
		0,
		0,
		gen_floatsidf2,
		0,
		0,
		gen_floathisf2,
		gen_floathidf2,
		gen_floatqisf2,
		gen_floatqidf2,
		gen_ftruncdf2,
		gen_ftruncsf2,
		gen_fixsfqi2,
		gen_fixsfhi2,
		gen_fixsfsi2,
		gen_fixdfqi2,
		gen_fixdfhi2,
		gen_fixdfsi2,
		gen_fix_truncsfsi2,
		gen_fix_truncdfsi2,
		gen_addsi3,
		0,
		gen_addhi3,
		0,
		gen_addqi3,
		0,
		gen_adddf3,
		0,
		0,
		gen_addsf3,
		0,
		0,
		gen_subsi3,
		0,
		gen_subhi3,
		0,
		gen_subqi3,
		0,
		gen_subdf3,
		0,
		0,
		gen_subsf3,
		0,
		0,
		gen_mulhi3,
		gen_mulhisi3,
		gen_mulsi3,
		gen_umulhi3,
		gen_umulhisi3,
		gen_umulsi3,
		gen_muldf3,
		0,
		0,
		gen_mulsf3,
		0,
		0,
		gen_divhi3,
		gen_divhisi3,
		gen_divsi3,
		gen_udivhi3,
		gen_udivhisi3,
		gen_udivsi3,
		gen_divdf3,
		0,
		0,
		gen_divsf3,
		0,
		0,
		gen_modhi3,
		gen_modhisi3,
		gen_umodhi3,
		gen_umodhisi3,
		gen_divmodsi4,
		gen_udivmodsi4,
		gen_andsi3,
		gen_andhi3,
		gen_andqi3,
		0,
		0,
		gen_iorsi3,
		gen_iorhi3,
		gen_iorqi3,
		gen_xorsi3,
		gen_xorhi3,
		gen_xorqi3,
		gen_negsi2,
		gen_neghi2,
		gen_negqi2,
		gen_negsf2,
		0,
		0,
		gen_negdf2,
		0,
		0,
		gen_abssf2,
		0,
		0,
		gen_absdf2,
		0,
		0,
		gen_one_cmplsi2,
		gen_one_cmplhi2,
		gen_one_cmplqi2,
		0,
		0,
		0,
		0,
		0,
		0,
		gen_ashlsi3,
		gen_ashlhi3,
		gen_ashlqi3,
		gen_ashrsi3,
		gen_ashrhi3,
		gen_ashrqi3,
		gen_lshlsi3,
		gen_lshlhi3,
		gen_lshlqi3,
		gen_lshrsi3,
		gen_lshrhi3,
		gen_lshrqi3,
		gen_rotlsi3,
		gen_rotlhi3,
		gen_rotlqi3,
		gen_rotrsi3,
		gen_rotrhi3,
		gen_rotrqi3,
		0,
		0,
		0,
		gen_extv,
		gen_extzv,
		0,
		0,
		0,
		gen_insv,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		gen_seq,
		gen_sne,
		gen_sgt,
		gen_sgtu,
		gen_slt,
		gen_sltu,
		gen_sge,
		gen_sgeu,
		gen_sle,
		gen_sleu,
		gen_beq,
		gen_bne,
		gen_bgt,
		gen_bgtu,
		gen_blt,
		gen_bltu,
		gen_bge,
		gen_bgeu,
		gen_ble,
		gen_bleu,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		gen_casesi_1,
		gen_casesi_2,
		gen_casesi,
		0,
		gen_jump,
		0,
		0,
		0,
		gen_call,
		gen_call_value,
		gen_nop,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	};

const int insn_n_operands[] =
	{
		2,
		2,
		1,
		1,
		1,
		1,
		2,
		1,
		1,
		2,
		1,
		2,
		2,
		2,
		2,
		3,
		2,
		2,
		3,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		1,
		2,
		2,
		1,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		3,
		3,
		3,
		2,
		3,
		2,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		2,
		3,
		2,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		4,
		4,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		4,
		4,
		4,
		4,
		4,
		4,
		3,
		3,
		4,
		4,
		4,
		3,
		3,
		4,
		3,
		3,
		3,
		3,
		3,
		3,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		4,
		2,
		5,
		1,
		0,
		1,
		1,
		1,
		2,
		3,
		0,
		2,
		2,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
		4,
	};

const int insn_n_dups[] =
	{
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		2,
		2,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		3,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		2,
		2,
		2,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
	};

char *const insn_operand_constraint[][MAX_RECOG_OPERANDS] =
	{
		{ "=m", "ro<>fyF", },
		{ "=m", "ro<>Fy", },
		{ "rm", },
		{ "rm", },
		{ "dm", },
		{ "", },
		{ "xmdF", "d", },
		{ "fdm", },
		{ "", },
		{ "xrmF", "d", },
		{ "fm", },
		{ "rKs,mr,>", "mr,Ksr,>", },
		{ "rnm,d,n,m", "d,rnm,m,n", },
		{ "dn,md,>", "dm,nd,>", },
		{ "", "", },
		{ "x,y", "xH,rmF", "d,d", },
		{ "f,mG", "fmG,f", },
		{ "", "", },
		{ "x,y", "xH,rmF", "d,d", },
		{ "f,mdG", "fmdG,f", },
		{ "do", "di", },
		{ "d", "di", },
		{ "do", "d", },
		{ "d", "d", },
		{ "md", "i", },
		{ "o,d", "i,i", },
		{ "do", "i", },
		{ "dm", },
		{ "dm", "i", },
		{ "=m", "J", },
		{ "=g", },
		{ "=g,da,y,!*x*r*m", "daymKs,i,g,*x*r*m", },
		{ "=g", "g", },
		{ "+dm", "rmn", },
		{ "=d,*a,m,m,?*a", "dmi*a,d*a,dmi,?*a,m", },
		{ "+dm", "dmn", },
		{ "=rmf,x,y,rm,!x,!rm", "rmfF,xH,rmF,y,rm,x", },
		{ "=rm,&rf,&rof<>,y,rm,x,!x,!rm", "rf,m,rofF<>,rmF,y,xH,rm,x", },
		{ "=rm,&r,&ro<>,y,rm,!*x,!rm", "rF,m,roi<>F,rmiF,y,rmF,*x", },
		{ "=m", "p", },
		{ "=dm,d", "doJ,i", },
		{ "=dm,d", "doJ,i", },
		{ "=dm,d", "roJ,i", },
		{ "", "", },
		{ "", "", },
		{ "", "", },
		{ "=do<>", "rmn", },
		{ "=do<>", "dmn", },
		{ "=do<>", "dmn", },
		{ "=*d,a", "0,rmn", },
		{ "=d", "0", },
		{ "=d", "0", },
		{ "", "", },
		{ "=x,y", "xH,rmF", },
		{ "=*fdm,f", "f,dmF", },
		{ "", "", },
		{ "=x,y", "xH,rmF", },
		{ "=dm", "f", },
		{ "", "", },
		{ "=y,x", "rmi,x", },
		{ "=f", "dmi", },
		{ "", "", },
		{ "=y,x", "rmi,x", },
		{ "=f", "dmi", },
		{ "=f", "dmn", },
		{ "=f", "dmn", },
		{ "=f", "dmn", },
		{ "=f", "dmn", },
		{ "=f", "fFm", },
		{ "=f", "dfFm", },
		{ "=dm", "f", },
		{ "=dm", "f", },
		{ "=dm", "f", },
		{ "=dm", "f", },
		{ "=dm", "f", },
		{ "=dm", "f", },
		{ "=x,y", "xH,rmF", },
		{ "=x,y", "xH,rmF", },
		{ "=m,r,!a,!a", "%0,0,a,rJK", "dIKLs,mrIKLs,rJK,a", },
		{ "=a", "0", "rmn", },
		{ "=m,r", "%0,0", "dn,rmn", },
		{ "+m,d", "dn,rmn", },
		{ "=m,d", "%0,0", "dn,dmn", },
		{ "+m,d", "dn,dmn", },
		{ "", "", "", },
		{ "=x,y", "%xH,y", "xH,dmF", },
		{ "=f", "%0", "fmG", },
		{ "", "", "", },
		{ "=x,y", "%xH,y", "xH,rmF", },
		{ "=f", "%0", "fdmF", },
		{ "=m,r,!a,?d", "0,0,a,mrIKs", "dIKs,mrIKs,J,0", },
		{ "=a", "0", "rmn", },
		{ "=m,r", "0,0", "dn,rmn", },
		{ "+m,d", "dn,rmn", },
		{ "=m,d", "0,0", "dn,dmn", },
		{ "+m,d", "dn,dmn", },
		{ "", "", "", },
		{ "=x,y,y", "xH,y,dmF", "xH,dmF,0", },
		{ "=f", "0", "fmG", },
		{ "", "", "", },
		{ "=x,y,y", "xH,y,rmF", "xH,rmF,0", },
		{ "=f", "0", "fdmF", },
		{ "=d", "%0", "dmn", },
		{ "=d", "%0", "dmn", },
		{ "=d", "%0", "dmsK", },
		{ "=d", "%0", "dmn", },
		{ "=d", "%0", "dmn", },
		{ "=d", "%0", "dmsK", },
		{ "", "", "", },
		{ "=x,y", "%xH,y", "xH,rmF", },
		{ "=f", "%0", "fmG", },
		{ "", "", "", },
		{ "=x,y", "%xH,y", "xH,rmF", },
		{ "=f", "%0", "fdmF", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmsK", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmsK", },
		{ "", "", "", },
		{ "=x,y,y", "xH,y,rmF", "xH,rmF,0", },
		{ "=f", "0", "fmG", },
		{ "", "", "", },
		{ "=x,y,y", "xH,y,rmF", "xH,rmF,0", },
		{ "=f", "0", "fdmF", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmn", },
		{ "=d", "0", "dmsK", "=d", },
		{ "=d", "0", "dmsK", "=d", },
		{ "=m,d", "%0,0", "dKs,dmKs", },
		{ "=m,d", "%0,0", "dn,dmn", },
		{ "=m,d", "%0,0", "dn,dmn", },
		{ "=d", "dm", "0", },
		{ "=d", "dm", "0", },
		{ "=m,d", "%0,0", "dKs,dmKs", },
		{ "=m,d", "%0,0", "dn,dmn", },
		{ "=m,d", "%0,0", "dn,dmn", },
		{ "=do,m", "%0,0", "di,dKs", },
		{ "=dm", "%0", "dn", },
		{ "=dm", "%0", "dn", },
		{ "=dm", "0", },
		{ "=dm", "0", },
		{ "=dm", "0", },
		{ "", "", },
		{ "=x,y", "xH,rmF", },
		{ "=f", "fdmF", },
		{ "", "", },
		{ "=x,y", "xH,rmF", },
		{ "=f", "fmF", },
		{ "", "", },
		{ "=x,y", "xH,rmF", },
		{ "=f", "fdmF", },
		{ "", "", },
		{ "=x,y", "xH,rmF", },
		{ "=f", "fmF", },
		{ "=dm", "0", },
		{ "=dm", "0", },
		{ "=dm", "0", },
		{ "=d", "m", },
		{ "=d", "m", },
		{ "i", "m", },
		{ "m", "i", },
		{ "i", "m", },
		{ "m", "i", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "=d", "0", "dI", },
		{ "+do", "i", "i", "d", },
		{ "=&d", "do", "i", "i", },
		{ "=d", "do", "i", "i", },
		{ "=d,d", "o,d", "di,di", "di,di", },
		{ "=d,d", "o,d", "di,di", "di,di", },
		{ "+o,d", "di,di", "di,di", "i,i", },
		{ "+o,d", "di,di", "di,di", },
		{ "+o,d", "di,di", "di,di", },
		{ "+o,d", "di,di", "di,di", "d,d", },
		{ "=d", "d", "di", "di", },
		{ "=d", "d", "di", "di", },
		{ "+d", "di", "di", },
		{ "+d", "di", "di", },
		{ "+d", "di", "di", "d", },
		{ "o", "di", "di", },
		{ "o", "di", "di", },
		{ "o", "di", "di", },
		{ "d", "di", "di", },
		{ "d", "di", "di", },
		{ "d", "di", "di", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ "=d", },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ "", "", "", "", },
		{ "", "", },
		{ "", "", "", "", "", },
		{ "r", },
		{ 0 },
		{ "=g", },
		{ "=g", },
		{ "=g", },
		{ "o", "g", },
		{ "=rf", "o", "g", },
		{ 0 },
		{ "=a", "p", },
		{ "f", "ad", },
		{ "=x,y,y", "%x,dmF,y", "xH,y,y", "xH,y,dmF", },
		{ "=x,y,y", "xH,y,dmF", "%x,dmF,y", "xH,y,y", },
		{ "=x,y,y", "%x,ydmF,y", "xH,y,ydmF", "xH,ydmF,ydmF", },
		{ "=x,y,y", "xH,ydmF,ydmF", "%x,ydmF,y", "xH,y,ydmF", },
		{ "=x,y,y", "xH,rmF,y", "%xH,y,y", "x,y,rmF", },
		{ "=x,y,y", "xH,rmF,yrmF", "%xH,rmF,y", "x,y,yrmF", },
		{ "=x,y,y", "%xH,y,y", "x,y,rmF", "xH,rmF,y", },
		{ "=x,y,y", "%xH,rmF,y", "x,y,yrmF", "xH,rmF,yrmF", },
		{ "=x,y,y", "%xH,y,y", "x,y,rmF", "xH,rmF,y", },
		{ "=x,y,y", "xH,rmF,y", "%xH,y,y", "x,y,rmF", },
		{ "=x,y,y", "%xH,rmF,y", "x,y,yrmF", "xH,rmF,yrmF", },
		{ "=x,y,y", "xH,rmF,yrmF", "%xH,rmF,y", "x,y,yrmF", },
		{ "=x,y,y", "xH,y,y", "x,y,rmF", "xH,rmF,y", },
		{ "=x,y,y", "xH,rmF,y", "xH,y,y", "x,y,rmF", },
		{ "=x,y,y", "xH,rmF,y", "x,y,yrmF", "xH,rmF,yrmF", },
		{ "=x,y,y", "xH,rmF,yrmF", "xH,rmF,y", "x,y,yrmF", },
	};

const enum machine_mode insn_operand_mode[][MAX_RECOG_OPERANDS] =
	{
		{ DFmode, DFmode, },
		{ DImode, DImode, },
		{ SImode, },
		{ HImode, },
		{ QImode, },
		{ SFmode, },
		{ SFmode, SImode, },
		{ SFmode, },
		{ DFmode, },
		{ DFmode, SImode, },
		{ DFmode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
		{ DFmode, DFmode, },
		{ DFmode, DFmode, SImode, },
		{ DFmode, DFmode, },
		{ SFmode, SFmode, },
		{ SFmode, SFmode, SImode, },
		{ SFmode, SFmode, },
		{ QImode, SImode, },
		{ SImode, SImode, },
		{ QImode, SImode, },
		{ SImode, SImode, },
		{ QImode, SImode, },
		{ HImode, SImode, },
		{ SImode, SImode, },
		{ QImode, },
		{ QImode, SImode, },
		{ SImode, SImode, },
		{ SImode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
		{ QImode, QImode, },
		{ SFmode, SFmode, },
		{ DFmode, DFmode, },
		{ DImode, DImode, },
		{ SImode, SImode, },
		{ QImode, SImode, },
		{ QImode, HImode, },
		{ HImode, SImode, },
		{ SImode, HImode, },
		{ HImode, QImode, },
		{ SImode, QImode, },
		{ SImode, HImode, },
		{ HImode, QImode, },
		{ SImode, QImode, },
		{ SImode, HImode, },
		{ HImode, QImode, },
		{ SImode, QImode, },
		{ DFmode, SFmode, },
		{ DFmode, SFmode, },
		{ DFmode, SFmode, },
		{ SFmode, DFmode, },
		{ SFmode, DFmode, },
		{ SFmode, DFmode, },
		{ SFmode, SImode, },
		{ SFmode, SImode, },
		{ SFmode, SImode, },
		{ DFmode, SImode, },
		{ DFmode, SImode, },
		{ DFmode, SImode, },
		{ SFmode, HImode, },
		{ DFmode, HImode, },
		{ SFmode, QImode, },
		{ DFmode, QImode, },
		{ DFmode, DFmode, },
		{ SFmode, SFmode, },
		{ QImode, SFmode, },
		{ HImode, SFmode, },
		{ SImode, SFmode, },
		{ QImode, DFmode, },
		{ HImode, DFmode, },
		{ SImode, DFmode, },
		{ SImode, SFmode, },
		{ SImode, DFmode, },
		{ SImode, SImode, SImode, },
		{ SImode, SImode, HImode, },
		{ HImode, HImode, HImode, },
		{ HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ QImode, QImode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SImode, SImode, SImode, },
		{ SImode, SImode, HImode, },
		{ HImode, HImode, HImode, },
		{ HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ QImode, QImode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ HImode, HImode, HImode, },
		{ SImode, HImode, HImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ SImode, HImode, HImode, },
		{ SImode, SImode, SImode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ HImode, HImode, HImode, },
		{ HImode, SImode, HImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ HImode, SImode, HImode, },
		{ SImode, SImode, SImode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, },
		{ HImode, HImode, HImode, },
		{ HImode, SImode, HImode, },
		{ HImode, HImode, HImode, },
		{ HImode, SImode, HImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, HImode, SImode, },
		{ SImode, QImode, SImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
		{ SFmode, SFmode, },
		{ SFmode, SFmode, },
		{ SFmode, SFmode, },
		{ DFmode, DFmode, },
		{ DFmode, DFmode, },
		{ DFmode, DFmode, },
		{ SFmode, SFmode, },
		{ SFmode, SFmode, },
		{ SFmode, SFmode, },
		{ DFmode, DFmode, },
		{ DFmode, DFmode, },
		{ DFmode, DFmode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
		{ SImode, SImode, },
		{ SImode, SImode, },
		{ QImode, SImode, },
		{ SImode, QImode, },
		{ QImode, SImode, },
		{ SImode, QImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, QImode, SImode, SImode, },
		{ SImode, QImode, SImode, SImode, },
		{ QImode, SImode, SImode, VOIDmode, },
		{ QImode, SImode, SImode, },
		{ QImode, SImode, SImode, },
		{ QImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, },
		{ SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ QImode, SImode, SImode, },
		{ QImode, SImode, SImode, },
		{ QImode, SImode, SImode, },
		{ SImode, SImode, SImode, },
		{ SImode, SImode, SImode, },
		{ SImode, SImode, SImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ QImode, },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, },
		{ SImode, SImode, SImode, VOIDmode, VOIDmode, },
		{ HImode, },
		{ VOIDmode },
		{ HImode, },
		{ SImode, },
		{ SImode, },
		{ QImode, SImode, },
		{ VOIDmode, QImode, SImode, },
		{ VOIDmode },
		{ SImode, QImode, },
		{ VOIDmode, VOIDmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ DFmode, DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
		{ SFmode, SFmode, SFmode, SFmode, },
	};

const char insn_operand_strict_low[][MAX_RECOG_OPERANDS] =
	{
		{ 0, 0, },
		{ 0, 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, 0, },
		{ 0, },
		{ 0, },
		{ 0, 0, },
		{ 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 1, 0, },
		{ 0, 0, },
		{ 1, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 1, 0, },
		{ 0, 0, 0, },
		{ 1, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 1, 0, },
		{ 0, 0, 0, },
		{ 1, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0, 0, 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, 0, 0, },
		{ 0, },
		{ 0 },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0 },
		{ 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
	};

int (*const insn_operand_predicate[][MAX_RECOG_OPERANDS])() =
	{
		{ push_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, },
		{ nonimmediate_operand, general_operand, },
		{ nonimmediate_operand, general_operand, },
		{ nonimmediate_operand, general_operand, },
		{ nonimmediate_operand, general_operand, },
		{ nonimmediate_operand, general_operand, },
		{ nonimmediate_operand, general_operand, },
		{ nonimmediate_operand, },
		{ nonimmediate_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ push_operand, address_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ register_operand, general_operand, },
		{ register_operand, general_operand, },
		{ register_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, memory_operand, },
		{ general_operand, memory_operand, },
		{ general_operand, memory_operand, },
		{ memory_operand, general_operand, },
		{ general_operand, memory_operand, },
		{ memory_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ nonimmediate_operand, immediate_operand, immediate_operand, general_operand, },
		{ general_operand, nonimmediate_operand, immediate_operand, immediate_operand, },
		{ general_operand, nonimmediate_operand, immediate_operand, immediate_operand, },
		{ general_operand, nonimmediate_operand, general_operand, general_operand, },
		{ general_operand, nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, immediate_operand, },
		{ nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, general_operand, },
		{ general_operand, nonimmediate_operand, general_operand, general_operand, },
		{ general_operand, nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, general_operand, },
		{ memory_operand, general_operand, general_operand, },
		{ memory_operand, general_operand, general_operand, },
		{ memory_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, },
		{ nonimmediate_operand, general_operand, general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0 },
		{ general_operand, immediate_operand, general_operand, general_operand, },
		{ 0, 0, },
		{ general_operand, immediate_operand, general_operand, 0, 0, },
		{ general_operand, },
		{ 0 },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, general_operand, },
		{ 0, general_operand, general_operand, },
		{ 0 },
		{ general_operand, address_operand, },
		{ 0, 0, },
		{ register_operand, general_operand, general_operand, general_operand, },
		{ register_operand, general_operand, general_operand, general_operand, },
		{ register_operand, general_operand, general_operand, general_operand, },
		{ register_operand, general_operand, general_operand, general_operand, },
		{ register_operand, general_operand, register_operand, general_operand, },
		{ register_operand, general_operand, register_operand, general_operand, },
		{ register_operand, register_operand, general_operand, general_operand, },
		{ register_operand, register_operand, general_operand, general_operand, },
		{ register_operand, register_operand, general_operand, general_operand, },
		{ register_operand, general_operand, register_operand, general_operand, },
		{ register_operand, register_operand, general_operand, general_operand, },
		{ register_operand, general_operand, register_operand, general_operand, },
		{ register_operand, register_operand, general_operand, general_operand, },
		{ register_operand, general_operand, register_operand, general_operand, },
		{ register_operand, register_operand, general_operand, general_operand, },
		{ register_operand, general_operand, register_operand, general_operand, },
	};

#ifndef DEFAULT_MACHINE_INFO
#define DEFAULT_MACHINE_INFO 0
#endif

const INSN_MACHINE_INFO insn_machine_info[] =
	{
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
		{ DEFAULT_MACHINE_INFO },
	};

const int insn_n_alternatives[] =
	{
		1,
		1,
		1,
		1,
		1,
		0,
		1,
		1,
		0,
		1,
		1,
		3,
		4,
		3,
		0,
		2,
		2,
		0,
		2,
		2,
		1,
		1,
		1,
		1,
		1,
		2,
		1,
		1,
		1,
		1,
		1,
		4,
		1,
		1,
		5,
		1,
		6,
		8,
		7,
		1,
		2,
		2,
		2,
		0,
		0,
		0,
		1,
		1,
		1,
		2,
		1,
		1,
		0,
		2,
		2,
		0,
		2,
		1,
		0,
		2,
		1,
		0,
		2,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		2,
		2,
		4,
		1,
		2,
		2,
		2,
		2,
		0,
		2,
		1,
		0,
		2,
		1,
		4,
		1,
		2,
		2,
		2,
		2,
		0,
		3,
		1,
		0,
		3,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		2,
		1,
		0,
		2,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		3,
		1,
		0,
		3,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		2,
		2,
		2,
		1,
		1,
		2,
		2,
		2,
		2,
		1,
		1,
		1,
		1,
		1,
		0,
		2,
		1,
		0,
		2,
		1,
		0,
		2,
		1,
		0,
		2,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		2,
		2,
		2,
		2,
		2,
		2,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		0,
		1,
		1,
		1,
		1,
		1,
		0,
		1,
		1,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
		3,
	};
