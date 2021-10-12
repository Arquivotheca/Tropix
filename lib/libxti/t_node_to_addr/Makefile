#*
#****************************************************************
#*	Makefile para a função "t_node_to_addr"			*
#****************************************************************
#*
PGM	= t_node_addr
PROT	= u=rw,a+r
MAN	= xti
DEF	= -D PC -D LITTLE_ENDIAN

.virtual	lint cp cmp man prman modif gar

$PGM.o:		$PGM.c
		cc $DEF -O -c $PGM.c
		size $PGM.o
		ls -l $PGM.o
		nm $PGM.o | /usr/bin/c

lint:		$PGM.c
		cc $DEF -T $PGM.c

t:		t.o $PGM.o
		cc -o t t.o $PGM.o -lxti

t.o:		t.c
		cc -c t.c

s:		s.o $PGM.o
		cc -o s s.o $PGM.o -lxti

s.o:		s.c
		cc -c s.c

cp:		$PGM.o
		mklib -rv /lib/libxti $PGM.o
		chmod -v $PROT /lib/libxti/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libxti

man:
		@echo Veja ../t_addr_to_node

prman:
		@echo Veja ../t_addr_to_node

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs9 $PGM
		cd ..; gar -icv9
