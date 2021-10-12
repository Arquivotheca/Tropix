#*
#****************************************************************
#*	Makefile para a biblioteca "pwd"			*
#****************************************************************
#*
man_exec = DUMMY

.putenv man_exec

.virtual	pwd n cp cmp man clr gar

pwd:
	cd getlogin;	make
	cd getpass;	make
	cd grcache;	make
	cd grent;	make
	cd pwcache;	make
	cd pwent;	make

n:
	cd getlogin;	make -n
	cd getpass;	make -n
	cd grcache;	make -n
	cd grent;	make -n
	cd pwcache;	make -n
	cd pwent;	make -n

cp:
	cd getlogin;	make cp
	cd getpass;	make cp
	cd grcache;	make cp
	cd grent;	make cp
	cd pwcache;	make cp
	cd pwent;	make cp

cmp:
	cd getlogin;	make cmp
	cd getpass;	make cmp
	cd grcache;	make cmp
	cd grent;	make cmp
	cd pwcache;	make cmp
	cd pwent;	make cmp

man:
	cd getlogin;	make man
	cd getpass;	make man
	cd grcache;	make man
	cd grent;	make man
	cd pwcache;	make man
	cd pwent;	make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

gar:
	cd ..; gar -ovs0 pwd
	cd ..; gar -icv0
