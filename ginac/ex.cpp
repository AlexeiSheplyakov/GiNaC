/** @file ex.cpp
 *
 *  Implementation of GiNaC's light-weight expression handles. */

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

#include <iostream>
#include <stdexcept>

#include "ex.h"
#include "add.h"
#include "mul.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h"
#include "lst.h"
#include "relational.h"
#include "input_lexer.h"
#include "utils.h"

namespace GiNaC {

//////////
// other ctors
//////////

// none (all inlined)

//////////
// non-virtual functions in this class
//////////

// public
	
/** Efficiently swap the contents of two expressions. */
void ex::swap(ex & other)
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
	GINAC_ASSERT(other.bp!=0);
	GINAC_ASSERT(other.bp->flags & status_flags::dynallocated);
	
	basic * tmpbp = bp;
	bp = other.bp;
	other.bp = tmpbp;
}

/** Print expression to stream. The formatting of the output is determined
 *  by the kind of print_context object that is passed. Possible formattings
 *  include ginsh-parsable output (the default), tree-like output for
 *  debugging, and C++ source.
 *  @see print_context */
void ex::print(const print_context & c, unsigned level) const
{
	GINAC_ASSERT(bp!=0);
	bp->print(c, level);
}

/** Print expression to stream in a tree-like format suitable for debugging. */
void ex::printtree(std::ostream & os) const
{
	GINAC_ASSERT(bp!=0);
	bp->print(print_tree(os));
}

/** Little wrapper arount print to be called within a debugger. */
void ex::dbgprint(void) const
{
	GINAC_ASSERT(bp!=0);
	bp->dbgprint();
}

/** Little wrapper arount printtree to be called within a debugger. */
void ex::dbgprinttree(void) const
{
	GINAC_ASSERT(bp!=0);
	bp->dbgprinttree();
}

ex ex::expand(unsigned options) const
{
	GINAC_ASSERT(bp!=0);
	if (options == 0 && (bp->flags & status_flags::expanded)) // The "expanded" flag only covers the standard options; someone might want to re-expand with different options
		return *bp;
	else
		return bp->expand(options);
}

/** Compute partial derivative of an expression.
 *
 *  @param s  symbol by which the expression is derived
 *  @param nth  order of derivative (default 1)
 *  @return partial derivative as a new expression */
ex ex::diff(const symbol & s, unsigned nth) const
{
	GINAC_ASSERT(bp!=0);

	if (!nth)
		return *this;
	else
		return bp->diff(s, nth);
}

/** Check whether expression matches a specified pattern. */
bool ex::match(const ex & pattern) const
{
	lst repl_lst;
	return bp->match(pattern, repl_lst);
}

/** Find all occurrences of a pattern. The found matches are appended to
 *  the "found" list. If the expression itself matches the pattern, the
 *  children are not further examined. This function returns true when any
 *  matches were found. */
bool ex::find(const ex & pattern, lst & found) const
{
	if (match(pattern)) {
		found.append(*this);
		found.sort();
		found.unique();
		return true;
	}
	bool any_found = false;
	for (unsigned i=0; i<nops(); i++)
		if (op(i).find(pattern, found))
			any_found = true;
	return any_found;
}

ex ex::operator[](const ex & index) const
{
	GINAC_ASSERT(bp!=0);
	return (*bp)[index];
}

ex ex::operator[](int i) const
{
	GINAC_ASSERT(bp!=0);
	return (*bp)[i];
}

/** Return modifyable operand/member at position i. */
ex & ex::let_op(int i)
{
	makewriteable();
	GINAC_ASSERT(bp!=0);
	return bp->let_op(i);
}

/** Left hand side of relational expression. */
ex ex::lhs(void) const
{
	if (!is_ex_of_type(*this,relational))
		throw std::runtime_error("ex::lhs(): not a relation");
	return (*static_cast<relational *>(bp)).lhs();
}

/** Right hand side of relational expression. */
ex ex::rhs(void) const
{
	if (!is_ex_of_type(*this,relational))
		throw std::runtime_error("ex::rhs(): not a relation");
	return (*static_cast<relational *>(bp)).rhs();
}

// private

/** Make this ex writable (if more than one ex handle the same basic) by 
 *  unlinking the object and creating an unshared copy of it. */
void ex::makewriteable()
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
	if (bp->refcount > 1) {
		basic * bp2 = bp->duplicate();
		++bp2->refcount;
		bp2->setflag(status_flags::dynallocated);
		--bp->refcount;
		bp = bp2;
	}
	GINAC_ASSERT(bp->refcount==1);
}

/** Ctor from basic implementation.
 *  @see ex::ex(const basic &) */
