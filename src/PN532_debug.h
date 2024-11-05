#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define DEBUG

#include "Particle.h"

#ifdef DEBUG_FUNC
/*
// TODO: check these functions to make sure they work correctly
#define DMSG(args...)       Log.trace(args)
#define DMSG_STR(str)       Log.trace(str)
#define DMSG_HEX(num)       Log.trace(" %X", (num >> 4) & 0x0F); Log.trace("%X",
num & 0x0F);
#define DMSG_INT(num)       Log.trace(" %d", (int)num) #else
*/
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

#endif
