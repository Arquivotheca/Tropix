/* Generated automatically by the program `genrecog'
from the machine description file `md'.  */

#include "config.h"
#include "../h/rtl.h"
#include "insn-config.h"
#include "../h/recog.h"
#include "../h/real.h"

/* `recog' contains a decision tree
   that recognizes whether the rtx X0 is a valid instruction.

   recog returns -1 if the rtx is not valid.
   If the rtx is valid, recog returns a nonnegative number
   which is the insn code number for the pattern that matched.
   This is the same as the order in the machine description of
   the entry that matched.  This number can be used as an index into
   insn_templates and insn_n_operands (found in insn-output.c)
   or as an argument to output_insn_hairy (also in insn-output.c).  */

rtx recog_operand[MAX_RECOG_OPERANDS];

rtx *recog_operand_loc[MAX_RECOG_OPERANDS];

rtx *recog_dup_loc[MAX_DUP_OPERANDS];

char recog_dup_num[MAX_DUP_OPERANDS];

extern rtx recog_addr_dummy;

#define operands recog_operand

int
recog_1 (x0, insn)
     register rtx x0;
     rtx insn;
{
  register rtx x1, x2, x3, x4, x5;
  rtx x6, x7, x8, x9, x10, x11;
  int tem;
 L539:
  x1 = XEXP (x0, 1);
  x2 = XEXP (x1, 0);
 switch (GET_CODE (x2))
  {
  case EQ:
  if (1)
    goto L540;
  break;
  case NE:
  if (1)
    goto L549;
  break;
  case GT:
  if (1)
    goto L558;
  break;
  case GTU:
  if (1)
    goto L567;
  break;
  case LT:
  if (1)
    goto L576;
  break;
  case LTU:
  if (1)
    goto L585;
  break;
  case GE:
  if (1)
    goto L594;
  break;
  case GEU:
  if (1)
    goto L603;
  break;
  case LE:
  if (1)
    goto L612;
  break;
  case LEU:
  if (1)
    goto L621;
  break;
  }
  goto ret0;
 L540:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L541;
  goto ret0;
 L541:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L542;
  goto ret0;
 L542:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L543;
  if (x2 == pc_rtx && 1)
    goto L633;
  goto ret0;
 L543:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L544; }
  goto ret0;
 L544:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 115;
  goto ret0;
 L633:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L634;
  goto ret0;
 L634:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 127; }
  goto ret0;
 L549:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L550;
  goto ret0;
 L550:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L551;
  goto ret0;
 L551:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L552;
  if (x2 == pc_rtx && 1)
    goto L642;
  goto ret0;
 L552:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L553; }
  goto ret0;
 L553:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 116;
  goto ret0;
 L642:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L643;
  goto ret0;
 L643:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 128; }
  goto ret0;
 L558:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L559;
  goto ret0;
 L559:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L560;
  goto ret0;
 L560:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L561;
  if (x2 == pc_rtx && 1)
    goto L651;
  goto ret0;
 L561:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L562; }
  goto ret0;
 L562:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 117;
  goto ret0;
 L651:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L652;
  goto ret0;
 L652:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 129; }
  goto ret0;
 L567:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L568;
  goto ret0;
 L568:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L569;
  goto ret0;
 L569:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L570;
  if (x2 == pc_rtx && 1)
    goto L660;
  goto ret0;
 L570:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L571; }
  goto ret0;
 L571:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 118;
  goto ret0;
 L660:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L661;
  goto ret0;
 L661:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 130; }
  goto ret0;
 L576:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L577;
  goto ret0;
 L577:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L578;
  goto ret0;
 L578:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L579;
  if (x2 == pc_rtx && 1)
    goto L669;
  goto ret0;
 L579:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L580; }
  goto ret0;
 L580:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 120;
  goto ret0;
 L669:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L670;
  goto ret0;
 L670:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 131; }
  goto ret0;
 L585:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L586;
  goto ret0;
 L586:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L587;
  goto ret0;
 L587:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L588;
  if (x2 == pc_rtx && 1)
    goto L678;
  goto ret0;
 L588:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L589; }
  goto ret0;
 L589:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 121;
  goto ret0;
 L678:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L679;
  goto ret0;
 L679:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 132; }
  goto ret0;
 L594:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L595;
  goto ret0;
 L595:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L596;
  goto ret0;
 L596:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L597;
  if (x2 == pc_rtx && 1)
    goto L687;
  goto ret0;
 L597:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L598; }
  goto ret0;
 L598:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 122;
  goto ret0;
 L687:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L688;
  goto ret0;
 L688:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 133; }
  goto ret0;
 L603:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L604;
  goto ret0;
 L604:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L605;
  goto ret0;
 L605:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L606;
  if (x2 == pc_rtx && 1)
    goto L696;
  goto ret0;
 L606:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L607; }
  goto ret0;
 L607:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 123;
  goto ret0;
 L696:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L697;
  goto ret0;
 L697:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 134; }
  goto ret0;
 L612:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L613;
  goto ret0;
 L613:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L614;
  goto ret0;
 L614:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L615;
  if (x2 == pc_rtx && 1)
    goto L705;
  goto ret0;
 L615:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L616; }
  goto ret0;
 L616:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 125;
  goto ret0;
 L705:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L706;
  goto ret0;
 L706:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 135; }
  goto ret0;
 L621:
  x3 = XEXP (x2, 0);
  if (x3 == cc0_rtx && 1)
    goto L622;
  goto ret0;
 L622:
  x3 = XEXP (x2, 1);
  if (x3 == const0_rtx && 1)
    goto L623;
  goto ret0;
 L623:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L624;
  if (x2 == pc_rtx && 1)
    goto L714;
  goto ret0;
 L624:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; goto L625; }
  goto ret0;
 L625:
  x2 = XEXP (x1, 2);
  if (x2 == pc_rtx && 1)
    return 126;
  goto ret0;
 L714:
  x2 = XEXP (x1, 2);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L715;
  goto ret0;
 L715:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[0] = x3; return 136; }
  goto ret0;
 ret0: return -1;
}

