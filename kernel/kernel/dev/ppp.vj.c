/*
 ****************************************************************
 *								*
 *			ppp.vj.c				*
 *								*
 *	Compressão TCP/IP para PPP segundo Van Jacobson		*
 *								*
 *	Versão	3.0.0, de 08.07.96				*
 *		3.0.0, de 15.07.96				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 * 		Copyright © 1999 NCE/UFRJ - tecle "man licença"	*
 * 								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"
#include "../h/scb.h"

#include "../h/itnet.h"
#include "../h/ppp.h"

#include "../h/proto.h"
#include "../h/extern.h"

/*
 ****************************************************************
 *	Definições Globais					*
 ****************************************************************
 */

/*
 *	Bits in first octet of compressed packet
 */

/* Flag bits for what changed in a packet */

#define NEW_C		0x40	/* New Slot Index */
#define NEW_I		0x20	/* New IP ID */
#define TCP_PSH_BIT	0x10

#define NEW_S		0x08	/* New seq_no */
#define NEW_A		0x04	/* New ack_no */
#define NEW_W		0x02	/* New window */
#define NEW_U		0x01	/* New urg_ptr */

/* Reserved, special-case values of above */

#define SPECIAL_I	(NEW_S|NEW_W|NEW_U)	  /* Echoed interactive traffic */
#define SPECIAL_D	(NEW_S|NEW_A|NEW_W|NEW_U) /* Unidirectional data */
#define SPECIALS_MASK	(NEW_S|NEW_A|NEW_W|NEW_U)

/*
 *	The following macros are used to encode and decode numbers.
 *	They all assume that `cp' points to a buffer where the next
 *	byte encoded (decoded) is to be stored (retrieved). Since
 *	the decode routines do arithmetic, they have to convert
 *	from and to network byte order
 */

/*
 *	ENCODE encodes a number that is known to be non-zero.
 *	ENCODEZ checks for zero (zero has to be encoded in the
 *	long, 3 byte form)
 */
#define ENCODE(n)						\
{ 								\
	if ((unsigned)(n) >= 256)				\
		{ *cp++ = 0; *cp++ = (n) >> 8; *cp++ = (n); }	\
	else							\
		{ *cp++ = (n); }				\
}

#define ENCODEZ(n)						\
{ 								\
	if ((unsigned)(n) >= 256 || (n) == 0)			\
		{ *cp++ = 0; *cp++ = (n) >> 8; *cp++ = (n); }	\
	else							\
		{ *cp++ = (n); }				\
}

/*
 *	DECODEL takes the (compressed) change at byte cp and
 *	adds it to the current value of packet field 'f'
 *	(which must be a 4-byte (long) integer in network byte order).
 *	DECODES does the same for a 2-byte (short) field.
 *	DECODEU takes the change at cp and stuffs it into the
 *	(short) field f. 'cp' is updated to point to the next
 *	field in the compressed header.
 */
#define DECODEL(f)		\
{ 				\
	if (*cp == 0)		\
	{			\
		(f) = ENDIAN_LONG (ENDIAN_LONG (f) + ((cp[1] << 8)|cp[2])); \
		cp += 3; 	\
	}			\
	else			\
	{ 			\
		(f) = ENDIAN_LONG (ENDIAN_LONG (f) + *cp++); \
	}			\
}

#define DECODES(f)		\
{ 				\
	if (*cp == 0)		\
	{			\
		(f) = ENDIAN_SHORT (ENDIAN_SHORT (f) + ((cp[1] << 8)|cp[2])); \
		cp += 3; 	\
	}			\
	else			\
	{ 			\
		(f) = ENDIAN_SHORT (ENDIAN_SHORT (f) + *cp++); \
	}			\
}

#define DECODEU(f)		\
{ 				\
	if (*cp == 0)		\
	{			\
		(f) = ENDIAN_SHORT ((cp[1] << 8)|cp[2]); \
		cp += 3; 	\
	}			\
	else			\
	{ 			\
		(f) = ENDIAN_SHORT (*cp++); \
	}			\
}

/*
 ****************************************************************
 *	Inicializa as estruturas de compressão			*
 ****************************************************************
 */
void
sl_compress_init (SLCOMPRESS *comp)
{
	register unsigned int	i;
	register CSTATE		*tstate = comp->tstate;

	/*
	 *	Clean out any junk left from the last time line was used
	 */
	memclr (comp, sizeof (SLCOMPRESS));

	/*
	 *	Link the transmit states into a circular list
	 */
	for (i = N_SLOT - 1; i > 0; i--)
	{
		tstate[i].cs_slot = i;
		tstate[i].cs_next = &tstate[i - 1];
	}

	tstate[0].cs_next = &tstate[N_SLOT - 1];
	tstate[0].cs_slot = 0;

	comp->last_cs = &tstate[0];

	/*
	 *	Make sure we don't accidentally do CID compression
	 *	(assumes N_SLOT < 255)
	 */
	comp->last_rcv_slot = 255;
	comp->last_snd_slot = 255;

} 	/* end sl_compress_init */

