/** @file fail.cpp
 *
 *  Implementation of class signaling failure of operation. Considered
 *  somewhat obsolete (most of this can be replaced by exceptions). */

/*
 *  GiNaC Copyright (C) 1999-2005 Johannes Gutenberg University Mainz, Germany
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

#include <iostream>

#include "fail.h"
#include "archive.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS_OPT(fail, basic,
  print_func<print_context>(&fail::do_print).
  print_func<print_tree>(&fail::do_print_tree))

//////////
// default constructor
//////////

DEFAULT_CTOR(fail)

//////////
// archiving
//////////

DEFAULT_ARCHIVING(fail)

//////////
// functions overriding virtual functions from base classes
//////////

DEFAULT_COMPARE(fail)
DEFAULT_PRINT(fail, "FAIL")

} // namespace GiNaC
