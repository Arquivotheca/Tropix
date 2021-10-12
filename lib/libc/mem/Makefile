#*
#****************************************************************
#*	Makefile para a biblioteca "mem"			*
#****************************************************************
#*
man_exec = DUMMY
MAC	 = pc

.putenv man_exec

.virtual	cp cmp man gar clr

mem:
	cd alloca/$MAC;		make
	cd calloc/$MAC;		make
	cd chkalloc;		make
	cd free;		make
	cd malloc;		make
	cd memccpy/$MAC;	make
	cd memchr/$MAC;		make
	cd memcmp/$MAC;		make
	cd memcpy/$MAC;		make
	cd memeq/$MAC;		make
	cd memmove/$MAC;	make
	cd memset/$MAC;		make
	cd memsetw/$MAC;	make
	cd memsetl/$MAC;	make
	cd memtcmp/$MAC;	make
	cd memttcmp/$MAC;	make
	cd realloc;		make
	cd smalloc;		make

n:
	cd alloca/$MAC;		make -n
	cd calloc/$MAC;		make -n
	cd chkalloc;		make -n
	cd free;		make -n
	cd malloc;		make -n
	cd memccpy/$MAC;	make -n
	cd memchr/$MAC;		make -n
	cd memcmp/$MAC;		make -n
	cd memcpy/$MAC;		make -n
	cd memeq/$MAC;		make -n
	cd memmove/$MAC;	make -n
	cd memset/$MAC;		make -n
	cd memsetw/$MAC;	make -n
	cd memsetl/$MAC;	make -n
	cd memtcmp/$MAC;	make -n
	cd memttcmp/$MAC;	make -n
	cd realloc;		make -n
	cd smalloc;		make -n

cp:
	cd alloca/$MAC;		make cp
	cd calloc/$MAC;		make cp
	cd chkalloc;		make cp
	cd free;		make cp
	cd malloc;		make cp
	cd memccpy/$MAC;	make cp
	cd memchr/$MAC;		make cp
	cd memcmp/$MAC;		make cp
	cd memcpy/$MAC;		make cp
	cd memeq/$MAC;		make cp
	cd memmove/$MAC;	make cp
	cd memset/$MAC;		make cp
	cd memsetw/$MAC;	make cp
	cd memsetl/$MAC;	make cp
	cd memtcmp/$MAC;	make cp
	cd memttcmp/$MAC;	make cp
	cd realloc;		make cp
	cd smalloc;		make cp

cmp:
	cd alloca/$MAC;		make cmp
	cd calloc/$MAC;		make cmp
	cd chkalloc;		make cmp
	cd free;		make cmp
	cd malloc;		make cmp
	cd memccpy/$MAC;	make cmp
	cd memchr/$MAC;		make cmp
	cd memcmp/$MAC;		make cmp
	cd memcpy/$MAC;		make cmp
	cd memeq/$MAC;		make cmp
	cd memmove/$MAC;	make cmp
	cd memset/$MAC;		make cmp
	cd memsetw/$MAC;	make cmp
	cd memsetl/$MAC;	make cmp
	cd memtcmp/$MAC;	make cmp
	cd memttcmp/$MAC;	make cmp
	cd realloc;		make cmp
	cd smalloc;		make cmp

man:
	cd alloca/$MAC;		make man
	cd calloc/$MAC;		make man
	cd malloc;		make man
	cd memccpy/$MAC;	make man
	cd memchr/$MAC;		make man
	cd memcmp/$MAC;		make man
	cd memcpy/$MAC;		make man
	cd memeq/$MAC;		make man
	cd memmove/$MAC;	make man
	cd memset/$MAC;		make man
	cd memsetw/$MAC;	make man
	cd memsetl/$MAC;	make man
	cd memtcmp/$MAC;	make man
	cd memttcmp/$MAC;	make man
	cd realloc;		make man
	cd smalloc;		make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

gar:
	cd ..; gar -ovs0 mem
	cd ..; gar -icv0
