/** @file debugmsg.h
 *
 *  Utilities needed for debugging only. */

/*
 *  GiNaC Copyright (C) 1999-2001 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __GINAC_DEBUGMSG_H__
#define __GINAC_DEBUGMSG_H__

#include <iostream>

#ifdef _DEBUG
#define VERBOSE
#define DO_GINAC_ASSERT (VERBOSE||DEBUG)
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
// #define LOGMASK (LOGLEVEL_PRINT | LOGLEVEL_ASSIGNMENT | LOGLEVEL_OPERATOR | LOGLEVEL_DUPLICATE | LOGLEVEL_OPERATOR | LOGLEVEL_MEMBER_FUNCTION | LOGLEVEL_NONMEMBER_FUNCTION)

#ifdef VERBOSE
#define debugmsg(msg, loglevel) if ((loglevel) & ~LOGMASK) std::clog << (msg) << std::endl;
#else
#define debugmsg(msg, loglevel)
#endif // def VERBOSE

#endif // ndef __GINAC_DEBUGMSG_H__
