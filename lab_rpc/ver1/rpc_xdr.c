/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc.h"

bool_t
xdr_QUEUE (XDR *xdrs, QUEUE *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->num))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->pid))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->result))
		 return FALSE;
	return TRUE;
}