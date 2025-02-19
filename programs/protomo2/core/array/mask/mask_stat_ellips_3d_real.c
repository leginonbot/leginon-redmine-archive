/*----------------------------------------------------------------------------*
*
*  mask_stat_ellips_3d_real.c  -  array: mask operations
*
*-----------------------------------------------------------------------------*
*
*  Copyright � 2012 Hanspeter Winkler
*
*  This software is distributed under the terms of the GNU General Public
*  License version 3 as published by the Free Software Foundation.
*
*----------------------------------------------------------------------------*/

#include "maskcommon.h"
#include "exception.h"
#include "mathdefs.h"


/* functions */

extern Status MaskStatEllips3dReal
              (const Size *len,
               void *addr,
               const Coord *A,
               const Coord *b,
               const Coord *w,
               Stat *dst,
               MaskFlags flags)

{
  Real *src = addr;
  Coord B[3*3];
  Coord c[3], p[3];
  Coord r[3], q[3];
  Coord min = +RealMax;
  Coord max = -RealMax;
  Coord mean = 0;
  Size count = 0;

  if ( argcheck( len  == NULL ) ) return exception( E_ARGVAL );
  if ( argcheck( addr == NULL ) ) return exception( E_ARGVAL );
  if ( argcheck( w    == NULL ) ) return exception( E_ARGVAL );
  if ( argcheck( dst  == NULL ) ) return exception( E_ARGVAL );

  MaskSetupParam( 3, len, b, w, c, p, flags );

  if ( A == NULL ) {

    for ( Size z = 0; z < len[2]; z++ ) {

      q[2] = p[2] * ( z - c[2] ); q[2] *= q[2];

      for ( Size y = 0; y < len[1]; y++ ) {

        q[1] = p[1] * ( y - c[1] ); q[1] *= q[1];

        for ( Size x = 0; x < len[0]; x++ ) {

          q[0] = p[0] * ( x - c[0] ); q[0] *= q[0];

          Coord q2 = q[0] + q[1] + q[2];

          if ( ~flags & MaskModeInv ) {
            if ( q2 <= 1 ) {
              if ( *src < min ) min = *src;
              if ( *src > max ) max = *src;
              mean += *src;
              count++;
            }
          } else {
            if ( q2 > 1 ) {
              if ( *src < min ) min = *src;
              if ( *src > max ) max = *src;
              mean += *src;
              count++;
            }
          }

          dst++;

        } /* end for x */

      } /* end for y */

    } /* end for z */

  } else {

    MaskSetupMat( 3, A, p, B );

    for ( Size z = 0; z < len[2]; z++ ) {

      r[2] = z - c[2];

      for ( Size y = 0; y < len[1]; y++ ) {

        r[1] = y - c[1];

        for ( Size x = 0; x < len[0]; x++ ) {

          r[0] = x - c[0];

          MaskMulVec3( B, r, q );

          Coord q2 = q[0]*q[0] + q[1]*q[1] + q[2]*q[2];

          if ( ~flags & MaskModeInv ) {
            if ( q2 <= 1 ) {
              if ( *src < min ) min = *src;
              if ( *src > max ) max = *src;
              mean += *src;
              count++;
            }
          } else {
            if ( q2 > 1 ) {
              if ( *src < min ) min = *src;
              if ( *src > max ) max = *src;
              mean += *src;
              count++;
            }
          }

          dst++;

        } /* end for x */

      } /* end for y */

    } /* end for z */

  }

  dst->count = count;
  dst->min = min;
  dst->max = max;
  dst->mean = count ? mean / count : 0;
  dst->sd = 0;

  return E_NONE;

}
