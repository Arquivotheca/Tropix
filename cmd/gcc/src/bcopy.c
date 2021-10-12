#include <string.h>

void *
bcopy (void *fonte, void *destino, int sz)
{
	return (memmove (destino, fonte, sz));
}

int
bcmp (void *fonte, void *destino, int sz)
{
	return (memcmp (destino, fonte, sz));
}

void *
bzero (void *destino, int sz)
{
	return (memset (destino, 0, sz));
}

