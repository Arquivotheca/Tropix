#*
#****************************************************************
#*	Makefile para a biblioteca "libm"			*
#****************************************************************
#*
.virtual	libm n lint cp cmp man clean rmobj

libm:
	cd conv;	make
	cd float;	make
	cd math;	make

n:
	cd conv;	make -n
	cd float;	make -n
	cd math;	make -n

lint:
	cd conv;	make lint
	cd float;	make lint
	cd math;	make lint

cp:
	-mkdir /lib/libm
	-cmp math.h $inc || cp math.h $inc
	cd conv;	make cp
	cd float;	make cp
	cd math;	make cp
	mklib /lib/libm

cmp:
	-cmp math.h $inc
	cd conv;	make cmp
	cd float;	make cmp
	cd math;	make cmp
	mklib -k /lib/libm

man:
	cd conv;	make man
	cd float;	make man
	cd math;	make man

clean:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

rmobj:
	walk -p "*.o" . | rm
