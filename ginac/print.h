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

#include "class_info.h"

namespace GiNaC {


/** This class stores information about a registered print_context class. */
class print_context_options {
public:
	print_context_options(const char *n, const char *p, unsigned i)
	 : name(n), parent_name(p), id(i) {}

	const char *get_name() const { return name; }
	const char *get_parent_name() const { return parent_name; }
	unsigned get_id() const { return id; }

private:
	const char *name;         /**< Class name. */
	const char *parent_name;  /**< Name of superclass. */
	unsigned id;              /**< ID number (assigned automatically). */
};

typedef class_info<print_context_options> print_context_class_info;


/** Flags to control the behavior of a print_context. */
class print_options {
public:
	enum {
		print_index_dimensions = 0x0001 ///< print the dimensions of indices
	};
};


/** Macro for inclusion in the declaration of a print_context class.
 *  It declares some functions that are common to all classes derived
 *  from 'print_context' as well as all required stuff for the GiNaC
 *  registry. */
#define GINAC_DECLARE_PRINT_CONTEXT(classname, supername) \
public: \
	typedef supername inherited; \
	friend class function_options; \
private: \
	static GiNaC::print_context_class_info reg_info; \
public: \
	virtual const GiNaC::print_context_class_info &get_class_info() const { return reg_info; } \
	virtual const char *class_name() const { return reg_info.options.get_name(); } \
	\
	classname(); \
	classname * duplicate() const { return new classname(*this); } \
private:

/** Macro for inclusion in the implementation of each print_context class. */
#define GINAC_IMPLEMENT_PRINT_CONTEXT(classname, supername) \
	GiNaC::print_context_class_info classname::reg_info = GiNaC::print_context_class_info(print_context_options(#classname, #supername, next_print_context_id++));

extern unsigned next_print_context_id;


/** Base class for print_contexts. */
class print_context
{
	GINAC_DECLARE_PRINT_CONTEXT(print_context, void)
public:
	print_context(std::ostream &, unsigned options = 0);
	virtual ~print_context() {}

	std::ostream & s; /**< stream to output to */
	unsigned options; /**< option flags */
};

/** Context for default (ginsh-parsable) output. */
class print_dflt : public print_context
{
	GINAC_DECLARE_PRINT_CONTEXT(print_dflt, print_context)
public:
	print_dflt(std::ostream &, unsigned options = 0);
};

/** Context for latex-parsable output. */
class print_latex : public print_context
{
	GINAC_DECLARE_PRINT_CONTEXT(print_latex, print_context)
public:
	print_latex(std::ostream &, unsigned options = 0);
};

/** Context for python pretty-print output. */
class print_python : public print_context
{
	GINAC_DECLARE_PRINT_CONTEXT(print_python, print_context)
public:
	print_python(std::ostream &, unsigned options = 0);
};

/** Context for python-parsable output. */
class print_python_repr : public print_context
{
	GINAC_DECLARE_PRINT_CONTEXT(print_python_repr, print_context)
public:
	print_python_repr(std::ostream &, unsigned options = 0);
};

/** Context for tree-like output for debugging. */
class print_tree : public print_context
{
	GINAC_DECLARE_PRINT_CONTEXT(print_tree, print_context)
public:
	print_tree(unsigned d);
	print_tree(std::ostream &, unsigned options = 0, unsigned d = 4);

	const unsigned delta_indent; /**< size of indentation step */
};

/** Base context for C source output. */
class print_csrc : public print_context
{
	GINAC_DECLARE_PRINT_CONTEXT(print_csrc, print_context)
public:
	print_csrc(std::ostream &, unsigned options = 0);
};

/** Context for C source output using float precision. */
class print_csrc_float : public print_csrc
{
	GINAC_DECLARE_PRINT_CONTEXT(print_csrc_float, print_csrc)
public:
	print_csrc_float(std::ostream &, unsigned options = 0);
};

/** Context for C source output using double precision. */
class print_csrc_double : public print_csrc
{
	GINAC_DECLARE_PRINT_CONTEXT(print_csrc_double, print_csrc)
public:
	print_csrc_double(std::ostream &, unsigned options = 0);
};

/** Context for C source output using CLN numbers. */
class print_csrc_cl_N : public print_csrc
{
	GINAC_DECLARE_PRINT_CONTEXT(print_csrc_cl_N, print_csrc)
public:
	print_csrc_cl_N(std::ostream &, unsigned options = 0);
};

/** Check if obj is a T, including base classes. */
template <class T>
inline bool is_a(const print_context & obj)
{ return dynamic_cast<const T *>(&obj) != 0; }

} // namespace GiNaC

#endif // ndef __GINAC_BASIC_H__
