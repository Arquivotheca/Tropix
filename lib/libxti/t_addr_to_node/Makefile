#*
#****************************************************************
#*	Makefile para a função "t_addr_to_node"			*
#****************************************************************
#*
PGM	= t_addr_node
PGM2	= t_addr_to_node
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

cp:		$PGM.o
		mklib -rv /lib/libxti $PGM.o
		chmod -v $PROT /lib/libxti/$PGM.o

cmp:
		-cmpobj $PGM.o /lib/libxti

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM2
		-ln $mandir/$MAN/$PGM2 $mandir/$MAN/t_node_to_addr
		-ln $mandir/$MAN/$PGM2 $mandir/$MAN/t_addr_to_name
		-ln $mandir/$MAN/$PGM2 $mandir/$MAN/t_addr_to_str
		-ln $mandir/$MAN/$PGM2 $mandir/$MAN/t_str_to_addr
		-ln $mandir/$MAN/$PGM2 $mandir/$MAN/t_addr
		man $PGM2

prman:
		sroff man.r | lpr

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

gar:
		cd ..; gar -ovs9 $PGM
		cd ..; gar -icv9
