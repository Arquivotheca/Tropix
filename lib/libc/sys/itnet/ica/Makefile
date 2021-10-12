#*
#****************************************************************
#*	Makefile para a chamada ao sistema "itnet"		*
#****************************************************************
#*
LIBC	= /lib/libc
DIR	= sys
PGM	= itnet
MAN	= sys

.virtual	cp cmp man prman modif gar

$PGM.o:
		@echo Não disponível para o ICARUS

cp:
		@echo Não disponível para o ICARUS

cmp:
		@echo Não disponível para o ICARUS

man:
		@echo Veja $libxti

prman:
		@echo Veja $libxti

modif:		../modif.r
		sroff ../modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
