#include <sys/syscall.h>
#include <malloc.h>

main ()
{
	register int	i;
	register char	*a;

	printf ("cbreak = %P\n", _cbreak);

	a = smalloc (1024);

	printf ("cbreak = %P\n", _cbreak);

	a[0]    = 55;
	a[1023] = 77;

	printf
	(	"smalloc: &a = %P,  a[0] = %d, a[1023] = %d\n", 
		a,
		a[0],
		a[1023]
	);

	sfree ();

	a = smalloc (1024);

	printf
	(	"smalloc: &a = %P,  a[0] = %d, a[1023] = %d\n", 
		a,
		a[0],
		a[1023]
	);

	sfree ();

	for (i = 4096/256 + 1; i > 0; i--)
		printf ("area [%02d]: %P\n", i, smalloc (256));
}
