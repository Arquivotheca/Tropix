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
  operands[1] = const0_rtx;
  if (REG_P (operands[0]))
    return AS1 (tst%L0,%0);
  return AS2 (cmp%L0,%1,%0);
}
}

static char *
output_1 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  operands[1] = const0_rtx;
  if (REG_P (operands[0]))
    return AS1 (tst%W0,%0);
  return AS2 (cmp%W0,%1,%0);
}
}

static char *
output_2 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  operands[1] = const0_rtx;
  if (REG_P (operands[0]))
    return AS1 (tst%B0,%0);
  return AS2 (cmp%B0,%1,%0);
}
}

static char *
output_3 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  rtx xops[1];
  if (!FP_REG_P (operands[0]))
    fp_push_sf (operands[0]);
/*  fp_pop_level--; */
  xops[0] = FP_TOP;
  cc_status.flags |= CC_IN_80387;
  if (FP_REG_P (operands[0]) && ! top_dead_p (insn))
    output_asm_insn ("ftst\n\tfnstsw	r0\n\tsahf", xops);
  else
    output_asm_insn ("ftst\n\tfstp	%0\n\tfnstsw	r0\n\tsahf", xops);
  RETCOM (testsf);
}
}

static char *
output_4 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  rtx xops[1];
  if (!FP_REG_P (operands[0]))
    fp_push_df (operands[0]);
/*  fp_pop_level--; */
  xops[0] = FP_TOP;
  cc_status.flags |= CC_IN_80387;
  if (FP_REG_P (operands[0]) && ! top_dead_p (insn))
    output_asm_insn ("ftst\n\tfnstsw	r0\n\tsahf", xops);
  else
    output_asm_insn ("ftst\n\tfstp	%0\n\tfnstsw	r0\n\tsahf", xops);
  RETCOM (testdf);
}
}

static char *
output_5 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    {
      cc_status.flags |= CC_REVERSED;
      return AS2 (cmp%L0,%0,%1);
    }
  return AS2 (cmp%L0,%1,%0);
}
}

static char *
output_6 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    {
      cc_status.flags |= CC_REVERSED;
      return AS2 (cmp%W0,%0,%1);
    }
  return AS2 (cmp%W0,%1,%0);
}
}

static char *
output_7 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[1])
      || (!REG_P (operands[0]) && GET_CODE (operands[0]) != MEM))
    {
      cc_status.flags |= CC_REVERSED;
      return AS2 (cmp%B0,%0,%1);
    }
  return AS2 (cmp%B0,%1,%0);
}
}

static char *
output_8 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[0]))
    {
      rtx tem = operands[1];
      operands[1] = operands[0];
      operands[0] = tem;
      cc_status.flags |= CC_REVERSED;
    }
  if (! FP_REG_P (operands[1]))
    output_movdf (FP_TOP, operands[1]);
  output_movdf (FP_TOP, operands[0]);
/*  fp_pop_level--;
  fp_pop_level--; */
  cc_status.flags |= CC_IN_80387;
  return "fcompp\n\tfnstsw	r0\n\tsahf";
}
}

static char *
output_9 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[0]))
    {
      rtx tem = operands[1];
      operands[1] = operands[0];
      operands[0] = tem;
      cc_status.flags |= CC_REVERSED;
    }
  if (! FP_REG_P (operands[1]))
    output_movsf (FP_TOP, operands[1]);
  output_movsf (FP_TOP, operands[0]);
/*  fp_pop_level--;
  fp_pop_level--; */
  cc_status.flags |= CC_IN_80387;
  return "fcompp\n\tfnstsw	r0\n\tsahf";
}
}

static char *
output_10 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (CONSTANT_P (operands[1]) || GET_CODE (operands[0]) == MEM)
    return AS2 (test%L0,%1,%0);
  return AS2 (test%L0,%0,%1);
}
}

static char *
output_11 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (CONSTANT_P (operands[1]) || GET_CODE (operands[0]) == MEM)
    return AS2 (test%W0,%1,%0);
  return AS2 (test%W0,%0,%1);
}
}

