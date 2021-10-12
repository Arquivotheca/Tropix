/*
 ****************************************************************
 *								*
 *			usbdi.c					*
 *								*
 *	"Driver" para dispositivo USB				*
 *								*
 *	Versão	4.3.0, de 07.10.02				*
 *		4.6.0, de 27.09.04				*
 *								*
 *	Módulo: Núcleo						*
 *		NÚCLEO do TROPIX para PC			*
 *								*
 *	TROPIX: Sistema Operacional Tempo-Real Multiprocessado	*
 *		Copyright © 2004 NCE/UFRJ - tecle "man licença"	*
 *		Baseado no FreeBSD 5.2				*
 *								*
 ****************************************************************
 */

#include "../h/common.h"
#include "../h/sync.h"

#include "../h/usb.h"

#define	USB_TST_STACK

#ifdef	USB_TST_STACK
#include "../h/signal.h"
#include "../h/region.h"
#include "../h/uproc.h"
#endif	USB_TST_STACK

#include "../h/extern.h"
#include "../h/proto.h"

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_open_pipe (struct usbd_interface *iface, uchar address,
	       uchar flags, struct usbd_pipe **pipe_ptr)
{ 
	return (usbd_open_pipe_ival (iface, address, flags, pipe_ptr, USBD_DEFAULT_INTERVAL));

}	/* end usbd_open_pipe */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int 
usbd_open_pipe_ival (struct usbd_interface *iface, uchar address,
		    uchar flags, struct usbd_pipe **pipe_ptr, int ival)
{ 
	struct usbd_pipe	*p;
	struct usbd_endpoint	*ep;
	int			err, i;

#ifdef	USB_MSG
	printf ("usbd_open_pipe: iface=%p address=0x%x flags=0x%x\n", iface, address, flags);
#endif	USB_MSG

	for (i = 0; i < iface->idesc->bNumEndpoints; i++)
	{
		ep = &iface->endpoints[i];

		if (ep->edesc == NULL)
			return (USBD_IOERROR);

		if (ep->edesc->bEndpointAddress == address)
			goto found;
	}

	return (USBD_BAD_ADDRESS);

    found:
	if ((flags & USBD_EXCLUSIVE_USE) && ep->refcnt != 0)
		return (USBD_IN_USE);

	if (err = usbd_setup_pipe (iface->device, iface, ep, ival, &p))
		return (err);

	if ((p->next_pipe = iface->pipe_list) != NULL)
		iface->pipe_list->prev_pipe = &p->next_pipe;

	iface->pipe_list = p; p->prev_pipe = &iface->pipe_list;	

	*pipe_ptr = p;

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_open_pipe_ival */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int 
usbd_open_pipe_intr (struct usbd_interface *iface, uchar address, uchar flags, struct usbd_pipe **pipe_ptr,
		    void * priv, void *buffer, ulong len, usbd_callback cb, int ival)
{
	int			err;
	struct usbd_xfer	*xfer;
	struct usbd_pipe	*ipipe;

#ifdef	USB_MSG
	printf ("usbd_open_pipe_intr: address=0x%x flags=0x%x len=%d\n", address, flags, len);
#endif	USB_MSG

	if (err = usbd_open_pipe_ival (iface, address, USBD_EXCLUSIVE_USE, &ipipe, ival))
		return (err);

	if ((xfer = usbd_alloc_xfer (iface->device)) == NULL)
		{ err = USBD_NOMEM; goto bad1; }

	usbd_setup_xfer (xfer, ipipe, priv, buffer, len, flags, USBD_NO_TIMEOUT, cb);

	ipipe->intrxfer	= xfer;
	ipipe->repeat	= 1;

	err = usbd_transfer (xfer);

	*pipe_ptr = ipipe;

	if (err != 0 && err != USBD_IN_PROGRESS)
		goto bad2;

	return (USBD_NORMAL_COMPLETION);

 bad2:
	ipipe->intrxfer	= NULL;
	ipipe->repeat	= 0;

	usbd_free_xfer (xfer);

 bad1:
	usbd_close_pipe (ipipe);
	return (err);

}	/* end usbd_open_pipe_intr */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_close_pipe (struct usbd_pipe *pipe_ptr)
{
	if (pipe_ptr == NULL)
		return (USBD_NORMAL_COMPLETION);

	if (--pipe_ptr->refcnt != 0)
		return (USBD_NORMAL_COMPLETION);

	if (pipe_ptr->first_xfer != NULL)
		return (USBD_PENDING_REQUESTS);

	if (pipe_ptr->next_pipe != NULL)
		pipe_ptr->next_pipe->prev_pipe = pipe_ptr->prev_pipe;

	*pipe_ptr->prev_pipe = pipe_ptr->next_pipe;

	pipe_ptr->endpoint->refcnt--;

	pipe_ptr->methods->close (pipe_ptr);

	if (pipe_ptr->intrxfer != NULL)
		usbd_free_xfer(pipe_ptr->intrxfer);

	free_byte (pipe_ptr);

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_close_pipe */

/*
 ****************************************************************
 *	Dequeue all pipe operations				*
 ****************************************************************
 */
int
usbd_ar_pipe (struct usbd_pipe *pipe_ptr)
{
	struct usbd_xfer	*xfer;

	pipe_ptr->repeat	= 0;
	pipe_ptr->aborting	= 1;

	while ((xfer = pipe_ptr->first_xfer) != NULL)
		pipe_ptr->methods->abort (xfer);

	pipe_ptr->aborting = 0;

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_ar_pipe */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int 
usbd_abort_pipe (struct usbd_pipe *pipe_ptr)
{
	int			err, s;
	struct usbd_bus		*bus = pipe_ptr->device->bus;

	if (pipe_ptr == NULL)
		return (USBD_NORMAL_COMPLETION);

	s = splx (irq_pl_vec[bus->irq]);

	err = usbd_ar_pipe (pipe_ptr);

	splx (s);

	return (err);

}	/* end usbd_abort_pipe */

/*
 ****************************************************************
 *	Inicializa um descritor de transferência		*
 ****************************************************************
 */
void
usbd_setup_xfer (struct usbd_xfer *xfer, struct usbd_pipe *pipe_ptr, void *priv,
		 void *buffer, ulong length, ushort flags, ulong timeout, usbd_callback callback)
{
#ifdef	USB_TST_STACK
	if (buffer >= (void *)&u && buffer < (void *)&u + UPROCSZ)
		panic ("usbd_setup_xfer: área de transferência na pilha do processo: %P\n", buffer);
#endif	USB_TST_STACK

	xfer->pipe	= pipe_ptr;
	xfer->priv	= priv;
	xfer->buffer	= buffer;
	xfer->length	= length;
	xfer->actlen	= 0;
	xfer->flags	= flags;
	xfer->timeout	= timeout;
	xfer->status	= USBD_NOT_STARTED;
	xfer->callback	= callback;
	xfer->rqflags  &= ~URQ_REQUEST;
	xfer->nframes	= 0;

}	/* end usbd_setup_xfer */

/*
 ****************************************************************
 *	Inicializa o descritor de transferência com o "default"	*
 ****************************************************************
 */
void
usbd_setup_default_xfer (struct usbd_xfer *xfer, struct usbd_device *udev, void *priv, ulong timeout,
			 struct usb_device_request *req, void *buffer, ulong length, ushort flags,
			 usbd_callback callback)
{
#ifdef	USB_TST_STACK
	if (buffer >= (void *)&u && buffer < (void *)&u + UPROCSZ)
		panic ("usbd_setup_xfer: área de transferência na pilha do processo: %P\n", buffer);
#endif	USB_TST_STACK

	xfer->pipe	= udev->default_pipe;
	xfer->priv	= priv;
	xfer->buffer	= buffer;
	xfer->length	= length;
	xfer->actlen	= 0;
	xfer->flags	= flags;
	xfer->timeout	= timeout;
	xfer->status	= USBD_NOT_STARTED;
	xfer->callback	= callback;
	xfer->request	= *req;
	xfer->rqflags  |= URQ_REQUEST;
	xfer->nframes	= 0;

}	/* end usbd_setup_default_xfer */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
struct usb_endpoint_descriptor *
usbd_interface2endpoint_descriptor (struct usbd_interface *iface, uchar index)
{
	if (index >= iface->idesc->bNumEndpoints)
		return (NULL);

	return (iface->endpoints[index].edesc);

}	/* end usbd_interface2endpoint_descriptor */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int 
usbd_clear_endpoint_stall_async (struct usbd_pipe *pipe_ptr)
{
	struct usbd_device		*udev = pipe_ptr->device;
	struct usb_device_request	req;

	pipe_ptr->methods->cleartoggle (pipe_ptr);

	req.bmRequestType	= UT_WRITE_ENDPOINT;
	req.bRequest		= UR_CLEAR_FEATURE;

	USETW (req.wValue, UF_ENDPOINT_HALT);
	USETW (req.wIndex, pipe_ptr->endpoint->edesc->bEndpointAddress);
	USETW (req.wLength, 0);

	return (usbd_do_request_async (udev, &req, 0));

}	/* end usbd_clear_endpoint_stall_async */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int 
usbd_device2interface_handle (struct usbd_device *udev, uchar ifaceno, struct usbd_interface **iface)
{
	if (udev->cdesc == NULL)
		return (USBD_NOT_CONFIGURED);

	if (ifaceno >= udev->cdesc->bNumInterface)
		return (USBD_INVAL);

	*iface = &udev->ifaces[ifaceno];

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_device2interface_handle */

/*
 ****************************************************************
 *	Inicia a próxima operação				*
 ****************************************************************
 */
void
usbd_start_next (struct usbd_pipe *pipe_ptr)
{
	struct usbd_xfer	*xfer;
	int			err;

	if ((xfer = pipe_ptr->first_xfer) == NULL)
		{ pipe_ptr->running = 0; return; }

	if ((err = pipe_ptr->methods->start (xfer)) != USBD_IN_PROGRESS)
	{
		printf ("usbd_start_next: error=%d\n", err);

		pipe_ptr->running = 0;

		/* XXX do what? */
	}

}	/* end usbd_start_next */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
void
usbd_do_request_async_cb (struct usbd_xfer *xfer, void *priv, int status)
{
#ifdef	USB_MSG
	if (xfer->actlen > xfer->length)
	{
		printf
		(	"usbd_do_request_async_cb: overrun addr=%d type=0x%02X req=0x"
			"%02X val=%d index=%d rlen=%d length=%d actlen=%d\n",
			xfer->pipe->device->address, 
			xfer->request.bmRequestType,
			xfer->request.bRequest,
			UGETW (xfer->request.wValue),
			UGETW (xfer->request.wIndex), 
			UGETW (xfer->request.wLength), 
			xfer->length,
			xfer->actlen
		);
	}
#endif	USB_MSG

	usbd_free_xfer (xfer);

}	/* end usbd_do_request_async_cb */

/*
 ****************************************************************
 *	Executa um pedido assincronamente			*
 ****************************************************************
 */
int
usbd_do_request_async (struct usbd_device *udev, struct usb_device_request *req, void *data)
{
	struct usbd_xfer	*xfer;
	int			err;

	if ((xfer = usbd_alloc_xfer (udev)) == NULL)
		return (USBD_NOMEM);

	usbd_setup_default_xfer
	(	xfer, udev, 0, USBD_DEFAULT_TIMEOUT, req,
		data, UGETW (req->wLength), 0, usbd_do_request_async_cb
	);

	if ((err = usbd_transfer (xfer)) != 0 && err != USBD_IN_PROGRESS)
		{ usbd_free_xfer (xfer); return (err); }

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_do_request_async */

/*
 ****************************************************************
 *	Aloca a área para DMA					*
 ****************************************************************
 */
void *
usbd_alloc_buffer (struct usbd_xfer *xfer, ulong size)
{
	if ((xfer->dmabuf = malloc_byte (size)) == NULL)
		return (NULL);

	xfer->rqflags |= URQ_DEV_DMABUF;

	return (xfer->dmabuf);

}	/* end usbd_alloc_buffer */

/*
 ****************************************************************
 *	Libera a área para DMA					*
 ****************************************************************
 */
void
usbd_free_buffer (struct usbd_xfer *xfer)
{
	xfer->rqflags &= ~(URQ_DEV_DMABUF | URQ_AUTO_DMABUF);

	free_byte (xfer->dmabuf); 	xfer->dmabuf = NULL;

}	/* end usbd_free_buffer */

/*
 ****************************************************************
 *	Insere um pedido de transferência			*
 ****************************************************************
 */
int
usb_insert_transfer (struct usbd_xfer *xfer)
{
	struct usbd_pipe	*pipe_ptr = xfer->pipe;
	struct usbd_bus		*bus = pipe_ptr->device->bus;
	int			err;
	int			s;

#ifdef	USB_MSG
	printf
	(	"usb_insert_transfer: pipe = %P running = %d timeout = %d\n", 
		pipe_ptr, pipe_ptr->running, xfer->timeout
	);
#endif	USB_MSG

	s = splx (irq_pl_vec[bus->irq]);

	xfer->next_xfer		= NULL;
	*pipe_ptr->last_xfer	= xfer;
	pipe_ptr->last_xfer	= &xfer->next_xfer;

	if (pipe_ptr->running)
		err = USBD_IN_PROGRESS;
	else
		{ pipe_ptr->running = 1; err = USBD_NORMAL_COMPLETION; }

	splx (s);

	return (err);

}	/* end usb_insert_transfer */

/*
 ****************************************************************
 *	Inicia uma transferência				*
 ****************************************************************
 */
int
usbd_transfer (struct usbd_xfer *xfer)
{
	struct usbd_pipe	*pipe_ptr = xfer->pipe;
	int			err;

#ifdef	USB_MSG
	printf
	(	"usbd_transfer: xfer = %P, flags = %d, pipe_ptr = %P, running = %d, dma_addr = %P, buffer = %P\n",
		xfer, xfer->flags, pipe_ptr, pipe_ptr->running, xfer->dmabuf, xfer->buffer
	);

	printf ("usbd_transfer: PILHA = %d\n", (void *)&err - (void *)&u.u_reser4);
#endif	USB_MSG

if (xfer->buffer >= (void *)&u && xfer->buffer < (void *)&u + UPROCSZ)
{
	printf ("%g: Área na PILHA\n");
	print_calls ();
}





   	EVENTCLEAR (&xfer->done);

	if (pipe_ptr->aborting)
		return (USBD_CANCELLED);

	/*
	 *	Aloca a área para DMA, se ainda não tiver sido alocada
	 */
	if ((xfer->rqflags & URQ_DEV_DMABUF) == 0 && xfer->length != 0)
	{
#if (0)	/*******************************************************/
		if ((xfer->dmabuf = malloc_byte (xfer->length)) == NULL)
			return (USBD_NOMEM);
#endif	/*******************************************************/
		xfer->dmabuf = xfer->buffer;

		xfer->rqflags |= URQ_AUTO_DMABUF;
	}

	/*
	 *	Se for escrita, copia os dados para a área de DMA
	 */
	if
	(	xfer->dmabuf != xfer->buffer && (xfer->flags & USBD_NO_COPY) == 0 &&
		xfer->length != 0 && !usbd_xfer_isread (xfer)
	)
		memmove (xfer->dmabuf, xfer->buffer, xfer->length);

	/*
	 *	Realiza a transferência
	 */
	if ((err = pipe_ptr->methods->transfer (xfer)) != 0 && err != USBD_IN_PROGRESS)
	{
		/* A transferência NÃO foi agendada: libera a área de DMA */

		if (xfer->rqflags & URQ_AUTO_DMABUF)
		{
#if (0)	/*******************************************************/
			free_byte (xfer->dmabuf); xfer->dmabuf = NULL;
#endif	/*******************************************************/
			xfer->dmabuf = NULL;

			xfer->rqflags &= ~URQ_AUTO_DMABUF;
		}
	}

	if ((xfer->flags & USBD_SYNCHRONOUS) == 0)
		return (err);

	/*
	 *	Transferência síncrona: espera terminar.
	 */
	if (err != USBD_IN_PROGRESS)
		return (err);

#if (0)	/*******************************************************/
	if (pipe_ptr->device->bus->use_polling)
	{
		if (EVENTTEST (&xfer->done) < 0)
			panic ("usbd_transfer: not done\n");
	}
	else
#endif	/*******************************************************/
	{
		EVENTWAIT (&xfer->done, PBLKIO);
	}

	return (xfer->status);

}	/* end usbd_transfer */

/*
 ****************************************************************
 *	Finaliza a transferência				*
 ****************************************************************
 */
void
usb_transfer_complete (struct usbd_xfer *xfer)
{
	struct usbd_pipe	*pipe_ptr = xfer->pipe;
	int			repeat    = pipe_ptr->repeat;
	int			polling = 0;

#ifdef	USB_MSG
	printf
	(	"usb_transfer_complete: pipe = %P xfer = %P status =%d actlen = %d, dma_addr = %P\n",
		pipe_ptr, xfer, xfer->status, xfer->actlen, xfer->dmabuf
	);
#endif	USB_MSG

#if (0)	/*******************************************************/
	if ((polling = pipe_ptr->device->bus->use_polling) != 0)
		pipe_ptr->running = 0;
#endif	/*******************************************************/

	if
	(	xfer->dmabuf != xfer->buffer && (xfer->flags & USBD_NO_COPY) == 0 &&
		xfer->actlen != 0 && usbd_xfer_isread (xfer)
	)
		memmove (xfer->buffer, xfer->dmabuf, xfer->actlen);

	/* Se "xfer->dmabuf" foi alocado em "usbd_transfer", libera-o aqui */

	if (xfer->rqflags & URQ_AUTO_DMABUF)
	{
		if (!repeat)
		{
#if (0)	/*******************************************************/
			free_byte (xfer->dmabuf); xfer->dmabuf = NULL;
#endif	/*******************************************************/
			xfer->dmabuf = NULL;

			xfer->rqflags &= ~URQ_AUTO_DMABUF;
		}
	}

	if (!repeat)
	{
		/* Remove request from queue */

		if ((pipe_ptr->first_xfer = pipe_ptr->first_xfer->next_xfer) == NULL)
			pipe_ptr->last_xfer = &pipe_ptr->first_xfer;
	}

#ifdef	USB_MSG
	printf
	(	"usb_transfer_complete: repeat = %d new head = %P\n", 
		repeat, pipe_ptr->first_xfer
	);
#endif	USB_MSG

	/*
	 *	Contabiliza o número de transferências realizadas
	 */
	++pipe_ptr->device->bus->stats.uds_requests[pipe_ptr->endpoint->edesc->bmAttributes & UE_XFERTYPE];

	if (!xfer->status && xfer->actlen < xfer->length && (xfer->flags & USBD_SHORT_XFER_OK) == 0)
		xfer->status = USBD_SHORT_XFER;

	/*
	 *	For repeat operations, call the callback first, as the xfer
	 *	will not go away and the "done" method may modify it. Otherwise
	 *	reverse the order in case the callback wants to free or reuse
	 *	the xfer.
	 */
	if (repeat)
	{
		if (xfer->callback)
			(*xfer->callback) (xfer, xfer->priv, xfer->status);

		(*pipe_ptr->methods->done) (xfer);
	}
	else
	{
		(*pipe_ptr->methods->done) (xfer);

		if (xfer->callback)
			(*xfer->callback) (xfer, xfer->priv, xfer->status);
	}

#if (0)	/*******************************************************/
	/*
	 *	Executa o "callback"
	 */
	if (xfer->callback != NULL)
		(*xfer->callback) (xfer, xfer->priv, xfer->status);

	if (pipe_ptr->methods->done == NULL)
		printf ("usb_transfer_complete: pipe_ptr->methods->done == NULL\n");
	else
		pipe_ptr->methods->done (xfer);
#endif	/*******************************************************/

	if   (polling)
		EVENTSET (&xfer->done);
	elif (xfer->flags & USBD_SYNCHRONOUS)
		EVENTDONE (&xfer->done);

#if (0)	/*******************************************************/
	if (xfer->flags & USBD_SYNCHRONOUS)
	{
		if (polling)
			EVENTSET  (&xfer->done);
		else
			EVENTDONE (&xfer->done);
	}
#endif	/*******************************************************/

	if (!repeat)
	{
		/* XXX should we stop the queue on all errors? */

		/* not control pipe */

		if ((xfer->status == USBD_CANCELLED || xfer->status == USBD_TIMEOUT) &&	pipe_ptr->iface != NULL)
			pipe_ptr->running = 0;
		else
			usbd_start_next (pipe_ptr);
	}

}	/* end usb_transfer_complete */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int 
usbd_clear_endpoint_stall (struct usbd_pipe *pipe_ptr)
{
	struct usbd_device		*udev = pipe_ptr->device;
	struct usb_device_request	req;

	/* 
	 *	Clearing en endpoint stall resets the endpoint toggle, so
	 *	do the same to the HC toggle.
	 */
	pipe_ptr->methods->cleartoggle (pipe_ptr);

	req.bmRequestType	= UT_WRITE_ENDPOINT;
	req.bRequest		= UR_CLEAR_FEATURE;

	USETW (req.wValue, UF_ENDPOINT_HALT);
	USETW (req.wIndex, pipe_ptr->endpoint->edesc->bEndpointAddress);
	USETW (req.wLength, 0);

	return (usbd_do_request (udev, &req, 0));

}	/* end usbd_clear_endpoint_stall */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_do_request (struct usbd_device *udev, struct usb_device_request *req, void *data)
{
	return (usbd_do_request_flags (udev, req, data, 0, 0, USBD_DEFAULT_TIMEOUT));

}	/* end usbd_do_request */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_do_request_flags (struct usbd_device *udev, struct usb_device_request *req,
		      void *data, ushort flags, int *actlen, ulong timo)
{
	return (usbd_do_request_flags_pipe (udev, udev->default_pipe, req, data, flags, actlen, timo));

}	/* end usbd_do_request_flags */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_do_request_flags_pipe (struct usbd_device *udev, struct usbd_pipe *pipe_ptr,
	struct usb_device_request *req, void *data, ushort flags, int *actlen, ulong timeout)
{
	int			err;
	struct usbd_xfer	*xfer;
	struct usb_status	*status = NULL;

#ifdef	USB_MSG
	printf
	(	"usbd_do_request_flags_pipe: request = %s (%d)\n",
		request_str[req->bRequest], req->bRequest
	);
#endif	USB_MSG

	if ((xfer = usbd_alloc_xfer (udev)) == NULL)
		return (USBD_NOMEM);

	usbd_setup_default_xfer (xfer, udev, 0, timeout, req, data, UGETW (req->wLength), flags, 0);

	xfer->pipe = pipe_ptr;

	err = usbd_sync_transfer (xfer);

if (err != 0)
{
printf ("%g: usbd_sync_transfer retornou erro %d, actlen = %d\n", err, xfer->actlen);
print_calls ();
}


	if (actlen != NULL)
	{
#if (0)	/*******************************************************/
printf ("%g: actlen = %d\n", xfer->actlen);
#endif	/*******************************************************/
		*actlen = xfer->actlen;
	}

	if (err == USBD_STALLED)
	{
		/* 
		 *	The control endpoint has stalled.  Control endpoints
		 *	should not halt, but some may do so anyway so clear
		 *	any halt condition.
		 */
		struct usb_device_request	treq;
		ushort				s;
		int				nerr;

		if ((status = malloc_byte (sizeof (struct usb_status))) == NULL)
			{ err = USBD_NOMEM; goto bad; }

		treq.bmRequestType	= UT_READ_ENDPOINT;
		treq.bRequest		= UR_GET_STATUS;

		USETW (treq.wValue, 0);
		USETW (treq.wIndex, 0);
		USETW (treq.wLength, sizeof (struct usb_status));

		usbd_setup_default_xfer
		(	xfer, udev, 0, USBD_DEFAULT_TIMEOUT,
			&treq, status, sizeof (struct usb_status),
			0, 0
		);

		if (nerr = usbd_sync_transfer (xfer))
			goto bad;

		if (((s = UGETW (status->wStatus)) & UES_HALT) == 0)
		{
			printf ("usbd_do_request_flags_pipe: status = 0x%04X\n", s);
			goto bad;
		}

		treq.bmRequestType	= UT_WRITE_ENDPOINT;
		treq.bRequest		= UR_CLEAR_FEATURE;

		USETW (treq.wValue, UF_ENDPOINT_HALT);
		USETW (treq.wIndex, 0);
		USETW (treq.wLength, 0);

		usbd_setup_default_xfer (xfer, udev, 0, USBD_DEFAULT_TIMEOUT, &treq, status, 0, 0, 0);

		if (nerr = usbd_sync_transfer (xfer))
			goto bad;
	}

	/*
	 *	Em caso de erro, ...
	 */
    bad:
	if (status != NULL)
		free_byte (status);

	usbd_free_xfer (xfer);

	return (err);

}	/* end usbd_do_request_flags_pipe  */

/*
 ****************************************************************
 *	Like usbd_transfer(), but waits for completion		*
 ****************************************************************
 */
int
usbd_sync_transfer (struct usbd_xfer *xfer)
{
	xfer->flags |= USBD_SYNCHRONOUS;

	return (usbd_transfer (xfer));

}	/* end usbd_sync_transfer */

/*
 ****************************************************************
 *	Aloca um descritor de transferência			*
 ****************************************************************
 */
struct usbd_xfer *
usbd_alloc_xfer (struct usbd_device *udev)
{
	struct usbd_xfer	*xfer;

	if ((xfer = udev->bus->methods->allocx (udev->bus)) == NULL)
		return (NULL);

	xfer->device = udev;

#if (0)	/*******************************************************/
	usb_callout_init(xfer->timeout_handle);
#endif	/*******************************************************/

	return (xfer);

}	/* end usbd_alloc_xfer */

/*
 ****************************************************************
 *	Libera um descritor de transferência			*
 ****************************************************************
 */
int 
usbd_free_xfer (struct usbd_xfer *xfer)
{
	if (xfer->rqflags & (URQ_DEV_DMABUF|URQ_AUTO_DMABUF))
		usbd_free_buffer (xfer);

	(*xfer->device->bus->methods->freex) (xfer->device->bus, xfer);

	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_free_xfer */

/*
 ****************************************************************
 *	Verifica se a transferência é uma leitura		*
 ****************************************************************
 */
int
usbd_xfer_isread (struct usbd_xfer *xfer)
{
	if (xfer->rqflags & URQ_REQUEST)
		return (xfer->request.bmRequestType & UT_READ);
	else
		return (xfer->pipe->endpoint->edesc->bEndpointAddress & UE_DIR_IN);

}	/* end usbd_xfer_isread */

/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_set_interface (struct usbd_interface *iface, int altidx)
{
	struct usb_device_request	req;
	int				err;
	void				*endpoints;

	if (iface->pipe_list != NULL)
		return (USBD_IN_USE);

	endpoints = iface->endpoints;

	if (err = usbd_fill_iface_data (iface->device, iface->index, altidx))
		return (err);

	/* new setting works, we can free old endpoints */

	if (endpoints != NULL)
		free_byte (endpoints);

	if (iface->idesc == NULL)
		{ printf ("%g: NULL pointer\n"); return (USBD_INVAL); } 

	req.bmRequestType	= UT_WRITE_INTERFACE;
	req.bRequest		= UR_SET_INTERFACE;

	USETW (req.wValue, iface->idesc->bAlternateSetting);
	USETW (req.wIndex, iface->idesc->bInterfaceNumber);
	USETW (req.wLength, 0);

	return (usbd_do_request (iface->device, &req, 0));

}	/* end usbd_set_interface */

#if (0)	/*******************************************************/
/*
 ****************************************************************
 *	x			*
 ****************************************************************
 */
int
usbd_endpoint_count (struct usbd_interface *iface, uchar *count)
{
	*count = iface->idesc->bNumEndpoints;
	return (USBD_NORMAL_COMPLETION);

}	/* end usbd_endpoint_count */
#endif	/*******************************************************/

/*
 ****************************************************************
 *	Obtém informações sobre o resultado da transferência	*
 ****************************************************************
 */
void
usbd_get_xfer_status (struct usbd_xfer *xfer, void **priv,
		     void **buffer, ulong *count, int *status)
{
	if (priv != NULL)
		*priv = xfer->priv;

	if (buffer != NULL)
		*buffer = xfer->buffer;

	if (count != NULL)
		*count = xfer->actlen;

	if (status != NULL)
		*status = xfer->status;

}	/* end usbd_get_xfer_status */