int
recog_2 (x0, insn)
     register rtx x0;
     rtx insn;
{
  register rtx x1, x2, x3, x4, x5;
  rtx x6, x7, x8, x9, x10, x11;
  int tem;
 L84:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, QImode))
    { recog_operand[1] = x1; return 18; }
 L123:
 switch (GET_CODE (x1))
  {
  case TRUNCATE:
  if (GET_MODE (x1) == QImode && 1)
    goto L124;
  break;
  case PLUS:
  if (GET_MODE (x1) == QImode && 1)
    goto L191;
  break;
  case MINUS:
  if (GET_MODE (x1) == QImode && 1)
    goto L226;
  break;
  case AND:
  if (GET_MODE (x1) == QImode && 1)
    goto L324;
  break;
  case IOR:
  if (GET_MODE (x1) == QImode && 1)
    goto L339;
  break;
  case XOR:
  if (GET_MODE (x1) == QImode && 1)
    goto L354;
  break;
  case NEG:
  if (GET_MODE (x1) == QImode && 1)
    goto L367;
  break;
  case NOT:
  if (GET_MODE (x1) == QImode && 1)
    goto L395;
  break;
  case ASHIFT:
  if (GET_MODE (x1) == QImode && 1)
    goto L409;
  break;
  case ASHIFTRT:
  if (GET_MODE (x1) == QImode && 1)
    goto L424;
  break;
  case LSHIFT:
  if (GET_MODE (x1) == QImode && 1)
    goto L439;
  break;
  case LSHIFTRT:
  if (GET_MODE (x1) == QImode && 1)
    goto L454;
  break;
  case ROTATE:
  if (GET_MODE (x1) == QImode && 1)
    goto L469;
  break;
  case ROTATERT:
  if (GET_MODE (x1) == QImode && 1)
    goto L484;
  break;
  case EQ:
  if (1)
    goto L489;
  break;
  case NE:
  if (1)
    goto L494;
  break;
  case GT:
  if (1)
    goto L499;
  break;
  case GTU:
  if (1)
    goto L504;
  break;
  case LT:
  if (1)
    goto L509;
  break;
  case LTU:
  if (1)
    goto L514;
  break;
  case GE:
  if (1)
    goto L519;
  break;
  case GEU:
  if (1)
    goto L524;
  break;
  case LE:
  if (1)
    goto L529;
  break;
  case LEU:
  if (1)
    goto L534;
  break;
  }
  goto ret0;
 L124:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; return 29; }
 L128:
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; return 30; }
  goto ret0;
 L191:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L192; }
  goto ret0;
 L192:
  x2 = XEXP (x1, 1);
  if (x2 == const1_rtx && 1)
    return 45;
 L197:
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 46; }
  goto ret0;
 L226:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L227; }
  goto ret0;
 L227:
  x2 = XEXP (x1, 1);
  if (x2 == const1_rtx && 1)
    return 52;
 L232:
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 53; }
  goto ret0;
 L324:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L325; }
  goto ret0;
 L325:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 70; }
  goto ret0;
 L339:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L340; }
  goto ret0;
 L340:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 73; }
  goto ret0;
 L354:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L355; }
  goto ret0;
 L355:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 76; }
  goto ret0;
 L367:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 79; }
  goto ret0;
 L395:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 86; }
  goto ret0;
 L409:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L410; }
  goto ret0;
 L410:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 89; }
  goto ret0;
 L424:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L425; }
  goto ret0;
 L425:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 92; }
  goto ret0;
 L439:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L440; }
  goto ret0;
 L440:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 95; }
  goto ret0;
 L454:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L455; }
  goto ret0;
 L455:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 98; }
  goto ret0;
 L469:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L470; }
  goto ret0;
 L470:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 101; }
  goto ret0;
 L484:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; goto L485; }
  goto ret0;
 L485:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[2] = x2; return 104; }
  goto ret0;
 L489:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L490;
  goto ret0;
 L490:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 105;
  goto ret0;
 L494:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L495;
  goto ret0;
 L495:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 106;
  goto ret0;
 L499:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L500;
  goto ret0;
 L500:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 107;
  goto ret0;
 L504:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L505;
  goto ret0;
 L505:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 108;
  goto ret0;
 L509:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L510;
  goto ret0;
 L510:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 109;
  goto ret0;
 L514:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L515;
  goto ret0;
 L515:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 110;
  goto ret0;
 L519:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L520;
  goto ret0;
 L520:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 111;
  goto ret0;
 L524:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L525;
  goto ret0;
 L525:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 112;
  goto ret0;
 L529:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L530;
  goto ret0;
 L530:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 113;
  goto ret0;
 L534:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L535;
  goto ret0;
 L535:
  x2 = XEXP (x1, 1);
  if (x2 == const0_rtx && 1)
    return 114;
  goto ret0;
 ret0: return -1;
}

