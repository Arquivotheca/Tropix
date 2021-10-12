#include <xti.h>

#undef	T_EXPEDITED
#define	T_EXPEDITED	T_URGENT

main ()
{
	printf ("EXPEDITED = %d, URGENT = %d\n", T_EXPEDITED, T_URGENT);

}