/*
 ****************************************************************
 *	Comprime um datagrama					*
 ****************************************************************
 */
int
sl_compress_tcp (ITBLOCK *bp, const PPP_STATUS *sp)
{
	register SLCOMPRESS	*comp = sp->ppp_slcompress_ptr;
	register CSTATE		*cs = comp->last_cs->cs_next;
	register IP_H		*ip = (IP_H *)bp->it_area;
	register unsigned int	hlen = ip->ip_h_size;
	unsigned int		vj_hlen, ip_hlen;
	unsigned int		tcp_checksum, seq_len;
	register TCP_H		*oth;		/* last TCP header */
	register TCP_H		*th;		/* current TCP header */
	register unsigned int	deltaS, deltaA;	/* general purpose temporaries */
	register unsigned int	changes = 0;	/* change mask */
	char			new_seq[16];	/* changes from last to current */
	register char		*cp = new_seq;

	/*
	 *	Bail if this is an IP fragment or if the TCP packet isn't
	 *	`compressible' (i.e., ACK isn't set or some other control
	 *	bit is set). (We assume that the caller has already made
 	 *	sure the packet is IP proto TCP)
	 */
	if (ip->ip_fragment_off || bp->it_count < 40)
		goto type_ip;

	th = (TCP_H *)&((int *)ip)[hlen];

	if ((th->th_ctl & (C_SYN|C_FIN|C_RST|C_ACK)) != C_ACK)
	{
	    type_ip:
#ifdef	MSG
		if (CSWT (30))
			printf ("VJ snd: IP\n");
#endif	MSG
		return (PPP_IP);
	}

	/*
	 *	Packet is compressible -- we're going to send either a
	 *	COMPRESSED_TCP or UNCOMPRESSED_TCP packet. Either way
	 *	we need to locate (or create) the connection state.
	 *	Special case the most recently used connection since
	 *	it's most likely to be used again & we don't have to
	 *	do any reordering if it's used
	 */
	if
	(	ip->ip_src_addr != cs->cs_ip.ip_src_addr ||
		ip->ip_dst_addr != cs->cs_ip.ip_dst_addr ||
		*(int *)th != ((int *)&cs->cs_ip)[cs->cs_ip.ip_h_size]
	)
	{
		/*
		 *	Wasn't the first -- search for it.
		 *
		 *	States are kept in a circularly linked list with last_cs
		 *	pointing to the end of the list. The list is kept in lru
		 *	order by moving a state to the head of the list whenever
		 *	it is referenced. Since the list is short and,
		 *	empirically, the connection we want is almost always
		 *	near the front, we locate states via linear search. If
		 *	we don't find a state for the datagram, the oldest state
		 *	is (re-)used.
		 */
		register CSTATE		*lcs, *lastcs = comp->last_cs;

		do
		{
			lcs = cs; cs = cs->cs_next;

			if
			(	ip->ip_src_addr == cs->cs_ip.ip_src_addr &&
				ip->ip_dst_addr == cs->cs_ip.ip_dst_addr &&
				*(int *)th == ((int *)&cs->cs_ip)[cs->cs_ip.ip_h_size]
			)
				goto found;
		}
		while (cs != lastcs);

		/*
		 *	Didn't find it -- re-use oldest cstate. Send an
		 *	uncompressed packet that tells the other side what
		 *	connection number we're using for this conversation. Note
		 *	that since the state list is circular, the oldest state
		 *	points to the newest and we only need to set last_cs to
		 *	update the lru linkage.
		 */
		comp->last_cs = lcs;
		hlen += th->th_h_size; hlen <<= 2;
		goto uncompressed;

		/*
		 *	Found it -- move to the front on the connection list.
		 */
	    found:
		if (lastcs == cs)
		{
			comp->last_cs = lcs;
		}
		else
		{
			lcs->cs_next = cs->cs_next;
			cs->cs_next = lastcs->cs_next;
			lastcs->cs_next = cs;
		}
	}

	/*
	 *	Make sure that only what we expect to change changed. The first
	 *	line of the `if' checks the IP protocol version, header length &
	 *	type of service. The 2nd line checks the "Don't fragment" bit.
	 *	The 3rd line checks the time-to-live and protocol (the protocol
	 *	check is unnecessary but costless). The 4th line checks the TCP
	 *	header length. The 5th line checks IP options, if any. The 6th
	 *	line checks TCP options, if any. If any of these things are
	 *	different between the previous & current datagram, we send the
	 *	current datagram `uncompressed'.
	 */
	oth = (TCP_H *)&((int *)&cs->cs_ip)[hlen];
	ip_hlen = hlen;
	hlen += th->th_h_size; hlen <<= 2;

	if
	(	((ushort *)ip)[0] != ((ushort *)&cs->cs_ip)[0] ||
#define	UNALIGNED
#ifdef	UNALIGNED
		((ulong *)ip)[3]  != ((ulong *)&cs->cs_ip)[3] ||
#else
		((ushort *)ip)[3] != ((ushort *)&cs->cs_ip)[3] ||
		((ushort *)ip)[4] != ((ushort *)&cs->cs_ip)[4] ||
#endif	UNALIGNED
		th->th_h_size != oth->th_h_size ||
		ip_hlen > 5 && !memeq (ip + 1, &cs->cs_ip + 1, (ip_hlen - 5) << 2) ||
		th->th_h_size > 5 && !memeq (th + 1, oth + 1, (th->th_h_size - 5) << 2)
	)
			goto uncompressed;

	/*
	 *	Figure out which of the changing fields changed. The receiver
	 *	expects changes in the order: urgent, window, ack, seq.
	 */
	if   (th->th_ctl & C_URG)
	{
		deltaS = ENDIAN_SHORT (th->th_urg_ptr); ENCODEZ (deltaS);
		changes |= NEW_U;
	}
	elif (th->th_urg_ptr != oth->th_urg_ptr)
	{
		/*
		 *	Argh! URG not set but urp changed -- a sensible
		 *	implementation should never do this but RFC793 doesn't
		 *	prohibit the change, so we have to deal with it.
		 */
		goto uncompressed;
	}

	if (deltaS = ENDIAN_SHORT (th->th_window) - ENDIAN_SHORT (oth->th_window))
		{ ENCODE (deltaS); changes |= NEW_W; }

	if (deltaA = ENDIAN_LONG (th->th_ack_no) - ENDIAN_LONG (oth->th_ack_no))
	{
		if (deltaA > 0xFFFF)
			goto uncompressed;

		ENCODE (deltaA); changes |= NEW_A;
	}

	if (deltaS = ENDIAN_LONG (th->th_seq_no) - ENDIAN_LONG (oth->th_seq_no))
	{
		if (deltaS > 0xFFFF)
			goto uncompressed;

		ENCODE (deltaS); changes |= NEW_S;
	}

	/*
	 *	Look for the special-case encodings
	 */
	switch (changes)
	{
		/*
		 *	Nothing changed. If this packet contains data and the
		 *	last one didn't, this is probably a data packet
		 *	following an ack (normal on an interactive connection)
		 *	and we send it compressed. Otherwise it's probably a
		 *	retransmit, retransmitted ack or window probe. Send it
		 *	uncompressed in case the other side missed the
		 *	compressed version.
		 */
	    case 0:
		if
		(	ip->ip_size != cs->cs_ip.ip_size &&
			ENDIAN_SHORT (cs->cs_ip.ip_size) == hlen
		)
			break;

		/* (fall through) */

		/*
		 *	Actual changes match one of our special case encodings --
		 *	send packet uncompressed.
		 */
	    case SPECIAL_I:
	    case SPECIAL_D:
		goto uncompressed;

		/*
		 *	Special case for echoed terminal traffic
		 */
	    case NEW_S|NEW_A:
		if
		(	deltaS == deltaA &&
			deltaS == ENDIAN_SHORT (cs->cs_ip.ip_size) - hlen
		)
		{
			 changes = SPECIAL_I; cp = new_seq;
		}
		break;

		/*
		 *	Special case for data transfer
		 */
	    case NEW_S:
		if (deltaS == ENDIAN_SHORT (cs->cs_ip.ip_size) - hlen)
			{ changes = SPECIAL_D; cp = new_seq; }
		break;

	}	/* end switch (changes) */

	/*
	 *	Decisão tomada: COMPRESSED!
	 */
	deltaS = ENDIAN_SHORT (ip->ip_id) - ENDIAN_SHORT (cs->cs_ip.ip_id);

	if (deltaS != 1)
		{ ENCODEZ (deltaS); changes |= NEW_I; }

	if (th->th_ctl & C_PSH)
		changes |= TCP_PSH_BIT;

	/*
	 *	Grab the checksum before we overwrite it below.
	 *	Then update our state with this packet's header.
	 */
	tcp_checksum = ENDIAN_SHORT (th->th_checksum);
	memmove (&cs->cs_ip, ip, hlen);

	/*
	 *	We are going to put the compressed header before the original.
	 *	(cp - new_seq) is the number of bytes we need for compressed
	 *	sequence numbers. In addition we need one byte for the change
	 *	mask, one for the slot id and two for the TCP checksum.
	 *	So, (cp - new_seq) + 4 bytes of header are needed.
	 */
	if ((bp->it_data_count = bp->it_count - hlen) > 0)
		bp->it_data_area = bp->it_area + hlen;

	seq_len = cp - new_seq; cp = bp->it_area;

	if (sp->snd.ppp_vj_slot_compression == 0 || comp->last_snd_slot != cs->cs_slot)
	{
		comp->last_snd_slot = cs->cs_slot;

		vj_hlen = seq_len + 4; cp -= vj_hlen;
		*cp++ = changes|NEW_C; *cp++ = cs->cs_slot;
	}
	else
	{
		vj_hlen = seq_len + 3; cp -= vj_hlen;
		*cp++ = changes;
	}

	bp->it_area -= vj_hlen;
   	bp->it_count = vj_hlen;

	*cp++ = tcp_checksum >> 8; *cp++ = tcp_checksum;
	memmove (cp, new_seq, seq_len);

#ifdef	MSG
	if (CSWT (30))
		printf ("VJ snd: COM (%d)\n", vj_hlen);
#endif	MSG

	return (PPP_VJ_COM);

	/*
	 *	We are going to put a copy of the TCP/IP header before
	 *	the original TCP/IP header, because of the protocol
	 *	field that will be altered.
	 *	Update connection state cs & send uncompressed packet
	 *	('uncompressed' means a regular ip/tcp packet but with the
	 *	'conversation id' we hope to use on future compressed packets
	 *	in the protocol field).
	 */
    uncompressed:
	if ((bp->it_data_count = bp->it_count - hlen) > 0)
		bp->it_data_area = bp->it_area + hlen;

	memmove (&cs->cs_ip, ip, hlen);
	comp->last_snd_slot = cs->cs_slot;

	bp->it_area -= hlen;
	bp->it_count = hlen;

	memmove (bp->it_area, ip, hlen);

	((IP_H *)bp->it_area)->ip_proto = cs->cs_slot;

#ifdef	MSG
	if (CSWT (30))
		printf ("VJ snd: UN (%d)\n", hlen);
#endif	MSG

	return (PPP_VJ_UN);

}	/* end sl_compress_tcp */