int
recog_3 (x0, insn)
     register rtx x0;
     rtx insn;
{
  register rtx x1, x2, x3, x4, x5;
  rtx x6, x7, x8, x9, x10, x11;
  int tem;
 L1:
  x1 = XEXP (x0, 0);
  if (x1 == cc0_rtx && 1)
    goto L2;
  if (x1 == pc_rtx && 1)
    goto L538;
 L731:
  if (1)
    { recog_operand[0] = x1; goto L732; }
 L80:
 switch (GET_MODE (x1))
  {
  case QImode:
  if (push_operand (x1, QImode))
    { recog_operand[0] = x1; goto L81; }
 L83:
  if (general_operand (x1, QImode))
    { recog_operand[0] = x1; goto L84; }
  break;
  case HImode:
  if (push_operand (x1, HImode))
    { recog_operand[0] = x1; goto L75; }
 L77:
  if (general_operand (x1, HImode))
    { recog_operand[0] = x1; goto L78; }
  break;
  case SImode:
  if (push_operand (x1, SImode))
    { recog_operand[0] = x1; goto L69; }
 L71:
  if (general_operand (x1, SImode))
    { recog_operand[0] = x1; goto L72; }
 L119:
  if (register_operand (x1, SImode))
    { recog_operand[0] = x1; goto L120; }
 L134:
  if (general_operand (x1, SImode))
    { recog_operand[0] = x1; goto L135; }
  break;
  case DImode:
  if (push_operand (x1, DImode))
    { recog_operand[0] = x1; goto L99; }
 L101:
  if (general_operand (x1, DImode))
    { recog_operand[0] = x1; goto L102; }
  break;
  case SFmode:
  if (push_operand (x1, SFmode))
    { recog_operand[0] = x1; goto L87; }
 L89:
  if (general_operand (x1, SFmode))
    { recog_operand[0] = x1; goto L90; }
  break;
  case DFmode:
  if (push_operand (x1, DFmode))
    { recog_operand[0] = x1; goto L93; }
 L95:
  if (general_operand (x1, DFmode))
    { recog_operand[0] = x1; goto L96; }
  break;
  }
  goto ret0;
 L2:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SImode))
    { recog_operand[0] = x1; return 0; }
 L5:
  if (general_operand (x1, HImode))
    { recog_operand[0] = x1; return 1; }
 L8:
  if (general_operand (x1, QImode))
    { recog_operand[0] = x1; return 2; }
 L23:
  if (GET_CODE (x1) == COMPARE && 1)
    goto L24;
  if (GET_CODE (x1) != AND)
    {  x1 = XEXP (x0, 0);
    goto L731; }
  if (GET_MODE (x1) == SImode && 1)
    goto L55;
  if (GET_MODE (x1) == HImode && 1)
    goto L60;
  if (GET_MODE (x1) == QImode && 1)
    goto L65;
  x1 = XEXP (x0, 0);
  goto L731;
 L24:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[0] = x2; goto L25; }
 L29:
  if (general_operand (x2, HImode))
    { recog_operand[0] = x2; goto L30; }
 L34:
  if (general_operand (x2, QImode))
    { recog_operand[0] = x2; goto L35; }
  x1 = XEXP (x0, 0);
  goto L731;
 L25:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; return 5; }
  x2 = XEXP (x1, 0);
  goto L29;
 L30:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; return 6; }
  x2 = XEXP (x1, 0);
  goto L34;
 L35:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 7; }
  x1 = XEXP (x0, 0);
  goto L731;
 L55:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[0] = x2; goto L56; }
  x1 = XEXP (x0, 0);
  goto L731;
 L56:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; return 10; }
  x1 = XEXP (x0, 0);
  goto L731;
 L60:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[0] = x2; goto L61; }
  x1 = XEXP (x0, 0);
  goto L731;
 L61:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; return 11; }
  x1 = XEXP (x0, 0);
  goto L731;
 L65:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[0] = x2; goto L66; }
  x1 = XEXP (x0, 0);
  goto L731;
 L66:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 12; }
  x1 = XEXP (x0, 0);
  goto L731;
 L538:
  x1 = XEXP (x0, 1);
  if (GET_CODE (x1) == IF_THEN_ELSE && 1)
    goto L539;
  if (GET_CODE (x1) == LABEL_REF && 1)
    goto L719;
  x1 = XEXP (x0, 0);
  goto L731;
 L539:
  tem = recog_1 (x0, insn);
  if (tem >= 0) return tem;
  x1 = XEXP (x0, 0);
  goto L731;
 L719:
  x2 = XEXP (x1, 0);
  if (1)
    { recog_operand[0] = x2; return 137; }
  x1 = XEXP (x0, 0);
  goto L731;
 L732:
  x1 = XEXP (x0, 1);
  if (GET_CODE (x1) == CALL && 1)
    goto L733;
  x1 = XEXP (x0, 0);
  goto L80;
 L733:
  x2 = XEXP (x1, 0);
  if (indirect_operand (x2, QImode))
    { recog_operand[1] = x2; goto L734; }
  x1 = XEXP (x0, 0);
  goto L80;
 L734:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 140; }
  x1 = XEXP (x0, 0);
  goto L80;
 L81:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, QImode))
    { recog_operand[1] = x1; return 17; }
  x1 = XEXP (x0, 0);
  goto L83;
 L84:
  return recog_2 (x0, insn);
 L75:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, HImode))
    { recog_operand[1] = x1; return 15; }
  x1 = XEXP (x0, 0);
  goto L77;
 L78:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, HImode))
    { recog_operand[1] = x1; return 16; }
 L131:
 switch (GET_CODE (x1))
  {
  case TRUNCATE:
  if (GET_MODE (x1) == HImode && 1)
    goto L132;
  break;
  case ZERO_EXTEND:
  if (GET_MODE (x1) == HImode && 1)
    goto L140;
  break;
  case SIGN_EXTEND:
  if (GET_MODE (x1) == HImode && 1)
    goto L152;
  break;
  case PLUS:
  if (GET_MODE (x1) == HImode && 1)
    goto L181;
  break;
  case MINUS:
  if (GET_MODE (x1) == HImode && 1)
    goto L216;
  break;
  case MULT:
  if (GET_MODE (x1) == SImode && 1)
    goto L246;
  break;
  case AND:
  if (GET_MODE (x1) == HImode && 1)
    goto L319;
  break;
  case IOR:
  if (GET_MODE (x1) == HImode && 1)
    goto L334;
  break;
  case XOR:
  if (GET_MODE (x1) == HImode && 1)
    goto L349;
  break;
  case NEG:
  if (GET_MODE (x1) == HImode && 1)
    goto L363;
  break;
  case NOT:
  if (GET_MODE (x1) == HImode && 1)
    goto L391;
  break;
  case ASHIFT:
  if (GET_MODE (x1) == HImode && 1)
    goto L404;
  break;
  case ASHIFTRT:
  if (GET_MODE (x1) == HImode && 1)
    goto L419;
  break;
  case LSHIFT:
  if (GET_MODE (x1) == HImode && 1)
    goto L434;
  break;
  case LSHIFTRT:
  if (GET_MODE (x1) == HImode && 1)
    goto L449;
  break;
  case ROTATE:
  if (GET_MODE (x1) == HImode && 1)
    goto L464;
  break;
  case ROTATERT:
  if (GET_MODE (x1) == HImode && 1)
    goto L479;
  break;
  }
  goto ret0;
 L132:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; return 31; }
  goto ret0;
 L140:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 33; }
  goto ret0;
 L152:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 36; }
  goto ret0;
 L181:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L182; }
  goto ret0;
 L182:
  x2 = XEXP (x1, 1);
  if (x2 == const1_rtx && 1)
    return 43;
 L187:
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 44; }
  goto ret0;
 L216:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L217; }
  goto ret0;
 L217:
  x2 = XEXP (x1, 1);
  if (x2 == const1_rtx && 1)
    return 50;
 L222:
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 51; }
  goto ret0;
 L246:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L247; }
  goto ret0;
 L247:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 56; }
  goto ret0;
 L319:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L320; }
  goto ret0;
 L320:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 69; }
  goto ret0;
 L334:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L335; }
  goto ret0;
 L335:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 72; }
  goto ret0;
 L349:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L350; }
  goto ret0;
 L350:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 75; }
  goto ret0;
 L363:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; return 78; }
  goto ret0;
 L391:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; return 85; }
  goto ret0;
 L404:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L405; }
  goto ret0;
 L405:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 88; }
  goto ret0;
 L419:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L420; }
  goto ret0;
 L420:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 91; }
  goto ret0;
 L434:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L435; }
  goto ret0;
 L435:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 94; }
  goto ret0;
 L449:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L450; }
  goto ret0;
 L450:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 97; }
  goto ret0;
 L464:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L465; }
  goto ret0;
 L465:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 100; }
  goto ret0;
 L479:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; goto L480; }
  goto ret0;
 L480:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, HImode))
    { recog_operand[2] = x2; return 103; }
  goto ret0;
 L69:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SImode))
    { recog_operand[1] = x1; return 13; }
  x1 = XEXP (x0, 0);
  goto L71;
 L72:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SImode))
    { recog_operand[1] = x1; return 14; }
 L105:
  if (GET_MODE (x1) != SImode)
    {
      x1 = XEXP (x0, 0);
      goto L119;
    }
  if (GET_CODE (x1) == PLUS && 1)
    goto L106;
  if (GET_CODE (x1) == MINUS && 1)
    goto L116;
  x1 = XEXP (x0, 0);
  goto L119;
 L106:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L107; }
  x1 = XEXP (x0, 0);
  goto L119;
 L107:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) != CONST_INT)
    {  x1 = XEXP (x0, 0);
    goto L119; }
  if (x2 == const1_rtx && 1)
    return 25;
 L112:
  if (XINT (x2, 0) == -1 && 1)
    return 26;
  x1 = XEXP (x0, 0);
  goto L119;
 L116:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L117; }
  x1 = XEXP (x0, 0);
  goto L119;
 L117:
  x2 = XEXP (x1, 1);
  if (x2 == const1_rtx && 1)
    return 27;
  x1 = XEXP (x0, 0);
  goto L119;
 L120:
  x1 = XEXP (x0, 1);
  if (address_operand (x1, QImode))
    { recog_operand[1] = x1; return 28; }
  x1 = XEXP (x0, 0);
  goto L134;
 L135:
  x1 = XEXP (x0, 1);
  if (GET_MODE (x1) != SImode)
    goto ret0;
 switch (GET_CODE (x1))
  {
  case ZERO_EXTEND:
  if (1)
    goto L136;
  break;
  case SIGN_EXTEND:
  if (1)
    goto L148;
  break;
  case PLUS:
  if (1)
    goto L176;
  break;
  case MINUS:
  if (1)
    goto L211;
  break;
  case MULT:
  if (1)
    goto L251;
  break;
  case AND:
  if (1)
    goto L314;
  break;
  case IOR:
  if (1)
    goto L329;
  break;
  case XOR:
  if (1)
    goto L344;
  break;
  case NEG:
  if (1)
    goto L359;
  break;
  case NOT:
  if (1)
    goto L387;
  break;
  case ASHIFT:
  if (1)
    goto L399;
  break;
  case ASHIFTRT:
  if (1)
    goto L414;
  break;
  case LSHIFT:
  if (1)
    goto L429;
  break;
  case LSHIFTRT:
  if (1)
    goto L444;
  break;
  case ROTATE:
  if (1)
    goto L459;
  break;
  case ROTATERT:
  if (1)
    goto L474;
  break;
  }
  goto ret0;
 L136:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; return 32; }
 L144:
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 34; }
  goto ret0;
 L148:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, HImode))
    { recog_operand[1] = x2; return 35; }
 L156:
  if (general_operand (x2, QImode))
    { recog_operand[1] = x2; return 37; }
  goto ret0;
 L176:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L177; }
  goto ret0;
 L177:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 42; }
  goto ret0;
 L211:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L212; }
  goto ret0;
 L212:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 49; }
  goto ret0;
 L251:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L252; }
  goto ret0;
 L252:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 57; }
  goto ret0;
 L314:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L315; }
  goto ret0;
 L315:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 68; }
  goto ret0;
 L329:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L330; }
  goto ret0;
 L330:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 71; }
  goto ret0;
 L344:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L345; }
  goto ret0;
 L345:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 74; }
  goto ret0;
 L359:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; return 77; }
  goto ret0;
 L387:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; return 84; }
  goto ret0;
 L399:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L400; }
  goto ret0;
 L400:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 87; }
  goto ret0;
 L414:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L415; }
  goto ret0;
 L415:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 90; }
  goto ret0;
 L429:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L430; }
  goto ret0;
 L430:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 93; }
  goto ret0;
 L444:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L445; }
  goto ret0;
 L445:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 96; }
  goto ret0;
 L459:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L460; }
  goto ret0;
 L460:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 99; }
  goto ret0;
 L474:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; goto L475; }
  goto ret0;
 L475:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[2] = x2; return 102; }
  goto ret0;
 L99:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, DImode))
    { recog_operand[1] = x1; return 23; }
  x1 = XEXP (x0, 0);
  goto L101;
 L102:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, DImode))
    { recog_operand[1] = x1; return 24; }
  goto ret0;
 L87:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SFmode))
    { recog_operand[1] = x1; return 19; }
  x1 = XEXP (x0, 0);
  goto L89;
 L90:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SFmode))
    { recog_operand[1] = x1; return 20; }
 L163:
  if (GET_MODE (x1) != SFmode)
    goto ret0;
 switch (GET_CODE (x1))
  {
  case FLOAT_TRUNCATE:
  if (1)
    goto L164;
  break;
  case FLOAT:
  if (1)
    goto L168;
  break;
  case PLUS:
  if (1)
    goto L206;
  break;
  case MINUS:
  if (1)
    goto L241;
  break;
  case MULT:
  if (1)
    goto L277;
  break;
  case DIV:
  if (1)
    goto L287;
  break;
  case NEG:
  if (1)
    goto L371;
  break;
  case ABS:
  if (1)
    goto L379;
  break;
  }
  goto ret0;
 L164:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, DFmode))
    { recog_operand[1] = x2; if (TARGET_80387) return 39; }
  goto ret0;
 L168:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; if (TARGET_80387) return 40; }
  goto ret0;
 L206:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SFmode))
    { recog_operand[1] = x2; goto L207; }
  goto ret0;
 L207:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 48; }
  goto ret0;
 L241:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SFmode))
    { recog_operand[1] = x2; goto L242; }
  goto ret0;
 L242:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 55; }
  goto ret0;
 L277:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SFmode))
    { recog_operand[1] = x2; goto L278; }
  goto ret0;
 L278:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 61; }
  goto ret0;
 L287:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SFmode))
    { recog_operand[1] = x2; goto L288; }
  goto ret0;
 L288:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 63; }
  goto ret0;
 L371:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SFmode))
    { recog_operand[1] = x2; if (TARGET_80387) return 80; }
  goto ret0;
 L379:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SFmode))
    { recog_operand[1] = x2; if (TARGET_80387) return 82; }
  goto ret0;
 L93:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, DFmode))
    { recog_operand[1] = x1; return 21; }
  x1 = XEXP (x0, 0);
  goto L95;
 L96:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, DFmode))
    { recog_operand[1] = x1; return 22; }
 L159:
  if (GET_MODE (x1) != DFmode)
    goto ret0;
 switch (GET_CODE (x1))
  {
  case FLOAT_EXTEND:
  if (1)
    goto L160;
  break;
  case FLOAT:
  if (1)
    goto L172;
  break;
  case PLUS:
  if (1)
    goto L201;
  break;
  case MINUS:
  if (1)
    goto L236;
  break;
  case MULT:
  if (1)
    goto L272;
  break;
  case DIV:
  if (1)
    goto L282;
  break;
  case NEG:
  if (1)
    goto L375;
  break;
  case ABS:
  if (1)
    goto L383;
  break;
  }
  goto ret0;
 L160:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SFmode))
    { recog_operand[1] = x2; if (TARGET_80387) return 38; }
  goto ret0;
 L172:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[1] = x2; if (TARGET_80387) return 41; }
  goto ret0;
 L201:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, DFmode))
    { recog_operand[1] = x2; goto L202; }
  goto ret0;
 L202:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, DFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 47; }
  goto ret0;
 L236:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, DFmode))
    { recog_operand[1] = x2; goto L237; }
  goto ret0;
 L237:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, DFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 54; }
  goto ret0;
 L272:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, DFmode))
    { recog_operand[1] = x2; goto L273; }
  goto ret0;
 L273:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, DFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 60; }
  goto ret0;
 L282:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, DFmode))
    { recog_operand[1] = x2; goto L283; }
  goto ret0;
 L283:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, DFmode))
    { recog_operand[2] = x2; if (TARGET_80387) return 62; }
  goto ret0;
 L375:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, DFmode))
    { recog_operand[1] = x2; if (TARGET_80387) return 81; }
  goto ret0;
 L383:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, DFmode))
    { recog_operand[1] = x2; if (TARGET_80387) return 83; }
  goto ret0;
 ret0: return -1;
}

