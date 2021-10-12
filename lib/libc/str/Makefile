#*
#****************************************************************
#*	Makefile para a biblioteca "str"			*
#****************************************************************
#*
man_exec = DUMMY
MAC	 = pc

.putenv man_exec

.virtual	str n cp cmp man clr gar

str:
	cd strcat/$MAC;		make
	cd strchr/$MAC;		make
	cd strcmp/$MAC;		make
	cd strcpy/$MAC;		make
	cd strcspn/$MAC;	make
	cd strdup;		make
	cd strend/$MAC;		make
	cd streq/$MAC;		make
	cd strerror;		make
	cd strfield/$MAC;	make
	cd strhash/$MAC;	make
	cd stricmp/$MAC;	make
	cd strircmp/$MAC;	make
	cd strlen/$MAC;		make
	cd strmove/$MAC;	make
	cd strncat/$MAC;	make
	cd strncmp/$MAC;	make
	cd strncpy/$MAC;	make
	cd strntcmp/$MAC;	make
	cd strnttcmp/$MAC;	make
	cd strpbrk/$MAC;	make
	cd strrchr/$MAC;	make
	cd strrcmp/$MAC;	make
	cd strscpy/$MAC;	make
	cd strspn/$MAC;		make
	cd strstr/$MAC;		make
	cd strtcmp/$MAC;	make
	cd strtok/$MAC;		make
	cd strttcmp/$MAC;	make

n:
	cd strcat/$MAC;		make -n
	cd strchr/$MAC;		make -n
	cd strcmp/$MAC;		make -n
	cd strcpy/$MAC;		make -n
	cd strcspn/$MAC;	make -n
	cd strdup;		make -n
	cd strend/$MAC;		make -n
	cd streq/$MAC;		make -n
	cd strerror;		make -n
	cd strfield/$MAC;	make -n
	cd strhash/$MAC;	make -n
	cd stricmp/$MAC;	make -n
	cd strircmp/$MAC;	make -n
	cd strlen/$MAC;		make -n
	cd strmove/$MAC;	make -n
	cd strncat/$MAC;	make -n
	cd strncmp/$MAC;	make -n
	cd strncpy/$MAC;	make -n
	cd strntcmp/$MAC;	make -n
	cd strnttcmp/$MAC;	make -n
	cd strpbrk/$MAC;	make -n
	cd strrchr/$MAC;	make -n
	cd strrcmp/$MAC;	make -n
	cd strscpy/$MAC;	make -n
	cd strspn/$MAC;		make -n
	cd strstr/$MAC;		make -n
	cd strtcmp/$MAC;	make -n
	cd strtok/$MAC;		make -n
	cd strttcmp/$MAC;	make -n

cp:
	cd strcat/$MAC;		make cp
	cd strchr/$MAC;		make cp
	cd strcmp/$MAC;		make cp
	cd strcpy/$MAC;		make cp
	cd strcspn/$MAC;	make cp
	cd strdup;		make cp
	cd strend/$MAC;		make cp
	cd streq/$MAC;		make cp
	cd strerror;		make cp
	cd strfield/$MAC;	make cp
	cd strhash/$MAC;	make cp
	cd stricmp/$MAC;	make cp
	cd strircmp/$MAC;	make cp
	cd strlen/$MAC;		make cp
	cd strmove/$MAC;	make cp
	cd strncat/$MAC;	make cp
	cd strncmp/$MAC;	make cp
	cd strncpy/$MAC;	make cp
	cd strntcmp/$MAC;	make cp
	cd strnttcmp/$MAC;	make cp
	cd strpbrk/$MAC;	make cp
	cd strrchr/$MAC;	make cp
	cd strrcmp/$MAC;	make cp
	cd strscpy/$MAC;	make cp
	cd strspn/$MAC;		make cp
	cd strstr/$MAC;		make cp
	cd strtcmp/$MAC;	make cp
	cd strtok/$MAC;		make cp
	cd strttcmp/$MAC;	make cp

cmp:
	cd strcat/$MAC;		make cmp
	cd strchr/$MAC;		make cmp
	cd strcmp/$MAC;		make cmp
	cd strcpy/$MAC;		make cmp
	cd strcspn/$MAC;	make cmp
	cd strdup;		make cmp
	cd strend/$MAC;		make cmp
	cd streq/$MAC;		make cmp
	cd strerror;		make cmp
	cd strfield/$MAC;	make cmp
	cd strhash/$MAC;	make cmp
	cd stricmp/$MAC;	make cmp
	cd strircmp/$MAC;	make cmp
	cd strlen/$MAC;		make cmp
	cd strmove/$MAC;	make cmp
	cd strncat/$MAC;	make cmp
	cd strncmp/$MAC;	make cmp
	cd strncpy/$MAC;	make cmp
	cd strntcmp/$MAC;	make cmp
	cd strnttcmp/$MAC;	make cmp
	cd strpbrk/$MAC;	make cmp
	cd strrchr/$MAC;	make cmp
	cd strrcmp/$MAC;	make cmp
	cd strscpy/$MAC;	make cmp
	cd strspn/$MAC;		make cmp
	cd strstr/$MAC;		make cmp
	cd strtcmp/$MAC;	make cmp
	cd strtok/$MAC;		make cmp
	cd strttcmp/$MAC;	make cmp

man:
	cd strcmp/$MAC;		make man
	cd strcpy/$MAC;		make man
	cd strtok/$MAC;		make man

clr:
	walk -p "*/?.o */? *bobo *garfield" . | rm -iN

gar:
	cd ..; gar -ovs0 str
	cd ..; gar -icv0
