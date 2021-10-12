#*
#****************************************************************
#*	Makefile para a biblioteca "lid"			*
#****************************************************************
#*
man_exec = DUMMY
MAC	 = pc

.putenv man_exec

.virtual	lid n cp cmp man gar clr

lid:
	cd  lidcan/$MAC;	make
	cd  lidclr/$MAC;	make
	cd  lidcmp/$MAC;	make
	cd  lidcpy/$MAC;	make
	cd  lideq/$MAC;		make
	cd  lidhash/$MAC;	make

n:
	cd  lidcan/$MAC;	make -n
	cd  lidclr/$MAC;	make -n
	cd  lidcmp/$MAC;	make -n
	cd  lidcpy/$MAC;	make -n
	cd  lideq/$MAC;		make -n
	cd  lidhash/$MAC;	make -n

cp:
	cd  lidcan/$MAC;	make cp
	cd  lidclr/$MAC;	make cp
	cd  lidcmp/$MAC;	make cp
	cd  lidcpy/$MAC;	make cp
	cd  lideq/$MAC;		make cp
	cd  lidhash/$MAC;	make cp

cmp:
	cd  lidcan/$MAC;	make cmp
	cd  lidclr/$MAC;	make cmp
	cd  lidcmp/$MAC;	make cmp
	cd  lidcpy/$MAC;	make cmp
	cd  lideq/$MAC;		make cmp
	cd  lidhash/$MAC;	make cmp

man:
	cd  lidcan/$MAC;	make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

gar:
	cd ..; gar -ovs0 lid
	cd ..; gar -icv0