static char *
output_12 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (CONSTANT_P (operands[1]) || GET_CODE (operands[0]) == MEM)
    return AS2 (test%B0,%1,%0);
  return AS2 (test%B0,%0,%1);
}
}

static char *
output_14 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  rtx link;
  if (operands[1] == const0_rtx && REG_P (operands[0]))
    return "clr%L0	%0";
  if (operands[1] == const1_rtx
      && (link = find_reg_note (insn, REG_WAS_0, 0))
      /* Make sure the insn that stored the 0 is still present.  */
      && ! XEXP (link, 0)->volatil
      && GET_CODE (XEXP (link, 0)) != NOTE
      /* Make sure cross jumping didn't happen here.  */
      && no_labels_between_p (XEXP (link, 0), insn))
    /* Fastest way to change a 0 to a 1.  */
    return "inc%L0	%0";
  return "mov%L0	%1,%0";
}
}

static char *
output_16 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  rtx link;
  if (operands[1] == const0_rtx && REG_P (operands[0]))
    return "clr%W0	%0";
  if (operands[1] == const1_rtx
      && (link = find_reg_note (insn, REG_WAS_0, 0))
      /* Make sure the insn that stored the 0 is still present.  */
      && ! XEXP (link, 0)->volatil
      && GET_CODE (XEXP (link, 0)) != NOTE
      /* Make sure cross jumping didn't happen here.  */
      && no_labels_between_p (XEXP (link, 0), insn))
    /* Fastest way to change a 0 to a 1.  */
    return "inc%W0	%0";
  return "mov%W0	%1,%0";
}
}

static char *
output_17 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  operands[1] = gen_rtx (REG, HImode, REGNO (operands[1]));
  return "push%W0	%1";
}
}

static char *
output_18 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  rtx link;
  if (operands[1] == const0_rtx && REG_P (operands[0]))
    return "clr%B0	%0";
  if (operands[1] == const1_rtx
      && (link = find_reg_note (insn, REG_WAS_0, 0))
      /* Make sure the insn that stored the 0 is still present.  */
      && ! XEXP (link, 0)->volatil
      && GET_CODE (XEXP (link, 0)) != NOTE
      /* Make sure cross jumping didn't happen here.  */
      && no_labels_between_p (XEXP (link, 0), insn))
    /* Fastest way to change a 0 to a 1.  */
    return "inc%B0	%0";
  /* If mov%B0 isn't allowed for one of these regs, use mov%W0.  */
  if (NON_QI_REG_P (operands[0]) || NON_QI_REG_P (operands[1]))
    return (AS2 (mov%W0,%w1,%w0));
  return (AS2 (mov%B0,%1,%0));
}
}

static char *
output_19 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[1]))
    {
      rtx xops[3];
      xops[0] = AT_SP (SFmode);
      xops[1] = gen_rtx (CONST_INT, VOIDmode, 4);
      xops[2] = stack_pointer_rtx;
/*      fp_pop_level--; */
      output_asm_insn (AS2 (sub%L0,%1,%2), xops);
      if (top_dead_p (insn))
        output_asm_insn ("fstp%S0	%0", xops);
      else
        output_asm_insn ("fst%S0	%0", xops);
      RET;
    }
  return "push%L0	%1";
}
}

static char *
output_20 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[1])
      && !FP_REG_P (operands[0])
      && !top_dead_p (insn))
    fp_store_sf (operands[0]);
  else
    output_movsf (operands[0], operands[1]);
  RETCOM (movsf);
}
}

static char *
output_21 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[1]))
    {
      rtx xops[3];
      xops[0] = AT_SP (DFmode);
      xops[1] = gen_rtx (CONST_INT, VOIDmode, 8);
      xops[2] = stack_pointer_rtx;
/*      fp_pop_level--; */
      output_asm_insn (AS2 (sub%L0,%1,%2), xops);
      if (top_dead_p(insn))
        output_asm_insn ("fstp%Q0	%0", xops);
      else
        output_asm_insn ("fst%Q0	%0", xops);
      RETCOM (pushdf);
    }
  else
    return output_move_double (operands);
}
}

static char *
output_22 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[1])
      && ! FP_REG_P (operands[0])
      && ! top_dead_p (insn))
    fp_store_df (operands[0]);
  else
    output_movdf (operands[0], operands[1]);
  RETCOM (movdf);
}
}

