/** @file print.h
 *
 *  Helper classes for expression output. */

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

#ifndef __GINAC_PRINT_H__
#define __GINAC_PRINT_H__

#include <iostream>
#include <string>

namespace GiNaC {

/** Context for default (ginsh-parsable) output. */
class print_context
{
public:
	print_context(std::ostream & os = std::cout) : s(os) {}
	std::ostream & s; /**< stream to output to */

	// dummy virtual function to make the class polymorphic
	virtual void dummy(void) {}
};

/** Context for latex-parsable output. */
class print_latex : public print_context
{
public:
	print_latex(std::ostream & os = std::cout)
	  : print_context(os) {}
};

/** Context for tree-like output for debugging. */
class print_tree : public print_context
{
public:
	print_tree(std::ostream & os = std::cout, unsigned d = 4)
	  : print_context(os), delta_indent(d) {}
	unsigned delta_indent; /**< size of indentation step */
};

/** Base context for C source output. */
class print_csrc : public print_context
{
public:
	print_csrc(std::ostream & os = std::cout)
	  : print_context(os) {}
};

/** Context for C source output using float numbers. */
class print_csrc_float : public print_csrc
{
public:
	print_csrc_float(std::ostream & os = std::cout)
	  : print_csrc(os) {}
};

/** Context for C source output using double numbers. */
class print_csrc_double : public print_csrc
{
public:
	print_csrc_double(std::ostream & os = std::cout)
	  : print_csrc(os) {}
};

/** Context for C source output using CLN numbers. */
class print_csrc_cl_N : public print_csrc
{
public:
	print_csrc_cl_N(std::ostream & os = std::cout)
	  : print_csrc(os) {}
};

/** Check if obj is a T, including base classes. */
template <class T>
inline bool is_a(const print_context & obj)
{ return dynamic_cast<const T *>(&obj)!=0; }

} // namespace GiNaC

#endif // ndef __GINAC_BASIC_H__
