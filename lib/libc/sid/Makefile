#*
#****************************************************************
#*	Makefile para a biblioteca "sid"			*
#****************************************************************
#*
man_exec = DUMMY
MAC	 = pc

.putenv man_exec

.virtual	sid n cp cmp man gar clr

sid:
	cd  sidcan/$MAC;	make
	cd  sidclr/$MAC;	make
	cd  sidcmp/$MAC;	make
	cd  sidcpy/$MAC;	make
	cd  sideq/$MAC;		make
	cd  sidhash/$MAC;	make

n:
	cd  sidcan/$MAC;	make -n
	cd  sidclr/$MAC;	make -n
	cd  sidcmp/$MAC;	make -n
	cd  sidcpy/$MAC;	make -n
	cd  sideq/$MAC;		make -n
	cd  sidhash/$MAC;	make -n

cp:
	cd  sidcan/$MAC;	make cp
	cd  sidclr/$MAC;	make cp
	cd  sidcmp/$MAC;	make cp
	cd  sidcpy/$MAC;	make cp
	cd  sideq/$MAC;		make cp
	cd  sidhash/$MAC;	make cp

cmp:
	cd  sidcan/$MAC;	make cmp
	cd  sidclr/$MAC;	make cmp
	cd  sidcmp/$MAC;	make cmp
	cd  sidcpy/$MAC;	make cmp
	cd  sideq/$MAC;		make cmp
	cd  sidhash/$MAC;	make cmp

man:
	cd  sidcan/$MAC;	make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

gar:
	cd ..; gar -ovs0 sid
	cd ..; gar -icv0
