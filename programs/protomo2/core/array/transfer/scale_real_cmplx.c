/*----------------------------------------------------------------------------*
*
*  scale_real_cmplx.c  -  array: pixel value transfer
*
*-----------------------------------------------------------------------------*
*
*  Copyright � 2012 Hanspeter Winkler
*
*  This software is distributed under the terms of the GNU General Public
*  License version 3 as published by the Free Software Foundation.
*
*----------------------------------------------------------------------------*/

#include "scale.h"
#include "exception.h"
#include "mathdefs.h"


/* functions */

extern Status ScaleRealCmplx
              (Size count,
               const void *src,
               void *dst,
               const TransferParam *param)

{
  const Real *s = src;

  if ( src == NULL ) return exception( E_ARGVAL );
  if ( dst == NULL ) return exception( E_ARGVAL );

  scale_fc_inc( Cmplx, -RealMax, RealMax, scale_rset );

  return E_NONE;

}
