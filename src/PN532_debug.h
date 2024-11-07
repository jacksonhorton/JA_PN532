#ifndef __DEBUG_H__
#define __DEBUG_H__

//#define PN532_DEBUG

#include "Particle.h"

#ifdef PN532_DEBUG
#define DMSG(args...)       Log.info(args)
#define DMSG_STR(str)       Log.info("%s", str)
#define DMSG_HEX(num)       Log.info(" %X", num)
#define DMSG_INT(num)       Log.info(" %d", num)
#else
#define DMSG(args...)
#define DMSG_STR(str)
#define DMSG_HEX(num)
#define DMSG_INT(num)
#endif

#endif
