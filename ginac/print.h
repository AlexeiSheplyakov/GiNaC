/** @file print.h
 *
 *  Definition of helper classes for expression output. */

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

#ifndef __GINAC_PRINT_H__
#define __GINAC_PRINT_H__

#include <iosfwd>
#include <string>

namespace GiNaC {


/*
 *  The following classes remain publicly visible for compatibility
 *  reasons only. New code should use the iostream manipulators defined
 *  in operators.h instead.
 */


/** Flags to control the behavior of a print_context. */
class print_options {
public:
	enum {
		print_index_dimensions = 0x0001 ///< print the dimensions of indices
	};
};


/** Context for default (ginsh-parsable) output. */
class print_context
{
public:
	print_context();
	print_context(std::ostream &, unsigned options = 0);
	virtual ~print_context() {}
	virtual print_context * duplicate() const {return new print_context(*this);}

	std::ostream & s; /**< stream to output to */
	unsigned options; /**< option flags */
};

/** Context for latex-parsable output. */
class print_latex : public print_context
{
public:
	print_latex();
	print_latex(std::ostream &, unsigned options = 0);
	print_context * duplicate() const {return new print_latex(*this);}
};

/** Context for python pretty-print output. */
class print_python : public print_context
{
public:
	print_python();
	print_python(std::ostream &, unsigned options = 0);
	print_context * duplicate() const {return new print_python(*this);}
};

/** Context for python-parsable output. */
class print_python_repr : public print_context
{
public:
	print_python_repr();
	print_python_repr(std::ostream &, unsigned options = 0);
	print_context * duplicate() const {return new print_python_repr(*this);}
};

/** Context for tree-like output for debugging. */
class print_tree : public print_context
{
public:
	print_tree(unsigned d = 4);
	print_tree(std::ostream &, unsigned options = 0, unsigned d = 4);
	print_context * duplicate() const {return new print_tree(*this);}

	const unsigned delta_indent; /**< size of indentation step */
};

/** Base context for C source output. */
class print_csrc : public print_context
{
public:
	print_csrc();
	print_csrc(std::ostream &, unsigned options = 0);
	print_context * duplicate() const {return new print_csrc(*this);}
};

/** Context for C source output using float precision. */
class print_csrc_float : public print_csrc
{
public:
	print_csrc_float();
	print_csrc_float(std::ostream &, unsigned options = 0);
	print_context * duplicate() const {return new print_csrc_float(*this);}
};

/** Context for C source output using double precision. */
class print_csrc_double : public print_csrc
{
public:
	print_csrc_double();
	print_csrc_double(std::ostream &, unsigned options = 0);
	print_context * duplicate() const {return new print_csrc_double(*this);}
};

/** Context for C source output using CLN numbers. */
class print_csrc_cl_N : public print_csrc
{
public:
	print_csrc_cl_N();
	print_csrc_cl_N(std::ostream &, unsigned options = 0);
	print_context * duplicate() const {return new print_csrc_cl_N(*this);}
};

/** Check if obj is a T, including base classes. */
template <class T>
inline bool is_a(const print_context & obj)
{ return dynamic_cast<const T *>(&obj)!=0; }

} // namespace GiNaC

#endif // ndef __GINAC_BASIC_H__
