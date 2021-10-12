#include	<curses.h>

main ()
{
	int	ch;

	incurses ("");

	ch = getctlch (C_SWITCH);
	printf ("switch: %d\n", ch);

	setctlch (C_SWITCH, 7);
	ch = getctlch (C_SWITCH);
	printf ("switch: %d\n", ch);

	outcurses ();
	exit (0);

}	/* end main */
