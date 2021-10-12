#include <stddef.h>
#include "mkasmtb.h"

typedef	struct
{
	short	a;
	short	c;
	short	*b;
	long	*d;

}	HASH;

short	symtb[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

long	abs[]	= { 9, 8, 7, 6, 5, 4, 3, 2, 1, 9, 8, 7, 6, 5, 4 };

HASH	hash[4] =
{
	{ 0, 6, &symtb[1], &abs[7] },
	{ 1, 5, &symtb[3], &abs[1] },
	{ 2, 4, &symtb[0], &abs[9] },
	{ 3, 3, &symtb[2], &abs[0] }
};

OBJENT tab[] =
{
	{ sizeof (short *), offsetof (HASH, b), 1, "symtb", symtb },
	{ sizeof (long *),  offsetof (HASH, d), 1, "abs", abs },
	{ 0 }

};

main ()
{
	mkasmtb
	(	"table.s",
		"hash",
		hash,
		sizeof hash,
		RO, 
		sizeof (HASH),
		tab
	);
}