void ex::construct_from_basic(const basic & other)
{
	if (!(other.flags & status_flags::evaluated)) {
		const ex & tmpex = other.eval(1); // evaluate only one (top) level
		bp = tmpex.bp;
		GINAC_ASSERT(bp->flags & status_flags::dynallocated);
		++bp->refcount;
		if ((other.refcount==0) && (other.flags & status_flags::dynallocated))
			delete &const_cast<basic &>(other);
	} else {
		if (other.flags & status_flags::dynallocated) {
			// ok, it is already on the heap, so just copy bp:
			bp = &const_cast<basic &>(other);
		} else {
			// create a duplicate on the heap:
			bp = other.duplicate();
			bp->setflag(status_flags::dynallocated);
		}
		GINAC_ASSERT(bp!=0);
		++bp->refcount;
	}
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
}

void ex::construct_from_int(int i)
{
	switch (i) {  // prefer flyweights over new objects
	case -12:
		bp = (basic*)_num_12_p;
		++bp->refcount;
		break;
	case -11:
		bp = (basic*)_num_11_p;
		++bp->refcount;
		break;
	case -10:
		bp = (basic*)_num_10_p;
		++bp->refcount;
		break;
	case -9:
		bp = (basic*)_num_9_p;
		++bp->refcount;
		break;
	case -8:
		bp = (basic*)_num_8_p;
		++bp->refcount;
		break;
	case -7:
		bp = (basic*)_num_7_p;
		++bp->refcount;
		break;
	case -6:
		bp = (basic*)_num_6_p;
		++bp->refcount;
		break;
	case -5:
		bp = (basic*)_num_5_p;
		++bp->refcount;
		break;
	case -4:
		bp = (basic*)_num_4_p;
		++bp->refcount;
		break;
	case -3:
		bp = (basic*)_num_3_p;
		++bp->refcount;
		break;
	case -2:
		bp = (basic*)_num_2_p;
		++bp->refcount;
		break;
	case -1:
		bp = (basic*)_num_1_p;
		++bp->refcount;
		break;
	case 0:
		bp = (basic*)_num0_p;
		++bp->refcount;
		break;
	case 1:
		bp = (basic*)_num1_p;
		++bp->refcount;
		break;
	case 2:
		bp = (basic*)_num2_p;
		++bp->refcount;
		break;
	case 3:
		bp = (basic*)_num3_p;
		++bp->refcount;
		break;
	case 4:
		bp = (basic*)_num4_p;
		++bp->refcount;
		break;
	case 5:
		bp = (basic*)_num5_p;
		++bp->refcount;
		break;
	case 6:
		bp = (basic*)_num6_p;
		++bp->refcount;
		break;
	case 7:
		bp = (basic*)_num7_p;
		++bp->refcount;
		break;
	case 8:
		bp = (basic*)_num8_p;
		++bp->refcount;
		break;
	case 9:
		bp = (basic*)_num9_p;
		++bp->refcount;
		break;
	case 10:
		bp = (basic*)_num10_p;
		++bp->refcount;
		break;
	case 11:
		bp = (basic*)_num11_p;
		++bp->refcount;
		break;
	case 12:
		bp = (basic*)_num12_p;
		++bp->refcount;
		break;
	default:
		bp = new numeric(i);
		bp->setflag(status_flags::dynallocated);
		++bp->refcount;
		GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
		GINAC_ASSERT(bp->refcount==1);
	}
}
	
void ex::construct_from_uint(unsigned int i)
{
	switch (i) {  // prefer flyweights over new objects
	case 0:
		bp = (basic*)_num0_p;
		++bp->refcount;
		break;
	case 1:
		bp = (basic*)_num1_p;
		++bp->refcount;
		break;
	case 2:
		bp = (basic*)_num2_p;
		++bp->refcount;
		break;
	case 3:
		bp = (basic*)_num3_p;
		++bp->refcount;
		break;
	case 4:
		bp = (basic*)_num4_p;
		++bp->refcount;
		break;
	case 5:
		bp = (basic*)_num5_p;
		++bp->refcount;
		break;
	case 6:
		bp = (basic*)_num6_p;
		++bp->refcount;
		break;
	case 7:
		bp = (basic*)_num7_p;
		++bp->refcount;
		break;
	case 8:
		bp = (basic*)_num8_p;
		++bp->refcount;
		break;
	case 9:
		bp = (basic*)_num9_p;
		++bp->refcount;
		break;
	case 10:
		bp = (basic*)_num10_p;
		++bp->refcount;
		break;
	case 11:
		bp = (basic*)_num11_p;
		++bp->refcount;
		break;
	case 12:
		bp = (basic*)_num12_p;
		++bp->refcount;
		break;
	default:
		bp = new numeric(i);
		bp->setflag(status_flags::dynallocated);
		++bp->refcount;
		GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
		GINAC_ASSERT(bp->refcount==1);
	}
}
	
