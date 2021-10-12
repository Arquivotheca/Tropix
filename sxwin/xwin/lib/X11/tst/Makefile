#*
#****************************************************************
#	Makefile para programas de teste da Xlib		*
#****************************************************************
#*
INC	=	-I ..			\
		-I ../../../include	\
		-I ../../../include/X11

.virtual	all lint unld

all:		xecho			\
		xcoord

xecho:		xecho.c
		cc -O $INC -o xecho xecho.c ../lib ../../os/lib
		size xecho

xcoord:		xcoord.c
		cc -O $INC -o xcoord xcoord.c ../lib ../../os/lib
		size xcoord

lint:
		cc -T $INC xecho.c
		cc -T $INC xcoord.c

unld:
		rm -f xecho xcoord
