/** @file wildcard.cpp
 *
 *  Implementation of GiNaC's wildcard objects. */

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

#include <iostream>

#include "wildcard.h"
#include "print.h"
#include "archive.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(wildcard, basic)

//////////
// default ctor, dtor, copy ctor, assignment operator and helpers
//////////

wildcard::wildcard() : label(0)
{
	tinfo_key = TINFO_wildcard;
}

void wildcard::copy(const wildcard & other)
{
	inherited::copy(other);
	label = other.label;
}

DEFAULT_DESTROY(wildcard)

//////////
// other constructors
//////////

wildcard::wildcard(unsigned l) : label(l)
{
	tinfo_key = TINFO_wildcard;
}

//////////
// archiving
//////////

wildcard::wildcard(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	n.find_unsigned("label", label);
}

void wildcard::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_unsigned("label", label);
}

DEFAULT_UNARCHIVE(wildcard)

//////////
// functions overriding virtual functions from base classes
//////////

int wildcard::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_a<wildcard>(other));
	const wildcard &o = static_cast<const wildcard &>(other);

	if (label == o.label)
		return 0;
	else
		return label < o.label ? -1 : 1;
}

void wildcard::print(const print_context & c, unsigned level) const
{
	if (is_a<print_tree>(c)) {
		c.s << std::string(level, ' ') << class_name() << " (" << label << ")"
		    << std::hex << ", hash=0x" << hashvalue << ", flags=0x" << flags << std::dec
		    << std::endl;
	} else if (is_a<print_python_repr>(c)) {
		c.s << class_name() << '(' << label << ')';
	} else
		c.s << "$" << label;
}

unsigned wildcard::calchash(void) const
{
	// this is where the schoolbook method
	// (golden_ratio_hash(tinfo()) ^ label)
	// is not good enough yet...
	hashvalue = golden_ratio_hash(golden_ratio_hash(tinfo()) ^ label);
	setflag(status_flags::hash_calculated);
	return hashvalue;
}

bool wildcard::match(const ex & pattern, lst & repl_lst) const
{
	// Wildcards must match each other exactly (this is required for
	// subs() to work properly because in the final step it substitutes
	// all wildcards by their matching expressions)
	return is_equal(ex_to<basic>(pattern));
}

} // namespace GiNaC
