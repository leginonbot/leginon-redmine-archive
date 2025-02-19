/*----------------------------------------------------------------------------*
*
*  i3ionest.h  -  io: i3 input/output
*
*-----------------------------------------------------------------------------*
*
*  Copyright � 2012 Hanspeter Winkler
*
*  This software is distributed under the terms of the GNU General Public
*  License version 3 as published by the Free Software Foundation.
*
*----------------------------------------------------------------------------*/

#ifndef i3ionest_h_
#define i3ionest_h_

#include "i3io.h"


/* prototypes */

extern I3io *I3ioCreateNested
             (I3io *i3io,
              int segm,
              I3ioMeta meta);

extern I3io *I3ioCreateOnlyNested
             (I3io *i3io,
              int segm,
              I3ioMeta meta);

extern I3io *I3ioOpenReadOnlyNested
             (I3io *i3io,
              int segm,
              I3ioMeta *meta);

extern I3io *I3ioOpenReadWriteNested
             (I3io *i3io,
              int segm,
              I3ioMeta *meta);

extern I3io *I3ioOpenUpdateNested
             (I3io *i3io,
              int segm,
              I3ioMeta *meta);


#endif
