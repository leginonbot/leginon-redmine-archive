/*----------------------------------------------------------------------------*
*
*  mat4vecmul.c  -  4 x 4 matrix operations
*
*-----------------------------------------------------------------------------*
*
*  Copyright � 2012 Hanspeter Winkler
*
*  This software is distributed under the terms of the GNU General Public
*  License version 3 as published by the Free Software Foundation.
*
*----------------------------------------------------------------------------*/

#include "mat4.h"


/* functions */

extern Status Mat4VecMul
              (Coord A[4],
               Coord B[4][4],
               Coord C[4])

{
  Coord c0  =  A[0] * B[0][0]  +  A[1] * B[1][0]  +  A[2] * B[2][0]  +  A[3] * B[3][0];
  Coord c1  =  A[0] * B[0][1]  +  A[1] * B[1][1]  +  A[2] * B[2][1]  +  A[3] * B[3][1];
  Coord c2  =  A[0] * B[0][2]  +  A[1] * B[1][2]  +  A[2] * B[2][2]  +  A[3] * B[3][2];
  Coord c3  =  A[0] * B[0][3]  +  A[1] * B[1][3]  +  A[2] * B[2][3]  +  A[3] * B[3][3];

  C[0] = c0;  C[1] = c1;  C[2] = c2;  C[3] = c3;

  return E_NONE;

}
