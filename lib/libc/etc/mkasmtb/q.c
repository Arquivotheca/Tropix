#include <stddef.h>
#include <mkasmtb.h>

#define NOSTR	(char *)NULL
#define NOVOID	(void *)NULL

typedef	struct
{
	short	a;
	short	c;
	short	*b[2];
	long	*d;

}	HASH;

short	symtb[] = { 1, 2, 3, 4, 5, 6, 7, 8 };

long	abs[]	= { 9, 8, 7, 6, 5, 4, 3, 2, 1, 9, 8, 7, 6, 5, 4 };

HASH	hash[4] =
{
	{ 0, 6, &symtb[1], &symtb[1], &abs[7] },
	{ 1, 5, &symtb[2], NOVOID,    &abs[1] },
	{ 2, 4, &symtb[3], &symtb[0], &abs[9] },
	{ 3, 3, &symtb[4], &symtb[2], &abs[0] }
};

const OBJENT tab[] =
{
	{ sizeof (short),   offsetof (HASH, a),    1, NOSTR, NOVOID },
	{ sizeof (short),   offsetof (HASH, c),    1, NOSTR, NOVOID },
	{ sizeof (short *), offsetof (HASH, b[0]), 2, "symtb", symtb },
	{ sizeof (long *),  offsetof (HASH, d),    1, "abs", abs },
	{ 0 }

};

main ()
{
	mkasmtb
	(	"qtable.s",
		"hash",
		hash,
		sizeof hash,
		RO, 
		sizeof (HASH),
		tab
	);
}