int
recog_4 (x0, insn)
     register rtx x0;
     rtx insn;
{
  register rtx x1, x2, x3, x4, x5;
  rtx x6, x7, x8, x9, x10, x11;
  int tem;
 L0:
 switch (GET_CODE (x0))
  {
  case SET:
  if (1)
    goto L1;
  break;
  case PARALLEL:
  if (XVECLEN (x0, 0) == 2 && 1)
    goto L10;
  break;
  case CALL:
  if (1)
    goto L728;
  break;
  case CONST_INT:
  if (x0 == const0_rtx && 1)
    return 141;
  break;
  }
  goto ret0;
 L1:
  return recog_3 (x0, insn);
 L10:
  x1 = XVECEXP (x0, 0, 0);
  if (GET_CODE (x1) == SET && 1)
    goto L11;
  goto ret0;
 L11:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L12;
  if (x2 == pc_rtx && 1)
    goto L723;
 L255:
  if (general_operand (x2, HImode))
    { recog_operand[0] = x2; goto L256; }
 L263:
  if (general_operand (x2, SImode))
    { recog_operand[0] = x2; goto L264; }
  goto ret0;
 L12:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SFmode))
    { recog_operand[0] = x2; goto L13; }
 L18:
  if (general_operand (x2, DFmode))
    { recog_operand[0] = x2; goto L19; }
 L39:
  if (GET_CODE (x2) == COMPARE && 1)
    goto L40;
  x2 = XEXP (x1, 0);
  goto L255;
 L13:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L14;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L18;
 L14:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 3;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L18;
 L19:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L20;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L39;
 L20:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 4;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L39;
 L40:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, DFmode))
    { recog_operand[0] = x3; goto L41; }
 L48:
  if (general_operand (x3, SFmode))
    { recog_operand[0] = x3; goto L49; }
  x2 = XEXP (x1, 0);
  goto L255;
 L41:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, DFmode))
    { recog_operand[1] = x3; goto L42; }
  x3 = XEXP (x2, 0);
  goto L48;
 L42:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L43;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  x3 = XEXP (x2, 0);
  goto L48;
 L43:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 8;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  x3 = XEXP (x2, 0);
  goto L48;
 L49:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SFmode))
    { recog_operand[1] = x3; goto L50; }
  x2 = XEXP (x1, 0);
  goto L255;
 L50:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L51;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L51:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 9;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L723:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[0] = x2; goto L724; }
  x2 = XEXP (x1, 0);
  goto L255;
 L724:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == USE && 1)
    goto L725;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L725:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L726;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L726:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[1] = x3; return 138; }
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L256:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == UMULT && GET_MODE (x2) == SImode && 1)
    goto L257;
  x2 = XEXP (x1, 0);
  goto L263;
 L257:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, HImode))
    { recog_operand[1] = x3; goto L258; }
  x2 = XEXP (x1, 0);
  goto L263;
 L258:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, HImode))
    { recog_operand[2] = x3; goto L259; }
  x2 = XEXP (x1, 0);
  goto L263;
 L259:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L260;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L263;
 L260:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == HImode && XINT (x2, 0) == 1 && 1)
    return 58;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L263;
 L264:
  x2 = XEXP (x1, 1);
  if (GET_MODE (x2) != SImode)
    goto ret0;
  if (GET_CODE (x2) == UMULT && 1)
    goto L265;
  if (GET_CODE (x2) == DIV && 1)
    goto L293;
  if (GET_CODE (x2) == UDIV && 1)
    goto L304;
  goto ret0;
 L265:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, SImode))
    { recog_operand[1] = x3; goto L266; }
  goto ret0;
 L266:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SImode))
    { recog_operand[2] = x3; goto L267; }
  goto ret0;
 L267:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L268;
  goto ret0;
 L268:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 1 && 1)
    return 59;
  goto ret0;
 L293:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, SImode))
    { recog_operand[1] = x3; goto L294; }
  goto ret0;
 L294:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SImode))
    { recog_operand[2] = x3; goto L295; }
  goto ret0;
 L295:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == SET && 1)
    goto L296;
  goto ret0;
 L296:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[3] = x2; goto L297; }
  goto ret0;
 L297:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == MOD && GET_MODE (x2) == SImode && 1)
    goto L298;
  goto ret0;
 L298:
  x3 = XEXP (x2, 0);
  if (rtx_equal_p (x3, recog_operand[1]) && 1)
    goto L299;
  goto ret0;
 L299:
  x3 = XEXP (x2, 1);
  if (rtx_equal_p (x3, recog_operand[2]) && 1)
    return 66;
  goto ret0;
 L304:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, SImode))
    { recog_operand[1] = x3; goto L305; }
  goto ret0;
 L305:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SImode))
    { recog_operand[2] = x3; goto L306; }
  goto ret0;
 L306:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == SET && 1)
    goto L307;
  goto ret0;
 L307:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[3] = x2; goto L308; }
  goto ret0;
 L308:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == UMOD && GET_MODE (x2) == SImode && 1)
    goto L309;
  goto ret0;
 L309:
  x3 = XEXP (x2, 0);
  if (rtx_equal_p (x3, recog_operand[1]) && 1)
    goto L310;
  goto ret0;
 L310:
  x3 = XEXP (x2, 1);
  if (rtx_equal_p (x3, recog_operand[2]) && 1)
    return 67;
  goto ret0;
 L728:
  x1 = XEXP (x0, 0);
  if (indirect_operand (x1, QImode))
    { recog_operand[0] = x1; goto L729; }
  goto ret0;
 L729:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SImode))
    { recog_operand[1] = x1; return 139; }
  goto ret0;
 ret0: return -1;
}

