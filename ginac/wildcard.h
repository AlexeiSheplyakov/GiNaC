/** @file wildcard.h
 *
 *  Interface to GiNaC's wildcard objects. */

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

#ifndef __GINAC_WILDCARD_H__
#define __GINAC_WILDCARD_H__

#include "ex.h"

namespace GiNaC {


/** This class acts as a wildcard for subs(), match() and has(). An integer
 *  label is used to identify different wildcards. */
class wildcard : public basic
{
	GINAC_DECLARE_REGISTERED_CLASS(wildcard, basic)

	// other constructors
public:
	/** Construct wildcard with specified label. */
	wildcard(unsigned label);

	// functions overriding virtual functions from bases classes
public:
	void print(const print_context & c, unsigned level = 0) const;
	unsigned calchash(void) const;
	bool match(const ex & pattern, lst & repl_lst) const;

	// non-virtual functions in this class
public:
	unsigned get_label(void) const {return label;}

	// member variables
private:
	unsigned label; /**< Label used to distinguish different wildcards */
};


// global functions
inline const wildcard &ex_to_wildcard(const ex &e)
{
	return static_cast<const wildcard &>(*e.bp);
}

/** Create a wildcard object with the specified label. */
inline ex wild(unsigned label = 0)
{
	return wildcard(label);
}

} // namespace GiNaC

#endif // ndef __GINAC_WILDCARD_H__
