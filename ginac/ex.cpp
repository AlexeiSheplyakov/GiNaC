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
#include "relational.h"
#include "indexed.h"
#include "lst.h"
#include "input_lexer.h"
#include "debugmsg.h"
#include "utils.h"

namespace GiNaC {

//////////
// other ctors
//////////

// none (all inlined)

//////////
// functions overriding virtual functions from bases classes
//////////

// none

//////////
// new virtual functions which can be overridden by derived classes
//////////

// none

//////////
// non-virtual functions in this class
//////////

// public

/** Efficiently swap the contents of two expressions. */
void ex::swap(ex & other)
{
	debugmsg("ex swap",LOGLEVEL_MEMBER_FUNCTION);

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
	debugmsg("ex print", LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	bp->print(c, level);
}

/** Print expression to stream in a tree-like format suitable for debugging. */
void ex::printtree(std::ostream & os) const
{
	debugmsg("ex printtree", LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	bp->print(print_tree(os));
}

/** Little wrapper arount print to be called within a debugger. */
void ex::dbgprint(void) const
{
	debugmsg("ex dbgprint", LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	bp->dbgprint();
}

/** Little wrapper arount printtree to be called within a debugger. */
void ex::dbgprinttree(void) const
{
	debugmsg("ex dbgprinttree", LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	bp->dbgprinttree();
}

ex ex::expand(unsigned options) const
{
	GINAC_ASSERT(bp!=0);
	if (bp->flags & status_flags::expanded)
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

/** Simplify/canonicalize expression containing indexed objects. This
 *  performs contraction of dummy indices where possible and checks whether
 *  the free indices in sums are consistent.
 *
 *  @return simplified expression */
ex ex::simplify_indexed(void) const
{
	return GiNaC::simplify_indexed(*this);
}

/** Simplify/canonicalize expression containing indexed objects. This
 *  performs contraction of dummy indices where possible, checks whether
 *  the free indices in sums are consistent, and automatically replaces
 *  scalar products by known values if desired.
 *
 *  @param sp Scalar products to be replaced automatically
 *  @return simplified expression */
ex ex::simplify_indexed(const scalar_products & sp) const
{
	return GiNaC::simplify_indexed(*this, sp);
}

ex ex::operator[](const ex & index) const
{
	debugmsg("ex operator[ex]",LOGLEVEL_OPERATOR);
	GINAC_ASSERT(bp!=0);
	return (*bp)[index];
}

ex ex::operator[](int i) const
{
	debugmsg("ex operator[int]",LOGLEVEL_OPERATOR);
	GINAC_ASSERT(bp!=0);
	return (*bp)[i];
}

/** Return modifyable operand/member at position i. */
ex & ex::let_op(int i)
{
	debugmsg("ex let_op()",LOGLEVEL_MEMBER_FUNCTION);
	makewriteable();
	GINAC_ASSERT(bp!=0);
	return bp->let_op(i);
}

/** Left hand side of relational expression. */
ex ex::lhs(void) const
{
	debugmsg("ex lhs()",LOGLEVEL_MEMBER_FUNCTION);
	if (!is_ex_of_type(*this,relational))
		throw std::runtime_error("ex::lhs(): not a relation");
	return (*static_cast<relational *>(bp)).lhs();
}

/** Right hand side of relational expression. */
ex ex::rhs(void) const
{
	debugmsg("ex rhs()",LOGLEVEL_MEMBER_FUNCTION);
	if (!is_ex_of_type(*this,relational))
		throw std::runtime_error("ex::rhs(): not a relation");
	return (*static_cast<relational *>(bp)).rhs();
}

// private

/** Make this ex writable (if more than one ex handle the same basic) by 
 *  unlinking the object and creating an unshared copy of it. */
void ex::makewriteable()
{
	debugmsg("ex makewriteable",LOGLEVEL_MEMBER_FUNCTION);
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
	if ((other.flags & status_flags::evaluated)==0) {
		// cf. copy ctor
		const ex & tmpex = other.eval(1); // evaluate only one (top) level
		bp = tmpex.bp;
		GINAC_ASSERT(bp!=0);
		GINAC_ASSERT(bp->flags & status_flags::dynallocated);
		++bp->refcount;
		if ((other.flags & status_flags::dynallocated)&&(other.refcount==0))
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
	switch (i) {  // some tiny efficiency-hack
	case -2:
		bp = _ex_2().bp;
		++bp->refcount;
		break;
	case -1:
		bp = _ex_1().bp;
		++bp->refcount;
		break;
	case 0:
		bp = _ex0().bp;
		++bp->refcount;
		break;
	case 1:
		bp = _ex1().bp;
		++bp->refcount;
		break;
	case 2:
		bp = _ex2().bp;
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
	switch (i) {  // some tiny efficiency-hack
	case 0:
		bp = _ex0().bp;
		++bp->refcount;
		break;
	case 1:
		bp = _ex1().bp;
		++bp->refcount;
		break;
	case 2:
		bp = _ex2().bp;
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
	switch (i) {  // some tiny efficiency-hack
	case -2:
		bp = _ex_2().bp;
		++bp->refcount;
		break;
	case -1:
		bp = _ex_1().bp;
		++bp->refcount;
		break;
	case 0:
		bp = _ex0().bp;
		++bp->refcount;
		break;
	case 1:
		bp = _ex1().bp;
		++bp->refcount;
		break;
	case 2:
		bp = _ex2().bp;
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
	switch (i) {  // some tiny efficiency-hack
	case 0:
		bp = _ex0().bp;
		++bp->refcount;
		break;
	case 1:
		bp = _ex1().bp;
		++bp->refcount;
		break;
	case 2:
		bp = _ex2().bp;
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
