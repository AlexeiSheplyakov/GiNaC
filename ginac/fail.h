/** @file fail.h
 *
 *  Interface to class signaling failure of operation. Considered obsolete
 *  somewhat obsolete (most of this can be replaced by exceptions). */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
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

#ifndef __GINAC_FAIL_H__
#define __GINAC_FAIL_H__

#include "basic.h"

namespace GiNaC {

class fail : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(fail, basic)
	
	// functions overriding virtual functions from base classes
public:
	void print(const print_context & c, unsigned level = 0) const;
protected:
	unsigned return_type(void) const { return return_types::noncommutative_composite; };
};

/** Specialization of is_exactly_a<fail>(obj) for fail objects. */
template<> inline bool is_exactly_a<fail>(const basic & obj)
{
	return obj.tinfo()==TINFO_fail;
}

} // namespace GiNaC

#endif // ndef __GINAC_FAIL_H__
