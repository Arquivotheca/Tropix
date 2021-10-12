#define elif else if

typedef unsigned long long quad;

int __ucmpdi2 (unsigned long long a, unsigned long long b);

quad f ();

main (int argc, const char *argv[])
{
	quad	q, p;

	printf ("%d\n", __ucmpdi2 ((quad)4, (quad)5));
	printf ("%d\n", __ucmpdi2 ((quad)5, (quad)5));
	printf ("%d\n", __ucmpdi2 ((quad)6, (quad)5));

	p = 5ll; q = 4ll;

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p = 5ll; q = 5ll;

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p = 4ll; q = 5ll;

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  123456789123456789ll; q = 123456789123456789ll; 
	p += 000001000000000000ll;

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  123456789123456789ll; q = 123456789123456789ll; 

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  123456789123456789ll; q = 123456789123456789ll; 
	q += 000001000000000000ll;

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	return (0);
}