int
recog (x0, insn)
     register rtx x0;
     rtx insn;
{
  register rtx x1, x2, x3, x4, x5;
  rtx x6, x7, x8, x9, x10, x11;
  int tem;
 L0:
 switch (GET_CODE (x0))
  {
  case SET:
  if (1)
    goto L1;
  break;
  case PARALLEL:
  if (XVECLEN (x0, 0) == 2 && 1)
    goto L10;
  break;
  case CALL:
  if (1)
    goto L728;
  break;
  case CONST_INT:
  if (x0 == const0_rtx && 1)
    return 141;
  break;
  }
  goto ret0;
 L1:
  return recog_3 (x0, insn);
 L10:
  x1 = XVECEXP (x0, 0, 0);
  if (GET_CODE (x1) == SET && 1)
    goto L11;
  goto ret0;
 L11:
  x2 = XEXP (x1, 0);
  if (x2 == cc0_rtx && 1)
    goto L12;
  if (x2 == pc_rtx && 1)
    goto L723;
 L255:
  if (general_operand (x2, HImode))
    { recog_operand[0] = x2; goto L256; }
 L263:
  if (general_operand (x2, SImode))
    { recog_operand[0] = x2; goto L264; }
  goto ret0;
 L12:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SFmode))
    { recog_operand[0] = x2; goto L13; }
 L18:
  if (general_operand (x2, DFmode))
    { recog_operand[0] = x2; goto L19; }
 L39:
  if (GET_CODE (x2) == COMPARE && 1)
    goto L40;
  x2 = XEXP (x1, 0);
  goto L255;
 L13:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L14;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L18;
 L14:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 3;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L18;
 L19:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L20;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L39;
 L20:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 4;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  goto L39;
 L40:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, DFmode))
    { recog_operand[0] = x3; goto L41; }
 L48:
  if (general_operand (x3, SFmode))
    { recog_operand[0] = x3; goto L49; }
  x2 = XEXP (x1, 0);
  goto L255;
 L41:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, DFmode))
    { recog_operand[1] = x3; goto L42; }
  x3 = XEXP (x2, 0);
  goto L48;
 L42:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L43;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  x3 = XEXP (x2, 0);
  goto L48;
 L43:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 8;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 1);
  x3 = XEXP (x2, 0);
  goto L48;
 L49:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SFmode))
    { recog_operand[1] = x3; goto L50; }
  x2 = XEXP (x1, 0);
  goto L255;
 L50:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L51;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L51:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 0 && 1)
    if (TARGET_80387) return 9;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L723:
  x2 = XEXP (x1, 1);
  if (general_operand (x2, SImode))
    { recog_operand[0] = x2; goto L724; }
  x2 = XEXP (x1, 0);
  goto L255;
 L724:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == USE && 1)
    goto L725;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L725:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == LABEL_REF && 1)
    goto L726;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L726:
  x3 = XEXP (x2, 0);
  if (1)
    { recog_operand[1] = x3; return 138; }
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L255;
 L256:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == UMULT && GET_MODE (x2) == SImode && 1)
    goto L257;
  x2 = XEXP (x1, 0);
  goto L263;
 L257:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, HImode))
    { recog_operand[1] = x3; goto L258; }
  x2 = XEXP (x1, 0);
  goto L263;
 L258:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, HImode))
    { recog_operand[2] = x3; goto L259; }
  x2 = XEXP (x1, 0);
  goto L263;
 L259:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L260;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L263;
 L260:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == HImode && XINT (x2, 0) == 1 && 1)
    return 58;
  x1 = XVECEXP (x0, 0, 0);
  x2 = XEXP (x1, 0);
  goto L263;
 L264:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == UMULT && 1)
    goto L265;
  if (GET_CODE (x2) == DIV && 1)
    goto L293;
  if (GET_CODE (x2) == UDIV && 1)
    goto L304;
  goto ret0;
 L265:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, SImode))
    { recog_operand[1] = x3; goto L266; }
  goto ret0;
 L266:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SImode))
    { recog_operand[2] = x3; goto L267; }
  goto ret0;
 L267:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == CLOBBER && 1)
    goto L268;
  goto ret0;
 L268:
  x2 = XEXP (x1, 0);
  if (GET_CODE (x2) == REG && GET_MODE (x2) == SImode && XINT (x2, 0) == 1 && 1)
    return 59;
  goto ret0;
 L293:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, SImode))
    { recog_operand[1] = x3; goto L294; }
  goto ret0;
 L294:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SImode))
    { recog_operand[2] = x3; goto L295; }
  goto ret0;
 L295:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == SET && 1)
    goto L296;
  goto ret0;
 L296:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[3] = x2; goto L297; }
  goto ret0;
 L297:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == MOD && GET_MODE (x2) == SImode && 1)
    goto L298;
  goto ret0;
 L298:
  x3 = XEXP (x2, 0);
  if (rtx_equal_p (x3, recog_operand[1]) && 1)
    goto L299;
  goto ret0;
 L299:
  x3 = XEXP (x2, 1);
  if (rtx_equal_p (x3, recog_operand[2]) && 1)
    return 66;
  goto ret0;
 L304:
  x3 = XEXP (x2, 0);
  if (general_operand (x3, SImode))
    { recog_operand[1] = x3; goto L305; }
  goto ret0;
 L305:
  x3 = XEXP (x2, 1);
  if (general_operand (x3, SImode))
    { recog_operand[2] = x3; goto L306; }
  goto ret0;
 L306:
  x1 = XVECEXP (x0, 0, 1);
  if (GET_CODE (x1) == SET && 1)
    goto L307;
  goto ret0;
 L307:
  x2 = XEXP (x1, 0);
  if (general_operand (x2, SImode))
    { recog_operand[3] = x2; goto L308; }
  goto ret0;
 L308:
  x2 = XEXP (x1, 1);
  if (GET_CODE (x2) == UMOD && GET_MODE (x2) == SImode && 1)
    goto L309;
  goto ret0;
 L309:
  x3 = XEXP (x2, 0);
  if (rtx_equal_p (x3, recog_operand[1]) && 1)
    goto L310;
  goto ret0;
 L310:
  x3 = XEXP (x2, 1);
  if (rtx_equal_p (x3, recog_operand[2]) && 1)
    return 67;
  goto ret0;
 L728:
  x1 = XEXP (x0, 0);
  if (indirect_operand (x1, QImode))
    { recog_operand[0] = x1; goto L729; }
  goto ret0;
 L729:
  x1 = XEXP (x0, 1);
  if (general_operand (x1, SImode))
    { recog_operand[1] = x1; return 139; }
  goto ret0;
 ret0: return -1;
}
