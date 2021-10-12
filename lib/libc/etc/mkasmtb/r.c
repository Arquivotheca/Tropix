#include <stddef.h>
#include <mkasmtb.h>

#define	NULL	0
#define	NOSTR	(char *)NULL
#define	NOVOID	(void *)NULL

typedef	struct
{
	short	a;
	short	c;

}	HASH;

HASH	hash[4] =
{
	{ 0, 6 },
	{ 1, 5 },
	{ 2, 4 },
	{ 3, 3 }
};

const OBJENT tab[] =
{
	{ sizeof (short), offsetof (HASH, a), 1, NOSTR, NOVOID },
	{ sizeof (short), offsetof (HASH, c), 1, NOSTR, NOVOID },
	{ 0 }
};

main ()
{
	mkasmtb
	(	"hash.s",
		"hash",
		hash,
		sizeof hash,
		RWL, 
		sizeof (HASH),
		tab
	);
}
