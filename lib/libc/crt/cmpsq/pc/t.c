#define elif else if

typedef long long quad;

#if (0)	/*******************************************************/
int __cmpdi2 (long long a, long long b);

quad f ();
#endif	/*******************************************************/

main (int argc, const char *argv[])
{
	quad	q, p;

#if (0)	/*******************************************************/
	printf ("%d\n", __cmpdi2 ((quad)4, (quad)5));
	printf ("%d\n", __cmpdi2 ((quad)5, (quad)5));
	printf ("%d\n", __cmpdi2 ((quad)6, (quad)5));

	printf ("%d\n", __cmpdi2 ((quad)-4, (quad)-5));
	printf ("%d\n", __cmpdi2 ((quad)-5, (quad)-5));
	printf ("%d\n", __cmpdi2 ((quad)-6, (quad)-5));
	printf ("\n");
#endif	/*******************************************************/

	p = 5ll; q = 4ll;
	printf ("5 :: 4	=> ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p = 5ll; q = 5ll;
	printf ("5 :: 5	=> ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p = 4ll; q = 5ll;
	printf ("4 :: 5	=> ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	printf ("\n");
	p =  123456789123456789ll; q = 123456789123456789ll; 
	p += 000001000000000000ll;
	printf ("123457789123456789 :: 123456789123456789 (maior) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  123456789123456789ll; q = 123456789123456789ll; 
	p += 000000000000000010ll;
	printf ("123456789123456799 :: 123456789123456789 (maior) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  123456789123456789ll; q = 123456789123456789ll; 
	printf ("123456789123456789 :: 123456789123456789 (igual) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  123456789123456789ll; q = 123456789123456789ll; 
	q += 000001000000000000ll;
	printf ("123456789123456789 :: 123457789123456789 (menor) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  123456789123456789ll; q = 123456789123456789ll; 
	q += 000000000000000010ll;
	printf ("123456789123456789 :: 123456789123456799 (menor) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	printf ("\n");
	p = -4ll; q = -5ll;
	printf ("-4 :: -5 => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p = -5ll; q = -5ll;
	printf ("-5 :: -5 => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p = -6ll; q = -5ll;
	printf ("-6 :: -5 => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	printf ("\n");
	p =  -123456789123456789ll; q = -123456789123456789ll; 
	p += 000001000000000000ll;
	printf ("-123455789123456789 :: -123456789123456789 (maior) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  -123456789123456789ll; q = -123456789123456789ll; 
	p += 000000000000000010ll;
	printf ("-123456789123456779 :: -123456789123456789 (maior) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  -123456789123456789ll; q = -123456789123456789ll; 
	printf ("-123456789123456789 :: -123456789123456789 (igual) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  -123456789123456789ll; q = -123456789123456789ll; 
	q += 000001000000000000ll;
	printf ("-123456789123456789 :: -123455789123456789 (menor) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p =  -123456789123456789ll; q = -123456789123456789ll; 
	q += 000000000000000010ll;
	printf ("-123456789123456789 :: -123456789123456779 (menor) => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	printf ("\n");
	p = 5ll; q = -5ll;
	printf ("5 :: -5 => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");

	p = -5ll; q = 5ll;
	printf ("-5 :: 5 => ");

	if   (p > q)
		printf ("É maior\n");
	elif (p < q)
		printf ("É menor\n");
	else
		printf ("É igual\n");


	return (0);
}
