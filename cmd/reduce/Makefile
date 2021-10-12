#*
#****************************************************************
#*	Makefile para o programa "reduce"			*
#****************************************************************
#*
ROOT	=
DIR	= $ROOT/usr/pub/bin
PGM	= reduce
PGM2	= rcat
PROT	= a+x
MAN	= cmd
DEBUG	= -d
TIME	= 
CRC	=

SRC	= $(PGM)0.c $(PGM)1.c $(PGM)2.c $(PGM)3.c
OBJ	= $(PGM)0.o $(PGM)1.o $(PGM)2.o $(PGM)3.o

.virtual	c lint tst tst1 tst2 tst3 tst4 cp cmp cmpq cmpqcp unld man modif gar gara

$PGM:		$PGM.h $OBJ
		cc -s -u -o $PGM $OBJ
		size $PGM
		ln -f $PGM $PGM2
		ls -l $PGM $PGM2

c:		$OBJ

$(PGM)0.o:	$(PGM)0.c $PGM.h
		cc -c -O $(PGM)0.c

$(PGM)1.o:	$(PGM)1.c $PGM.h
		cc -c -O $(PGM)1.c

$(PGM)2.o:	$(PGM)2.c $PGM.h
		cc -c -O $(PGM)2.c

$(PGM)3.o:	$(PGM)3.c $PGM.h
		cc -c -O $(PGM)3.c

lint:		$SRC
		cc -T $SRC

tst:
		cat *.c >t.c
		reduce -v t.c t.z
		rcat -v t.z | cmp -l - t.c
		rm -f t.c t.z

tst1:
		$TIME$PGM -v $DEBUG $CRC .tst/fonte.16 /tmp/fonte.16.z
		$TIME$PGM -uv $DEBUG /tmp/fonte.16.z /tmp/u.fonte.16
		cmp /tmp/u.fonte.16 .tst/fonte.16
		-rm -f /tmp/u.fonte.16 /tmp/fonte.16.z

tst2:
		$TIME$PGM -v $DEBUG  $CRC .tst/fonte.40 /tmp/fonte.40.z
		$TIME$PGM -uv $DEBUG /tmp/fonte.40.z /tmp/u.fonte.40
		cmp /tmp/u.fonte.40 .tst/fonte.40
		-rm -f /tmp/u.fonte.40 /tmp/fonte.40.z

tst3:
		$TIME$PGM -v $DEBUG $CRC .tst/fonte.120 /tmp/fonte.120.z
		$TIME$PGM -uv $DEBUG /tmp/fonte.120.z /tmp/u.fonte.120
		cmp /tmp/u.fonte.120 .tst/fonte.120
		-rm -f /tmp/u.fonte.120 /tmp/fonte.120.z

tst4:
		$TIME$PGM -v $DEBUG $CRC .tst/fonte.420 /tmp/fonte.420.z
		$TIME$PGM -uv $DEBUG /tmp/fonte.420.z .tst/u.fonte.420
		cmp .tst/u.fonte.420 .tst/fonte.420
		-rm -f .tst/u.fonte.420 /tmp/fonte.420.z

cp:		$PGM
		cp -f $PGM $DIR
		chmod $PROT $DIR/$PGM
		ln -f $DIR/$PGM $DIR/$PGM2
		ls -l $DIR/$PGM $DIR/$PGM2

cmp:
		-cmpobj $PGM $DIR
		-cmpobj $DIR/$PGM $DIR/$PGM2

cmpq:
		-cmpobj -q $PGM $DIR

cmpqcp:
		-cmpobj -q $PGM $DIR || make cp

unld:
		-rm -f $PGM

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln -f $mandir/$MAN/$PGM $mandir/$MAN/$PGM2
		man $PGM

modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

prman:
		sroff man.r | lpr

gar:
		cd ..; gar -ovs0 $PGM
		cd ..; gar -icv0
gara:
		cd ..; gar -avs0 -B 1 $PGM
		cd ..; gar -icv0 -B 1
