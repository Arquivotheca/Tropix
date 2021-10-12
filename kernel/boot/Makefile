#*
#****************************************************************
#*	Makefile para o programa "pc_boot"			*
#****************************************************************
#*
PGM	= boot
DIR	= boot
MAN	= cmd

.virtual	all cmp man modif clean gar gara gars garcmp
.virtual	zipto zipfrom pento penfrom pensave

all:
		cd boot0; make
		cd boot1; make
		cd boot2; make

cmp:
		cd boot0; make cmp
		cd boot1; make cmp
		cd boot2; make cmpq

man:		man.r
		sroff man.r >$mandir/$MAN/$PGM
		ln $mandir/$MAN/$PGM $mandir/$MAN/boot0
		ln $mandir/$MAN/$PGM $mandir/$MAN/boot1
		ln $mandir/$MAN/$PGM $mandir/$MAN/boot2
		man $PGM
		cd boot2; make man

modif:
		cd boot0; make modif
		cd boot1; make modif
		cd boot2; make modif

clean:
		walk -p "*/bobo */garfield" . | rm

gar:
		cd ..; gar -ovs0 $DIR
		cd ..; gar -icv0

gars:
		cd ..; gar -ovs0 $DIR

gara:
		cd ..; gar -avs0 $DIR
		cd ..; gar -icv0

garcmp:
		cd ..; gar -icv0

zipto:
		-cmptree -sg . /zip$boot
		-forldiff /zip$boot
		-rm garfield

zipfrom:
		cd /zip$boot &&		\
		cmptree -sg . $boot;	\
		forldiff $boot;		\
		rm garfield

pento:
		-mount /pen
		-cmptree -sg . /pen$boot
		-forldiff /pen$boot
		-rm -f garfield
		-umount /pen

penfrom:
		-mount /pen
		cd /pen$boot &&		\
		cmptree -sg . $boot;	\
		forldiff $boot;		\
		rm -f /pen$boot/garfield
		-umount /pen

pensave:
		-mount /pen
		-pento $boot
		-umount /pen