void ex::construct_from_long(long i)
{
	switch (i) {  // prefer flyweights over new objects
	case -12:
		bp = (basic*)_num_12_p;
		++bp->refcount;
		break;
	case -11:
		bp = (basic*)_num_11_p;
		++bp->refcount;
		break;
	case -10:
		bp = (basic*)_num_10_p;
		++bp->refcount;
		break;
	case -9:
		bp = (basic*)_num_9_p;
		++bp->refcount;
		break;
	case -8:
		bp = (basic*)_num_8_p;
		++bp->refcount;
		break;
	case -7:
		bp = (basic*)_num_7_p;
		++bp->refcount;
		break;
	case -6:
		bp = (basic*)_num_6_p;
		++bp->refcount;
		break;
	case -5:
		bp = (basic*)_num_5_p;
		++bp->refcount;
		break;
	case -4:
		bp = (basic*)_num_4_p;
		++bp->refcount;
		break;
	case -3:
		bp = (basic*)_num_3_p;
		++bp->refcount;
		break;
	case -2:
		bp = (basic*)_num_2_p;
		++bp->refcount;
		break;
	case -1:
		bp = (basic*)_num_1_p;
		++bp->refcount;
		break;
	case 0:
		bp = (basic*)_num0_p;
		++bp->refcount;
		break;
	case 1:
		bp = (basic*)_num1_p;
		++bp->refcount;
		break;
	case 2:
		bp = (basic*)_num2_p;
		++bp->refcount;
		break;
	case 3:
		bp = (basic*)_num3_p;
		++bp->refcount;
		break;
	case 4:
		bp = (basic*)_num4_p;
		++bp->refcount;
		break;
	case 5:
		bp = (basic*)_num5_p;
		++bp->refcount;
		break;
	case 6:
		bp = (basic*)_num6_p;
		++bp->refcount;
		break;
	case 7:
		bp = (basic*)_num7_p;
		++bp->refcount;
		break;
	case 8:
		bp = (basic*)_num8_p;
		++bp->refcount;
		break;
	case 9:
		bp = (basic*)_num9_p;
		++bp->refcount;
		break;
	case 10:
		bp = (basic*)_num10_p;
		++bp->refcount;
		break;
	case 11:
		bp = (basic*)_num11_p;
		++bp->refcount;
		break;
	case 12:
		bp = (basic*)_num12_p;
		++bp->refcount;
		break;
	default:
		bp = new numeric(i);
		bp->setflag(status_flags::dynallocated);
		++bp->refcount;
		GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
		GINAC_ASSERT(bp->refcount==1);
	}
}
	
void ex::construct_from_ulong(unsigned long i)
{
	switch (i) {  // prefer flyweights over new objects
	case 0:
		bp = (basic*)_num0_p;
		++bp->refcount;
		break;
	case 1:
		bp = (basic*)_num1_p;
		++bp->refcount;
		break;
	case 2:
		bp = (basic*)_num2_p;
		++bp->refcount;
		break;
	case 3:
		bp = (basic*)_num3_p;
		++bp->refcount;
		break;
	case 4:
		bp = (basic*)_num4_p;
		++bp->refcount;
		break;
	case 5:
		bp = (basic*)_num5_p;
		++bp->refcount;
		break;
	case 6:
		bp = (basic*)_num6_p;
		++bp->refcount;
		break;
	case 7:
		bp = (basic*)_num7_p;
		++bp->refcount;
		break;
	case 8:
		bp = (basic*)_num8_p;
		++bp->refcount;
		break;
	case 9:
		bp = (basic*)_num9_p;
		++bp->refcount;
		break;
	case 10:
		bp = (basic*)_num10_p;
		++bp->refcount;
		break;
	case 11:
		bp = (basic*)_num11_p;
		++bp->refcount;
		break;
	case 12:
		bp = (basic*)_num12_p;
		++bp->refcount;
		break;
	default:
		bp = new numeric(i);
		bp->setflag(status_flags::dynallocated);
		++bp->refcount;
		GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
		GINAC_ASSERT(bp->refcount==1);
	}
}
	
void ex::construct_from_double(double d)
{
	bp = new numeric(d);
	bp->setflag(status_flags::dynallocated);
	++bp->refcount;
	GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
	GINAC_ASSERT(bp->refcount==1);
}

void ex::construct_from_string_and_lst(const std::string &s, const ex &l)
{
	set_lexer_string(s);
	set_lexer_symbols(l);
	ginac_yyrestart(NULL);
	if (ginac_yyparse())
		throw (std::runtime_error(get_parser_error()));
	else {
		bp = parsed_ex.bp;
		GINAC_ASSERT(bp!=0);
		GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
		++bp->refcount;
	}
}
	
//////////
// static member variables
//////////

// none

//////////
// functions which are not member functions
//////////

// none

//////////
// global functions
//////////

// none


} // namespace GiNaC
