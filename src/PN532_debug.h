#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DEBUG

#include "Particle.h"

#ifdef DEBUG
#define DMSG(args...)       Log.info(args)
#define DMSG_STR(str)       Log.info(str)
#define DMSG_HEX(num)       Log.info(" "); Log.info("%X", (num >> 4) & 0x0F); Log.info("%X", num & 0x0F);
#define DMSG_INT(num)       Log.info(" "); Log.info("%d", (int)num)
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

#endif