static char *
output_23 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  return output_move_double (operands);
}
}

static char *
output_24 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
   return output_move_double (operands);
}
}

static char *
output_28 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;
  /* Adding a constant to a register is faster with an add.  */
  if (GET_CODE (operands[1]) == PLUS
      && GET_CODE (XEXP (operands[1], 1)) == CONST_INT
      && rtx_equal_p (operands[0], XEXP (operands[1], 0)))
    {
      operands[1] = XEXP (operands[1], 1);
      return AS2 (add%L0,%1,%0);
    }
  return "lea%L0	%a1,%0";
}
}

static char *
output_29 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (CONSTANT_P (operands[1]) && GET_CODE (operands[1]) != CONST_INT)
    return "mov%L0	%1,%k0";
  return "mov%B0	%b1,%0";
}
}

static char *
output_30 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (CONSTANT_P (operands[1]) && GET_CODE (operands[1]) != CONST_INT)
    return "mov%W0	%1,%w0";
  return "mov%B0	%b1,%0";
}
}

static char *
output_31 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (CONSTANT_P (operands[1]) && GET_CODE (operands[1]) != CONST_INT)
    return "mov%L0	%1,%k0";
  return "mov%W0	%w1,%0";
}
}

static char *
output_38 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (FP_REG_P (operands[0]))
    {
      output_movsf (operands[0], operands[1]);
      RET;
    }
  if (FP_REG_P (operands[1]))
    {
      if (top_dead_p (insn))
        fp_pop_df (operands[0]);
      else
        fp_store_df (operands[0]);
      RET;
    }
  output_movsf (FP_TOP, operands[1]);
  fp_pop_df (operands[0]);
  RETCOM (extendsfdf2);
}
}

static char *
output_39 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (top_dead_p (insn))
    fp_pop_sf (operands[0]);
  else
    fp_store_sf (operands[0]);
  RETCOM (truncdfsf2);
}
}

static char *
output_40 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
/*  fp_pop_level++; */

  if (GET_CODE (operands[1]) != MEM)
    {
      rtx xops[2];
      output_asm_insn ("push%L0	%1", operands);
      operands[1] = AT_SP (SImode);
      output_asm_insn ("fild%L0	%1", operands);
      xops[0] = stack_pointer_rtx;
      xops[1] = gen_rtx (CONST_INT, VOIDmode, 4);
      output_asm_insn (AS2 (add%L0,%1,%0), xops);
    }
  else
    output_asm_insn ("fild%L0	%1", operands);

  if (! FP_REG_P (operands[0]))
    {
/*      fp_pop_level--; */
      return "fstp%S0	%0";
    }
  RET;
}
}

static char *
output_41 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
/*  fp_pop_level++; */
  if (GET_CODE (operands[1]) != MEM)
    {
      rtx xops[2];
      output_asm_insn ("push%L0	%1", operands);
      operands[1] = AT_SP (SImode);
      output_asm_insn ("fild%L0	%1", operands);
      xops[0] = stack_pointer_rtx;
      xops[1] = gen_rtx (CONST_INT, VOIDmode, 4);
      output_asm_insn (AS2 (add%L0,%1,%0), xops);
    }
  else
    output_asm_insn ("fild%L0	%1", operands);
  if (! FP_REG_P (operands[0]))
    {
/*      fp_pop_level--; */
      return "fstp%Q0	%0";
    }
  RET;
}
}

static char *
output_47 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fadd%z0	%0", "fadd%z0	%0", 2)
}

static char *
output_48 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fadd%z0	%0", "fadd%z0	%0", 2)
}

static char *
output_54 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fsub%z0	%0", "fsubr%z0	%0", 2)
}

static char *
output_55 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fsub%z0	%0", "fsubr%z0	%0", 2)
}

static char *
output_56 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[1]) == REG
      && REGNO (operands[1]) == REGNO (operands[0])
      && (GET_CODE (operands[2]) == MEM
	  || GET_CODE (operands[2]) == REG))
    /* Assembler has weird restrictions.  */
    return AS2 (muls%W0,%2,%0);
  return AS3 (muls%W0,%2,%1,%0);
}
}

