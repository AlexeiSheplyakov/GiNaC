/** @file fail.cpp
 *
 *  Implementation of class signaling failure of operation. Considered
 *  obsolete all this stuff ought to be replaced by exceptions. */

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

#include "fail.h"
#include "archive.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(fail, basic)

//////////
// default ctor, dtor, copy ctor assignment operator and helpers
//////////

DEFAULT_CTORS(fail)

//////////
// archiving
//////////

DEFAULT_ARCHIVING(fail)

//////////
// functions overriding virtual functions from bases classes
//////////

DEFAULT_COMPARE(fail)
DEFAULT_PRINT(fail, "FAIL")

} // namespace GiNaC
