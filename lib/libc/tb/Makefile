#*
#****************************************************************
#*	Makefile para a biblioteca "tb"				*
#****************************************************************
#*
man_exec = DUMMY

.putenv man_exec

.virtual	tb n cp cmp man clr gar

tb:
	cd cmpafoldtb;		make
	cd cmpdictb;		make
	cd cmpfoldtb;		make
	cd cmpisotb;		make
	cd isotoatb;		make
	cd isotoabitb;		make
	cd isotoisotb;		make

n:
	cd cmpafoldtb;		make -n
	cd cmpdictb;		make -n
	cd cmpfoldtb;		make -n
	cd cmpisotb;		make -n
	cd isotoatb;		make -n
	cd isotoabitb;		make -n
	cd isotoisotb;		make -n

cp:
	cd cmpafoldtb;		make cp
	cd cmpdictb;		make cp
	cd cmpfoldtb;		make cp
	cd cmpisotb;		make cp
	cd isotoatb;		make cp
	cd isotoabitb;		make cp
	cd isotoisotb;		make cp

cmp:
	cd cmpafoldtb;		make cmp
	cd cmpdictb;		make cmp
	cd cmpfoldtb;		make cmp
	cd cmpisotb;		make cmp
	cd isotoatb;		make cmp
	cd isotoabitb;		make cmp
	cd isotoisotb;		make cmp

man:
	cd cmpafoldtb;		make man
	cd cmpdictb;		make man
	cd cmpfoldtb;		make man
	cd cmpisotb;		make man
	cd isotoatb;		make man
	cd isotoabitb;		make man
	cd isotoisotb;		make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

gar:
	cd ..; gar -ovs0 tb
	cd ..; gar -icv0