static char *
output_57 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[1]) == REG
      && REGNO (operands[1]) == REGNO (operands[0])
      && (GET_CODE (operands[2]) == MEM
	  || GET_CODE (operands[2]) == REG))
    /* Assembler has weird restrictions.  */
    return AS2 (muls%L0,%2,%0);
  return AS3 (muls%L0,%2,%1,%0);
}
}

static char *
output_60 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fmul%z0	%0", "fmul%z0	%0", 2)

}

static char *
output_61 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fmul%z0	%0", "fmul%z0	%0", 2)

}

static char *
output_62 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fdiv%z0	%0", "fdivr%z0	%0", 2)

}

static char *
output_63 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL ("fdiv%z0	%0", "fdivr%z0	%0", 2)

}

static char *
output_80 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL1 ("fchs")
}

static char *
output_81 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL1 ("fchs")
}

static char *
output_82 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL1 ("fabs")
}

static char *
output_83 (operands, insn)
		rtx *operands;
		rtx insn;
{
FP_CALL1 ("fabs")
}

static char *
output_87 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (asl%L0,%R0r1,%0);
  else if (REG_P (operands[1]) && GET_CODE (operands[2]) == CONST_INT
	   && INTVAL (operands[2]) == 1)
    return AS2 (add%L0,%1,%1);
  return AS2 (asl%L0,%2,%1);
}
}

static char *
output_88 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (asl%W0,%R0r1,%0);
  else
    return AS2 (asl%W0,%2,%1);
}
}

static char *
output_89 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (asl%B0,%R0r1,%0);
  else
    return AS2 (asl%B0,%2,%1);
}
}

static char *
output_90 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (asr%L0,%R0r1,%0);
  else
    return AS2 (asr%L0,%2,%0);
}
}

static char *
output_91 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (asr%W0,%R0r1,%0);
  else
    return AS2 (asr%W0,%2,%0);
}
}

static char *
output_92 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (asr%B0,%R0r1,%0);
  return
    AS2 (asr%B0,%2,%1);
}
}

static char *
output_93 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (lsl%L0,%R0r1,%0);
  else
    return AS2 (lsl%L0,%2,%1);
}
}

static char *
output_94 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (lsl%W0,%R0r1,%0);
  else
    return AS2 (lsl%W0,%2,%1);
}
}

static char *
output_95 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (lsl%B0,%R0r1,%0);
  else
    return AS2 (lsl%B0,%2,%1);
}
}

static char *
output_96 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (lsr%L0,%R0r1,%0);
  else
    return AS2 (lsr%L0,%2,%1);
}
}

static char *
output_97 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (lsr%W0,r1,%0);
  else
    return AS2 (lsr%W0,%2,%1);
}
}

static char *
output_98 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (lsr%B0,r1,%0);
  else
    return AS2 (lsr%B0,%2,%1);
}
}

static char *
output_99 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (rol%L0,r1,%0);
  else
    return AS2 (rol%L0,%2,%1);
}
}

static char *
output_100 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (rol%W0,r1,%0);
  else
    return AS2 (rol%W0,%2,%1);
}
}

static char *
output_101 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (rol%B0,r1,%0);
  else
    return AS2 (rol%B0,%2,%1);
}
}

static char *
output_102 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (ror%L0,r1,%0);
  else
    return AS2 (ror%L0,%2,%1);
}
}

static char *
output_103 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (ror%W0,r1,%0);
  else
    return AS2 (ror%W0,%2,%1);
}
}

static char *
output_104 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (REG_P (operands[2]))
    return AS2 (ror%B0,r1,%0);
  else
    return AS2 (ror%B0,%2,%1);
}
}

static char *
output_105 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  return "sete	%0";

}

static char *
output_106 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  return "setne	%0";

}

static char *
output_107 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  OUTPUT_JUMP ("setg	%0", "seta	%0", 0);

}

static char *
output_108 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  return "seta	%0"; 
}

static char *
output_109 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  OUTPUT_JUMP ("setl	%0", "setb	%0", "sets	%0"); 
}

static char *
output_110 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  return "setb	%0"; 
}

