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

#if defined(VERBOSE)
#  define GINAC_CONDITIONAL_INLINE
#  include "ex.h"
#  undef GINAC_CONDITIONAL_INLINE
#else
#  include "ex.h"
#endif

#include "add.h"
#include "mul.h"
#include "ncmul.h"
#include "numeric.h"
#include "power.h"
#include "relational.h"
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

/** Output formatted to be useful as ginsh input. */
void ex::print(std::ostream & os, unsigned upper_precedence) const
{
	debugmsg("ex print",LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	bp->print(os,upper_precedence);
}

/** Unreadable output with detailed type information. */
void ex::printraw(std::ostream & os) const
{
	debugmsg("ex printraw",LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	os << "ex(";
	bp->printraw(os);
	os << ")";
}

/** Very detailed and unreadable output with type information and all this. */
void ex::printtree(std::ostream & os, unsigned indent) const
{
	debugmsg("ex printtree",LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	// os << "refcount=" << bp->refcount << " ";
	bp->printtree(os,indent);
}

/** Print expression as a C++ statement. The output looks like
 *  "<type> <var_name> = <expression>;". The "type" parameter has an effect
 *  on how number literals are printed.
 *
 *  @param os output stream
 *  @param type variable type (one of the csrc_types)
 *  @param var_name variable name to be printed */
void ex::printcsrc(std::ostream & os, unsigned type, const char *var_name) const
{
	debugmsg("ex print csrc", LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	switch (type) {
		case csrc_types::ctype_float:
			os << "float ";
			break;
		case csrc_types::ctype_double:
			os << "double ";
			break;
		case csrc_types::ctype_cl_N:
			os << "cl_N ";
			break;
	}
	os << var_name << " = ";
	bp->printcsrc(os, type, 0);
	os << ";\n";
}

/** Little wrapper arount print to be called within a debugger. */
void ex::dbgprint(void) const
{
	debugmsg("ex dbgprint",LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	bp->dbgprint();
}

/** Little wrapper arount printtree to be called within a debugger. */
void ex::dbgprinttree(void) const
{
	debugmsg("ex dbgprinttree",LOGLEVEL_PRINT);
	GINAC_ASSERT(bp!=0);
	bp->dbgprinttree();
}

bool ex::info(unsigned inf) const
{
	return bp->info(inf);
}

unsigned ex::nops() const
{
	GINAC_ASSERT(bp!=0);
	return bp->nops();
}

ex ex::expand(unsigned options) const
{
	GINAC_ASSERT(bp!=0);
	if (bp->flags & status_flags::expanded)
		return *bp;
	else
		return bp->expand(options);
}

bool ex::has(const ex & other) const
{
	GINAC_ASSERT(bp!=0);
	return bp->has(other);
}

int ex::degree(const symbol & s) const
{
	GINAC_ASSERT(bp!=0);
	return bp->degree(s);
}

int ex::ldegree(const symbol & s) const
{
	GINAC_ASSERT(bp!=0);
	return bp->ldegree(s);
}

ex ex::coeff(const symbol & s, int n) const
{
	GINAC_ASSERT(bp!=0);
	return bp->coeff(s,n);
}

ex ex::collect(const symbol & s) const
{
	GINAC_ASSERT(bp!=0);
	return bp->collect(s);
}

ex ex::eval(int level) const
{
	GINAC_ASSERT(bp!=0);
	return bp->eval(level);
}

ex ex::evalf(int level) const
{
	GINAC_ASSERT(bp!=0);
	return bp->evalf(level);
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

ex ex::subs(const lst & ls, const lst & lr) const
{
	GINAC_ASSERT(bp!=0);
	return bp->subs(ls,lr);
}

ex ex::subs(const ex & e) const
{
	GINAC_ASSERT(bp!=0);
	return bp->subs(e);
}

exvector ex::get_indices(void) const
{
	GINAC_ASSERT(bp!=0);
	return bp->get_indices();
}

ex ex::simplify_ncmul(const exvector & v) const
{
	GINAC_ASSERT(bp!=0);
	return bp->simplify_ncmul(v);
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

/** Return operand/member at position i. */
ex ex::op(int i) const
{
	debugmsg("ex op()",LOGLEVEL_MEMBER_FUNCTION);
	GINAC_ASSERT(bp!=0);
	return bp->op(i);
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
	GINAC_ASSERT(is_ex_of_type(*this,relational));
	return (*static_cast<relational *>(bp)).lhs();
}

/** Right hand side of relational expression. */
ex ex::rhs(void) const
{
	debugmsg("ex rhs()",LOGLEVEL_MEMBER_FUNCTION);
	GINAC_ASSERT(is_ex_of_type(*this,relational));
	return (*static_cast<relational *>(bp)).rhs();
}

unsigned ex::return_type(void) const
{
	GINAC_ASSERT(bp!=0);
	return bp->return_type();
}

unsigned ex::return_type_tinfo(void) const
{
	GINAC_ASSERT(bp!=0);
	return bp->return_type_tinfo();
}

unsigned ex::gethash(void) const
{
	GINAC_ASSERT(bp!=0);
	return bp->gethash();
}

ex ex::exadd(const ex & rh) const
{
	return (new add(*this,rh))->setflag(status_flags::dynallocated);
}

ex ex::exmul(const ex & rh) const
{
	return (new mul(*this,rh))->setflag(status_flags::dynallocated);
}

ex ex::exncmul(const ex & rh) const
{
	return (new ncmul(*this,rh))->setflag(status_flags::dynallocated);
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
		// bp->clearflag(status_flags::evaluated);
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
