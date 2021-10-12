.virtual	all cp cmp cmpq cmpqcp unld man modif clean rmobj

all:
		cd libs		&& make
		cd fvwm		&& make
		cd pager	&& make

cp:
		cd libs		&& make
		cd fvwm		&& make cp
		cd pager	&& make cp

cmp:
		cd fvwm		&& make cmp
		cd pager	&& make cmp

cmpq:
		cd fvwm		&& make cmpq
		cd pager	&& make cmpq

cmpqcp:
		cd fvwm		&& make cmpqcp
		cd pager	&& make cmpqcp

unld:
		cd fvwm		&& make unld
		cd pager	&& make unld

man modif:
		cd fvwm		&& make man modif

clean rmobj:
		walk -. -p "*.o" -p "*/.LIBSYM" . | rm -f