static char *
output_111 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  OUTPUT_JUMP ("setge	%0", "setae	%0", "setns	%0"); 
}

static char *
output_112 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  return "setae	%0"; 
}

static char *
output_113 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  OUTPUT_JUMP ("setle	%0", "setbe	%0", 0);

}

static char *
output_114 (operands, insn)
		rtx *operands;
		rtx insn;
{

  CC_STATUS_INIT;
  return "setbe	%0"; 
}

static char *
output_117 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jgt	%l0", "jhi	%l0", 0)
}

static char *
output_120 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jlt	%l0", "jlo	%l0", "jmi	%l0")
}

static char *
output_122 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jge	%l0", "jhs	%l0", "jpl	%l0")
}

static char *
output_125 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jle	%l0", "jls	%l0", 0) 
}

static char *
output_129 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jle	%l0", "jls	%l0", 0) 
}

static char *
output_131 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jge	%l0", "jhs	%l0", "jpl	%l0")

}

static char *
output_133 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jlt	%l0", "jlo	%l0", "jmi	%l0")
}

static char *
output_135 (operands, insn)
		rtx *operands;
		rtx insn;
{
OUTPUT_JUMP ("jgt	%l0", "jhi	%l0", 0)
}

static char *
output_138 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  CC_STATUS_INIT;

  return "jmp	%*%0";
}
}

static char *
output_139 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[0]) == MEM
      && ! CONSTANT_ADDRESS_P (XEXP (operands[0], 0)))
    {
      operands[0] = XEXP (operands[0], 0);
      return "call	%*%0";
    }
  else
    return "call	%0";
}
}

static char *
output_140 (operands, insn)
		rtx *operands;
		rtx insn;
{

{
  if (GET_CODE (operands[1]) == MEM
      && ! CONSTANT_ADDRESS_P (XEXP (operands[1], 0)))
    {
      operands[1] = XEXP (operands[1], 0);
      output_asm_insn ("call	%*%1", operands);
    }
  else
    output_asm_insn ("call	%1", operands);

#ifndef FMTOWNS
  if (TARGET_80387)
#endif
    if (GET_MODE (operands[0]) == DFmode
	|| GET_MODE (operands[0]) == SFmode)
      {
  /*      fp_pop_level++; */
	/* pop if reg dead */
	if (!FP_REG_P (operands[0]))
	  abort ();
	if (top_dead_p (insn))
	  {
	    POP_ONE_FP;
	  }
      }
  RET;
}
}

char * const insn_template[] =
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
		"push%L0	%1",
		0,
		"push%W0	%1",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"inc%L0	%0",
		"dec%L0	%0",
		"dec%L0	%0",
		0,
		0,
		0,
		0,
		"movz%W0%L0	%1,%0",
		"movz%B0%W0	%1,%0",
		"movz%B0%L0	%1,%0",
		"movs%W0%L0	%1,%0",
		"movs%B0%W0	%1,%0",
		"movs%B0%L0	%1,%0",
		0,
		0,
		0,
		0,
		"add%L0	%2,%0",
		"inc%W0	%0",
		"add%W0	%2,%0",
		"inc%B0	%0",
		"add%B0	%2,%0",
		0,
		0,
		"sub%L0	%2,%0",
		"dec%W0	%0",
		"sub%W0	%2,%0",
		"dec%B0	%0",
		"sub%B0	%2,%0",
		0,
		0,
		0,
		0,
		"mulu%W0	%2,%0",
		"mulu%L0	%2,%0",
		0,
		0,
		0,
		0,
		0,
		0,
		"extlq\n\tdivs%L0	%2",
		"clr%L0	%3\n\tdivu%L0	%2",
		"and%L0	%2,%0",
		"and%W0	%2,%0",
		"and%B0	%2,%0",
		"or%L0	%2,%0",
		"or%W0	%2,%0",
		"or%B0	%2,%0",
		"eor%L0	%2,%0",
		"eor%W0	%2,%0",
		"eor%B0	%2,%0",
		"neg%L0	%0",
		"neg%W0	%0",
		"neg%B0	%0",
		0,
		0,
		0,
		0,
		"not%L0	%0",
		"not%W0	%0",
		"not%B0	%0",
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		"jeq	%l0",
		"jne	%l0",
		0,
		"jhi	%l0",
		0,
		0,
		"jlo	%l0",
		0,
		"jhs	%l0",
		0,
		0,
		"jls	%l0",
		"jne	%l0",
		"jeq	%l0",
		0,
		"jls	%l0",
		0,
		"jhs	%l0",
		0,
		"jlo	%l0",
		0,
		"jhi	%l0",
		"jmp	%l0",
		0,
		0,
		0,
		"nop",
	};

