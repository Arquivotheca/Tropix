#*
#****************************************************************
#*	Makefile para "terminfo"				*
#****************************************************************
#*/
PGM	= terminfo
MAN	= fmt

man_exec = DUMMY
.putenv man_exec

.virtual	obj n lint cp cmp man prman modif

obj:
	cd h;		make
	cd capnames;	make
	cd getinfo;	make
	cd putinfo;	make
	cd comp;	make
	cd data;	make
	cd exec;	make
	cd uncomp;	make
	cd caphash;	make

n:
	cd h;		make -n
	cd capnames;	make -n
	cd getinfo;	make -n
	cd putinfo;	make -n
	cd comp;	make -n
	cd data;	make -n
	cd exec;	make -n
	cd uncomp;	make -n
	cd caphash;	make -n

lint:
#	cd h;		make lint
	cd capnames;	make lint
	cd getinfo;	make lint
	cd putinfo;	make lint
	cd comp;	make lint
	cd data;	make lint
	cd exec;	make lint
	cd uncomp;	make lint
	cd caphash;	make lint

cp:
	cd h;		make cp
	cd capnames;	make cp
	cd getinfo;	make cp
	cd putinfo;	make cp
	cd comp;	make cp
	cd data;	make cp
	cd exec;	make cp
	cd uncomp;	make cp
	cd caphash;	make cp

cmp:
	cd h;		make cmp
	cd capnames;	make cmp
	cd getinfo;	make cmp
	cd putinfo;	make cmp
	cd comp;	make cmp
	cd data;	make cmp
	cd exec;	make cmp
	cd uncomp;	make cmp
	cd caphash;	make cmp

man:	man.r
	sroff man.r >$mandir/$MAN/$PGM
	man $PGM

prman:
	sroff man.r | lpr

modif:	modif.r
	sroff modif.r >$mandir/modif/$MAN/$PGM
	modif $PGM
