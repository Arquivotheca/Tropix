#*
#****************************************************************
#*	Makefile para a função "t_str_to_addr"			*
#****************************************************************
#*
PGM	= t_str_addr
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
		cc -f -u -o t t.o $PGM.o -lxti

t.o:		t.c
		cc -c t.c

cp:		$PGM.o
		mklib -rv /lib/libxti $PGM.o
		chmod -v $PROT /lib/libxti/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libxti

man:
		@echo Veja ../t_addr_to_id

prman:
		@echo Veja ../t_addr_to_id

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs9 $PGM
		cd ..; gar -icv9