char *(*const insn_outfun[])() =
	{
		output_0,
		output_1,
		output_2,
		output_3,
		output_4,
		output_5,
		output_6,
		output_7,
		output_8,
		output_9,
		output_10,
		output_11,
		output_12,
		0,
		output_14,
		0,
		output_16,
		output_17,
		output_18,
		output_19,
		output_20,
		output_21,
		output_22,
		output_23,
		output_24,
		0,
		0,
		0,
		output_28,
		output_29,
		output_30,
		output_31,
		0,
		0,
		0,
		0,
		0,
		0,
		output_38,
		output_39,
		output_40,
		output_41,
		0,
		0,
		0,
		0,
		0,
		output_47,
		output_48,
		0,
		0,
		0,
		0,
		0,
		output_54,
		output_55,
		output_56,
		output_57,
		0,
		0,
		output_60,
		output_61,
		output_62,
		output_63,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		output_80,
		output_81,
		output_82,
		output_83,
		0,
		0,
		0,
		output_87,
		output_88,
		output_89,
		output_90,
		output_91,
		output_92,
		output_93,
		output_94,
		output_95,
		output_96,
		output_97,
		output_98,
		output_99,
		output_100,
		output_101,
		output_102,
		output_103,
		output_104,
		output_105,
		output_106,
		output_107,
		output_108,
		output_109,
		output_110,
		output_111,
		output_112,
		output_113,
		output_114,
		0,
		0,
		output_117,
		0,
		0,
		output_120,
		0,
		output_122,
		0,
		0,
		output_125,
		0,
		0,
		0,
		output_129,
		0,
		output_131,
		0,
		output_133,
		0,
		output_135,
		0,
		0,
		output_138,
		output_139,
		output_140,
		0,
	};

rtx (*const insn_gen_function[]) () =
	{
		gen_tstsi,
		gen_tsthi,
		gen_tstqi,
		gen_tstsf,
		gen_tstdf,
		gen_cmpsi,
		gen_cmphi,
		gen_cmpqi,
		gen_cmpdf,
		gen_cmpsf,
		0,
		0,
		0,
		0,
		gen_movsi,
		0,
		gen_movhi,
		0,
		gen_movqi,
		0,
		gen_movsf,
		0,
		gen_movdf,
		0,
		gen_movdi,
		0,
		0,
		0,
		0,
		gen_truncsiqi2,
		gen_trunchiqi2,
		gen_truncsihi2,
		gen_zero_extendhisi2,
		gen_zero_extendqihi2,
		gen_zero_extendqisi2,
		gen_extendhisi2,
		gen_extendqihi2,
		gen_extendqisi2,
		gen_extendsfdf2,
		gen_truncdfsf2,
		gen_floatsisf2,
		gen_floatsidf2,
		gen_addsi3,
		0,
		gen_addhi3,
		0,
		gen_addqi3,
		gen_adddf3,
		gen_addsf3,
		gen_subsi3,
		0,
		gen_subhi3,
		0,
		gen_subqi3,
		gen_subdf3,
		gen_subsf3,
		gen_mulhi3,
		gen_mulsi3,
		gen_umulhi3,
		gen_umulsi3,
		gen_muldf3,
		gen_mulsf3,
		gen_divdf3,
		gen_divsf3,
		gen_divmodsi4,
		gen_udivmodsi4,
		0,
		0,
		gen_andsi3,
		gen_andhi3,
		gen_andqi3,
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
		gen_negdf2,
		gen_abssf2,
		gen_absdf2,
		gen_one_cmplsi2,
		gen_one_cmplhi2,
		gen_one_cmplqi2,
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
		0,
		gen_bltu,
		gen_bge,
		gen_bgeu,
		gen_ble,
		0,
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
		gen_jump,
		gen_tablejump,
		gen_call,
		gen_call_value,
		gen_nop,
	};

