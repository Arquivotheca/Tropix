#*
#****************************************************************
#*	Makefile para o programa "echomon"			*
#****************************************************************
#*
PGM	= echomon
PROT	= a+x
MAN	= cmd
CC	= cc

all:
		cd $(PGM)_gen; make
		cd $(PGM)_pr;  make

lint:
		cd $(PGM)_gen; make lint
		cd $(PGM)_pr;  make lint

cp:
		cd $(PGM)_gen; make cp
		cd $(PGM)_pr;  make cp

cmp:
		cd $(PGM)_gen; make cmp
		cd $(PGM)_pr;  make cmp

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0

gara:
		cd ..; gar -avs0 $PGM
		cd ..; gar -icv0