/*
 ****************************************************************
 *	Descomprime um datagrama				*
 ****************************************************************
 */
int
sl_uncompress_tcp (ITBLOCK *bp, int proto, SLCOMPRESS *comp)
{
	char			*bufp = bp->it_u_area;
	int			len   = bp->it_u_count;
	int			old_len;
	register char		*cp;
	register unsigned int	hlen, changes, i;
	register TCP_H		*th;
	register CSTATE		*cs;
	register IP_H		*ip;

	/*
	 *	Verifica o tipo
	 */
	switch (proto)
	{
		/*
		 *	Locate the saved state for this connection.
		 *	If the state index is legal, clear the 'discard' flag.
		 */
	    case PPP_VJ_UN:
		ip = (IP_H *)bufp;

		if (ip->ip_proto >= N_SLOT)
			goto bad;

		cs = &comp->rstate[comp->last_rcv_slot = ip->ip_proto];
		comp->flags &= ~SLF_TOSS;

		/*
		 *	Restore the IP protocol field then save a copy of this
		 *	packet header. (The checksum is zeroed in the copy so we
		 *	don't have to zero it each time we process a compressed
		 *	packet.
		 */
		ip->ip_proto = TCP_PROTO;
		hlen = ip->ip_h_size;
		hlen += ((TCP_H *)&((int *)ip)[hlen])->th_h_size; hlen <<= 2;
		memmove (&cs->cs_ip, ip, hlen);
		cs->cs_ip.ip_h_checksum = 0;
		cs->cs_hlen = hlen;
#ifdef	MSG
		if (CSWT (30))
			printf ("VJ rcv: UN (%d)\n", hlen);
#endif	MSG
		return (0);

	    case PPP_VJ_COM:
		break;

	    default:
		goto bad;

	}	/* end switch (proto) */

	/*
	 *	We've got a compressed packet.
	 */
	cp = bufp; changes = *cp++;

	if (changes & NEW_C)
	{
		/*
		 *	Make sure the state index is in range, then grab the
		 *	state. If we have a good state index, clear the
		 *	'discard' flag.
		 */
		if (*cp >= N_SLOT)
			goto bad;

		comp->flags &= ~SLF_TOSS;
		comp->last_rcv_slot = *cp++;
	}
	else
	{
		/*
		 *	This packet has an implicit state index. If we've had a
		 *	line error since the last time we got an explicit state
		 *	index, we have to toss the packet.
		 */
		if (comp->flags & SLF_TOSS)
		{
#ifdef	MSG
			if (CSWT (30))
				printf ("VJ rcv: TOSS\n");
#endif	MSG
			return (-1);
		}
	}

	/*
	 *	Find the state then fill in the TCP checksum and PUSH bit.
	 */
	cs = &comp->rstate[comp->last_rcv_slot];
	hlen = cs->cs_ip.ip_h_size << 2;
	th = (TCP_H *)&((char *)&cs->cs_ip)[hlen];
	th->th_checksum = ENDIAN_SHORT ((cp[0] << 8)|cp[1]); cp += 2;

	if (changes & TCP_PSH_BIT)
		th->th_ctl |= C_PSH;
	else
		th->th_ctl &= ~C_PSH;

	/*
	 *	Fix up the state's ack, seq, urg and win fields based on the
	 *	changemask.
	 */
	switch (changes & SPECIALS_MASK)
	{
	    case SPECIAL_I:
		i = ENDIAN_SHORT (cs->cs_ip.ip_size) - cs->cs_hlen;
		th->th_ack_no = ENDIAN_LONG (ENDIAN_LONG (th->th_ack_no) + i);
		th->th_seq_no = ENDIAN_LONG (ENDIAN_LONG (th->th_seq_no) + i);
		break;

	    case SPECIAL_D:
		th->th_seq_no = ENDIAN_LONG (ENDIAN_LONG (th->th_seq_no) +
			ENDIAN_SHORT (cs->cs_ip.ip_size) - cs->cs_hlen);
		break;

	    default:
		if (changes & NEW_U)
			{ th->th_ctl |= C_URG; DECODEU (th->th_urg_ptr); }
		else
			th->th_ctl &= ~C_URG;

		if (changes & NEW_W)
			DECODES (th->th_window);

		if (changes & NEW_A)
			DECODEL (th->th_ack_no);

		if (changes & NEW_S)
			DECODEL (th->th_seq_no);
		break;

	}	/* end switch (changes & SPECIALS_MASK) */

	/*
	 *	Update the IP ID
	 */
	if (changes & NEW_I)
		{ DECODES (cs->cs_ip.ip_id); }
	else
		cs->cs_ip.ip_id = ENDIAN_SHORT (ENDIAN_SHORT (cs->cs_ip.ip_id) + 1);

	/*
	 *	At this point, cp points to the first byte of data in the
	 *	packet. If we're not aligned on a 4-byte boundary, copy the
	 *	data down so the IP & TCP headers will be aligned. Then back
	 *	up cp by the TCP/IP header length to make room for the
	 *	reconstructed header (we assume the packet we were handed has
	 *	enough space to prepend 128 bytes of header). Adjust the lenght
	 *	to account for the new header & fill in the IP total length.
	 */
	old_len = cp - bufp; len -= old_len;

	/*
	 *	We must have dropped some characters (crc should detect
	 *	this but the old slip framing won't)
	 */
	if (len < 0)
		goto bad;

	if ((int)cp & 3)
	{
		memmove ((void *)((int)cp & ~3), cp, len);
		cp = (char *)((int)cp & ~3);
	}

	cp  -= cs->cs_hlen;
	len += cs->cs_hlen;

	if (cp < bp->it_frame)
	{
		printf ("sl_uncompress_tcp: NÃO há espaço em ITBLOCK\n");
		return (-1);
	}

	cs->cs_ip.ip_size = ENDIAN_SHORT (len);
	memmove (cp, &cs->cs_ip, cs->cs_hlen);

	bp->it_u_area  = cp;
	bp->it_u_count = len;

	/*
	 *	Recompute the ip header checksum
	 */
	ip = (IP_H *)cp;
   /***	ip->ip_h_checksum = 0; ***/
   	ip->ip_h_checksum = compute_checksum (ip, hlen);

#ifdef	MSG
	if (CSWT (30))
		printf ("VJ rcv: COM (%d)\n", old_len);
#endif	MSG

	return (0);

	/*
	 *	Houve algum erro
	 */
    bad:
	comp->flags |= SLF_TOSS;
	return (-1);

}	/* end sl_uncompress_tcp */
