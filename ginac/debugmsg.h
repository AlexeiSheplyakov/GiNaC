/** @file debugmsg.h
 *
 *  Utilities needed for debugging only. */

#ifndef _DEBUGMSG_H_
#define _DEBUGMSG_H_

#ifdef _DEBUG
#define VERBOSE
#define DOASSERT (VERBOSE||DEBUG)
#endif

#define LOGLEVEL_CONSTRUCT          0x0001
#define LOGLEVEL_DESTRUCT           0x0002
#define LOGLEVEL_ASSIGNMENT         0x0004
#define LOGLEVEL_DUPLICATE          0x0008
#define LOGLEVEL_PRINT              0x0010
#define LOGLEVEL_OPERATOR           0x0020
#define LOGLEVEL_MEMBER_FUNCTION    0x4000
#define LOGLEVEL_NONMEMBER_FUNCTION 0x8000
#define LOGLEVEL_ALL                0xffff

#define LOGMASK (LOGLEVEL_PRINT)
// #define LOGMASK (LOGLEVEL_PRINT | LOGLEVEL_ASSIGNMENT | LOGLEVEL_OPERATOR | LOGLEVEL_DUPLICATE | LOGLEVEL_OPERATOR | LOGLEVEL_MEMBER_FUNCTION | LOGLEVEL_NONMEMBER_FUNCTION )

#include <assert.h>
#include <iostream>

#ifdef VERBOSE
#define debugmsg(msg, loglevel) if ((loglevel) & ~LOGMASK) clog << (msg) << endl;
#else
#define debugmsg(msg, loglevel)
#endif // def VERBOSE

#ifdef DOASSERT
#define ASSERT(X) assert(X)
#else
#define ASSERT(X) ((void)0)
#endif

#endif // ndef _DEBUGMSG_H_

