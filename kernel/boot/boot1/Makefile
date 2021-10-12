#*
#****************************************************************
#*	Makefile para o programa "boot1"			*
#****************************************************************
#*
PGM	= boot1
MAN	= cmd
AS	= as
LD	= ld
CPDIR	= /etc/boot

.virtual	all cpd cmpd cp cmp modif
.virtual	pento penfrom pensave

all:		t1.l.boot1 t1.g.boot1 t1.d.boot1	\
		v7.l.boot1 v7.g.boot1 v7.d.boot1	\
		cd.boot1

#*
#****** Sistema de Arquivos T1 **********************************
#*
t1.l.boot1:	t1.boot1.s
		$AS -D DEV=81 -l t1.l.boot1.l -o t1.l.boot1.o t1.boot1.s
		$LD -T 0x7C00 t1.l.boot1.o
		size a.out
		strip a.out
		dd if=a.out of=t1.l.boot1 bs=80 skip=1
		ls -l t1.l.boot1
		rm -f t1.l.boot1.o a.out

.virtual	cat

cat:		t1.l.boot1.t1
		cat t1.l.boot1.t1 >/dev/root
		-cmp t1.l.boot1.t1 /dev/root

t1.g.boot1:	t1.boot1.s
		$AS -D DEV=80 -l t1.g.boot1.l -o t1.g.boot1.o t1.boot1.s
		$LD -T 0x7C00 t1.g.boot1.o
		size a.out
		strip a.out
		dd if=a.out of=t1.g.boot1 bs=80 skip=1
		ls -l t1.g.boot1
		rm -f t1.g.boot1.o a.out

t1.d.boot1:	t1.boot1.s
		$AS -D DEV=1440 -l t1.d.boot1.l -o t1.d.boot1.o t1.boot1.s
		$LD -T 0x7C00 t1.d.boot1.o
		size a.out
		strip a.out
		dd if=a.out of=t1.d.boot1 bs=80 skip=1
		ls -l t1.d.boot1
		rm -f t1.d.boot1.o a.out

#*
#****** Sistema de Arquivos V7 **********************************
#*
v7.l.boot1:	v7.boot1.s
		$AS -D DEV=81 -l v7.l.boot1.l -o v7.l.boot1.o v7.boot1.s
		$LD -T 0x7C00 v7.l.boot1.o
		size a.out
		strip a.out
		dd if=a.out of=v7.l.boot1 bs=80 skip=1
		ls -l v7.l.boot1
		rm -f v7.l.boot1.o a.out

v7.g.boot1:	v7.boot1.s
		$AS -D DEV=80 -l v7.g.boot1.l -o v7.g.boot1.o v7.boot1.s
		$LD -T 0x7C00 v7.g.boot1.o
		size a.out
		strip a.out
		dd if=a.out of=v7.g.boot1 bs=80 skip=1
		ls -l v7.g.boot1
		rm -f v7.g.boot1.o a.out

v7.d.boot1:	v7.boot1.s
		$AS -D DEV=1440 -l v7.d.boot1.l -o v7.d.boot1.o v7.boot1.s
		$LD -T 0x7C00 v7.d.boot1.o
		size a.out
		strip a.out
		dd if=a.out of=v7.d.boot1 bs=80 skip=1
		ls -l v7.d.boot1
		rm -f v7.d.boot1.o a.out

#*
#****** Sistema de Arquivos CD **********************************
#*
cd.boot1:	cd.boot1.s
		$AS -l cd.boot1.l -o cd.boot1.o cd.boot1.s
		$LD -T 0x7C00 cd.boot1.o
		size a.out
		strip a.out
		dd if=a.out of=cd.boot1 bs=80 skip=1
		ls -l cd.boot1
		rm -f cd.boot1.o a.out

#*
#****** Cópias, comparações *************************************
#*
DEV	= fd0

cpd:
		cat t1.d.boot1.t1 >/dev/$DEV
		-cmp t1.d.boot1.t1 /dev$DEV

cmpd:
		-cmp t1.d.boot1.t1 /dev$DEV

cp:		all $CPDIR
		cp t1.l.boot1 t1.g.boot1 t1.d.boot1 v7.l.boot1 v7.g.boot1 v7.d.boot1 cd.boot1 $CPDIR

cmp:		$CPDIR
		cmp t1.l.boot1 t1.g.boot1 t1.d.boot1 v7.l.boot1 v7.g.boot1 v7.d.boot1 cd.boot1 $CPDIR

$CPDIR:		.exist
		mkdir $CPDIR

#*
#****** Manuais	*************************************************
#*
modif:		modif.r
		sroff modif.r >$mandir/modif/$MAN/$PGM
		modif $PGM

#*
#****** Atualiza o fonte ****************************************
#*
pento:
		-mount /pen
		-cmptree -sg . /pen$boot1
		-forldiff /pen$boot1
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$boot1 &&		\
		cmptree -sg . $boot1;		\
		forldiff $boot1;		\
		rm -f /pen$boot1/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $boot1
		-umount /pen

