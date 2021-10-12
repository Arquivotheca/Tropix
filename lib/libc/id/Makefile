#*
#****************************************************************
#*	Makefile para a biblioteca "id"				*
#****************************************************************
#*
man_exec = DUMMY
MAC	 = pc

.putenv man_exec

.virtual	id n cp cmp man clr gar

id:
	cd  idcan/$MAC;		make
	cd  idclr/$MAC;		make
	cd  idcmp/$MAC;		make
	cd  idcpy/$MAC;		make
	cd  ideq/$MAC;		make
	cd  idhash/$MAC;	make

n:
	cd  idcan/$MAC;		make -n
	cd  idclr/$MAC;		make -n
	cd  idcmp/$MAC;		make -n
	cd  idcpy/$MAC;		make -n
	cd  ideq/$MAC;		make -n
	cd  idhash/$MAC;	make -n

cp:
	cd  idcan/$MAC;		make cp
	cd  idclr/$MAC;		make cp
	cd  idcmp/$MAC;		make cp
	cd  idcpy/$MAC;		make cp
	cd  ideq/$MAC;		make cp
	cd  idhash/$MAC;	make cp

cmp:
	cd  idcan/$MAC;		make cmp
	cd  idclr/$MAC;		make cmp
	cd  idcmp/$MAC;		make cmp
	cd  idcpy/$MAC;		make cmp
	cd  ideq/$MAC;		make cmp
	cd  idhash/$MAC;	make cmp

man:
	cd  idcan/$MAC;		make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

gar:
	cd ..; gar -ovs0 id
	cd ..; gar -icv0