const int insn_n_operands[] =
	{
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
		2,
		3,
		2,
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
		4,
		4,
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
		2,
		3,
		0,
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
		0,
		0,
		0,
		0,
		0,
		0,
		0,
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
		{ "rm", },
		{ "rm", },
		{ "qm", },
		{ "rm,f", },
		{ "rm,f", },
		{ "mr,ri", "ri,mr", },
		{ "mr,ri", "ri,mr", },
		{ "qn,mq", "qm,nq", },
		{ "m,f*r,m,f,r,!*r", "m,m,f*r,r,f,*r", },
		{ "m,f*r,m,f,r,!*r", "m,m,f*r,r,f,*r", },
		{ "rm,ri", "ri,rm", },
		{ "rm,ri", "ri,rm", },
		{ "qm,qi", "qi,qm", },
		{ "=<", "g", },
		{ "=g,r", "ri,m", },
		{ "=<", "g", },
		{ "=g,r", "ri,m", },
		{ "=<", "q", },
		{ "=q,*r,m", "*g,q,qi", },
		{ "=<,<", "gF,f", },
		{ "=rf,mrf,!rm", "mrf,rf,F", },
		{ "=<,<", "gF,f", },
		{ "=rmf,&fr,!rm", "fr,mrf,F", },
		{ "=<", "roiF", },
		{ "=&r,rm", "m,riF", },
		{ "=g", "0", },
		{ "=g", "0", },
		{ "=g", "0", },
		{ "=r", "p", },
		{ "=q,m", "qim,qn", },
		{ "=q,m", "qim,qn", },
		{ "=r,m", "rim,rn", },
		{ "=r", "rm", },
		{ "=r", "qm", },
		{ "=r", "qm", },
		{ "=r", "rm", },
		{ "=r", "qm", },
		{ "=r", "qm", },
		{ "=fm,f,fm,fm", "m,0,f,!*r", },
		{ "=m,!*r", "f,f", },
		{ "=fm,fm", "m,!*r", },
		{ "=fm,fm", "m,!*r", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=g", "0", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=qm", "0", },
		{ "=m,q", "%0,0", "qn,qmn", },
		{ "=f,m,f", "%0,0,0", "m,!f,!*r", },
		{ "=f,m,f", "%0,0,0", "m,!f,!*r", },
		{ "=rm,r", "0,0", "ri,rm", },
		{ "=g", "0", },
		{ "=rm,r", "0,0", "ri,rm", },
		{ "=qm", "0", },
		{ "=m,q", "0,0", "qn,qmn", },
		{ "=f,m,f,f", "0,0,0,m", "m,!f,!*r,*0", },
		{ "=f,m,f,f", "0,0,0,m", "m,!f,!*r,*0", },
		{ "=r,r", "%0,rm", "g,i", },
		{ "=r,r", "%0,rm", "g,i", },
		{ "=a", "%0", "rm", },
		{ "=a", "%0", "rm", },
		{ "=f,m,f", "%0,0,0", "m,!f,!*r", },
		{ "=f,m,f", "%0,0,0", "m,!f,!*r", },
		{ "=f,m,f,f", "0,0,0,m", "m,!f,!*r,*0", },
		{ "=f,m,f,f", "0,0,0,m", "m,!f,!*r,*0", },
		{ "", "", "", "", },
		{ "", "", "", "", },
		{ "=a", "0", "rm", "=&d", },
		{ "=a", "0", "rm", "=&d", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=m,q", "%0,0", "qn,qmn", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=m,q", "%0,0", "qn,qmn", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=rm,r", "%0,0", "ri,rm", },
		{ "=qm", "%0", "qn", },
		{ "=rm", "0", },
		{ "=rm", "0", },
		{ "=qm", "0", },
		{ "=f,!m", "0,0", },
		{ "=f,!m", "0,0", },
		{ "=f,!m", "0,0", },
		{ "=f,!m", "0,0", },
		{ "=rm", "0", },
		{ "=rm", "0", },
		{ "=qm", "0", },
		{ "=rm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=qm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=qm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=qm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=qm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=qm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=rm", "0", "cI", },
		{ "=qm", "0", "cI", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
		{ "=q", },
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
		{ 0 },
		{ 0 },
		{ 0 },
		{ "rm", },
		{ "m", "g", },
		{ "=rf", "m", "g", },
		{ 0 },
	};

const enum machine_mode insn_operand_mode[][MAX_RECOG_OPERANDS] =
	{
		{ SImode, },
		{ HImode, },
		{ QImode, },
		{ SFmode, },
		{ DFmode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
		{ DFmode, DFmode, },
		{ SFmode, SFmode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
		{ SImode, SImode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
		{ QImode, QImode, },
		{ SFmode, SFmode, },
		{ SFmode, SFmode, },
		{ DFmode, DFmode, },
		{ DFmode, DFmode, },
		{ DImode, DImode, },
		{ DImode, DImode, },
		{ SImode, SImode, },
		{ SImode, SImode, },
		{ SImode, SImode, },
		{ SImode, QImode, },
		{ QImode, SImode, },
		{ QImode, HImode, },
		{ HImode, SImode, },
		{ SImode, HImode, },
		{ HImode, QImode, },
		{ SImode, QImode, },
		{ SImode, HImode, },
		{ HImode, QImode, },
		{ SImode, QImode, },
		{ DFmode, SFmode, },
		{ SFmode, DFmode, },
		{ SFmode, SImode, },
		{ DFmode, SImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, },
		{ QImode, QImode, QImode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, },
		{ QImode, QImode, QImode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ HImode, HImode, HImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ SImode, SImode, SImode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ DFmode, DFmode, DFmode, },
		{ SFmode, SFmode, SFmode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, SImode, },
		{ SImode, SImode, SImode, },
		{ HImode, HImode, HImode, },
		{ QImode, QImode, QImode, },
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
		{ DFmode, DFmode, },
		{ SFmode, SFmode, },
		{ DFmode, DFmode, },
		{ SImode, SImode, },
		{ HImode, HImode, },
		{ QImode, QImode, },
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
		{ VOIDmode },
		{ VOIDmode },
		{ VOIDmode },
		{ SImode, },
		{ QImode, SImode, },
		{ VOIDmode, QImode, SImode, },
		{ VOIDmode },
	};

const char insn_operand_strict_low[][MAX_RECOG_OPERANDS] =
	{
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
		{ 0, },
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
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
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
		{ 0, 0, 0, 0, },
		{ 0, 0, 0, 0, },
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
		{ 0 },
		{ 0 },
		{ 0 },
		{ 0, },
		{ 0, 0, },
		{ 0, 0, 0, },
		{ 0 },
	};

int (*const insn_operand_predicate[][MAX_RECOG_OPERANDS])() =
	{
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, general_operand, },
		{ push_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ general_operand, general_operand, },
		{ register_operand, address_operand, },
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
		{ general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, },
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
		{ general_operand, general_operand, general_operand, general_operand, },
		{ general_operand, general_operand, general_operand, general_operand, },
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
		{ 0 },
		{ 0 },
		{ 0 },
		{ general_operand, },
		{ indirect_operand, general_operand, },
		{ 0, indirect_operand, general_operand, },
		{ 0 },
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
	};

const int insn_n_alternatives[] =
	{
		1,
		1,
		1,
		2,
		2,
		2,
		2,
		2,
		6,
		6,
		2,
		2,
		2,
		1,
		2,
		1,
		2,
		1,
		3,
		2,
		3,
		2,
		3,
		1,
		2,
		1,
		1,
		1,
		1,
		2,
		2,
		2,
		1,
		1,
		1,
		1,
		1,
		1,
		4,
		2,
		2,
		2,
		2,
		1,
		2,
		1,
		2,
		3,
		3,
		2,
		1,
		2,
		1,
		2,
		4,
		4,
		2,
		2,
		1,
		1,
		3,
		3,
		4,
		4,
		0,
		0,
		1,
		1,
		2,
		2,
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
		1,
		1,
		0,
	};
