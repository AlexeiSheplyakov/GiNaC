/** @file relational.cpp
 *
 *  Implementation of relations between expressions */

/*
 *  GiNaC Copyright (C) 1999-2002 Johannes Gutenberg University Mainz, Germany
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

#include "relational.h"
#include "numeric.h"
#include "print.h"
#include "archive.h"
#include "utils.h"

namespace GiNaC {

GINAC_IMPLEMENT_REGISTERED_CLASS(relational, basic)

//////////
// default ctor, dtor, copy ctor, assignment operator and helpers
//////////

relational::relational() : basic(TINFO_relational) {}

void relational::copy(const relational & other)
{
	basic::copy(other);
	lh=other.lh;
	rh=other.rh;
	o=other.o;
}

DEFAULT_DESTROY(relational)

//////////
// other ctors
//////////

// public

relational::relational(const ex & lhs, const ex & rhs, operators oper) : basic(TINFO_relational), lh(lhs), rh(rhs), o(oper) {}

//////////
// archiving
//////////

relational::relational(const archive_node &n, const lst &sym_lst) : inherited(n, sym_lst)
{
	unsigned int opi;
	if (!(n.find_unsigned("op", opi)))
		throw (std::runtime_error("unknown relational operator in archive"));
	o = (operators)opi;
	n.find_ex("lh", lh, sym_lst);
	n.find_ex("rh", rh, sym_lst);
}

void relational::archive(archive_node &n) const
{
	inherited::archive(n);
	n.add_ex("lh", lh);
	n.add_ex("rh", rh);
	n.add_unsigned("op", o);
}

DEFAULT_UNARCHIVE(relational)

//////////
// functions overriding virtual functions from base classes
//////////

// public

void relational::print(const print_context & c, unsigned level) const
{
	if (is_a<print_tree>(c)) {

		inherited::print(c, level);

	} else {

		if (is_a<print_python_repr>(c)) {
			c.s << class_name() << '(';
			lh.print(c);
			c.s << ',';
			rh.print(c);
			c.s << ",'";
		} else {
			if (precedence() <= level)
				c.s << "(";
			lh.print(c, precedence());
		}
		switch (o) {
			case equal:
				c.s << "==";
				break;
			case not_equal:
				c.s << "!=";
				break;
			case less:
				c.s << "<";
				break;
			case less_or_equal:
				c.s << "<=";
				break;
			case greater:
				c.s << ">";
				break;
			case greater_or_equal:
				c.s << ">=";
				break;
			default:
				c.s << "(INVALID RELATIONAL OPERATOR)";
		}
		if (is_a<print_python_repr>(c))
			c.s << "')";
		else {
			rh.print(c, precedence());
			if (precedence() <= level)
				c.s << ")";
		}
	}
}

bool relational::info(unsigned inf) const
{
	switch (inf) {
		case info_flags::relation:
			return 1;
		case info_flags::relation_equal:
			return o==equal;
		case info_flags::relation_not_equal:
			return o==not_equal;
		case info_flags::relation_less:
			return o==less;
		case info_flags::relation_less_or_equal:
			return o==less_or_equal;
		case info_flags::relation_greater:
			return o==greater;
		case info_flags::relation_greater_or_equal:
			return o==greater_or_equal;
	}
	return 0;
}

unsigned relational::nops() const
{
	return 2;
}

ex & relational::let_op(int i)
{
	GINAC_ASSERT(i>=0);
	GINAC_ASSERT(i<2);

	return i==0 ? lh : rh;
}

ex relational::eval(int level) const
{
	if (level==1)
		return this->hold();
	
	if (level == -max_recursion_level)
		throw(std::runtime_error("max recursion level reached"));
	
	return (new relational(lh.eval(level-1),rh.eval(level-1),o))->setflag(status_flags::dynallocated | status_flags::evaluated);
}

ex relational::subs(const lst & ls, const lst & lr, bool no_pattern) const
{
	const ex & subsed_lh = lh.subs(ls, lr, no_pattern);
	const ex & subsed_rh = rh.subs(ls, lr, no_pattern);

	if (!are_ex_trivially_equal(lh, subsed_lh) || !are_ex_trivially_equal(rh, subsed_rh))
		return relational(subsed_lh, subsed_rh, o).basic::subs(ls, lr, no_pattern);
	else
		return basic::subs(ls, lr, no_pattern);
}

ex relational::simplify_ncmul(const exvector & v) const
{
	return lh.simplify_ncmul(v);
}

// protected

int relational::compare_same_type(const basic & other) const
{
	GINAC_ASSERT(is_exactly_a<relational>(other));
	const relational &oth = static_cast<const relational &>(other);
	if (o==oth.o && lh.is_equal(oth.lh) && rh.is_equal(oth.rh))
		return 0;
	switch (o) {
		case equal:
		case not_equal:
			if (oth.o!=o)
				return (o < oth.o) ? -1 : 1;
			break;
		case less:
			if (oth.o!=greater)
				return (o < oth.o) ? -1 : 1;
			break;
		case less_or_equal:
			if (oth.o!=greater_or_equal)
				return (o < oth.o) ? -1 : 1;
			break;
		case greater:
			if (oth.o!=less)
				return (o < oth.o) ? -1 : 1;
			break;
		case greater_or_equal:
			if (oth.o!=less_or_equal)
				return (o < oth.o) ? -1 : 1;
			break;
	}
	const int lcmpval = lh.compare(oth.rh);
	return (lcmpval!=0) ? lcmpval : rh.compare(oth.lh);
}

bool relational::match_same_type(const basic & other) const
{
	GINAC_ASSERT(is_exactly_a<relational>(other));
	const relational &oth = static_cast<const relational &>(other);

	return o == oth.o;
}

unsigned relational::return_type(void) const
{
	GINAC_ASSERT(lh.return_type()==rh.return_type());
	return lh.return_type();
}
   
unsigned relational::return_type_tinfo(void) const
{
	GINAC_ASSERT(lh.return_type_tinfo()==rh.return_type_tinfo());
	return lh.return_type_tinfo();
}

unsigned relational::calchash(void) const
{
	unsigned v = golden_ratio_hash(tinfo());
	unsigned lhash = lh.gethash();
	unsigned rhash = rh.gethash();

	v = rotate_left_31(v);
	switch(o) {
		case equal:
		case not_equal:
			if (lhash>rhash) {
				v ^= lhash;
				lhash = rhash;
			} else {
				v ^= rhash;
			}
			break;
		case less:
		case less_or_equal:
			v ^= rhash;
			break;
		case greater:
		case greater_or_equal:
	   		v ^= lhash;
			lhash = rhash;
			break;
	}
	v = rotate_left_31(v);
	v ^= lhash;

	// mask out numeric hashes:
	v &= 0x7FFFFFFFU;

	// store calculated hash value only if object is already evaluated
	if (flags & status_flags::evaluated) {
		setflag(status_flags::hash_calculated);
		hashvalue = v;
	}

	return v;
}

//////////
// new virtual functions which can be overridden by derived classes
//////////

/** Left hand side of relational. */
ex relational::lhs(void) const
{
	return lh;
}

/** Right hand side of relational. */
ex relational::rhs(void) const
{
	return rh;    
}

//////////
// non-virtual functions in this class
//////////

/** Cast the relational into a boolean, mainly for evaluation within an 
 *  if-statement.  Note that (a<b) == false does not imply (a>=b) == true in
 *  the general symbolic case.  A false result means the comparison is either
 *  false or undecidable (except of course for !=, where true means either
 *  unequal or undecidable). */
relational::operator bool() const
{
	const ex df = lh-rh;
	if (!is_ex_exactly_of_type(df,numeric))
		// cannot decide on non-numerical results
		return o==not_equal ? true : false;
	
	switch (o) {
		case equal:
			return ex_to<numeric>(df).is_zero();
		case not_equal:
			return !ex_to<numeric>(df).is_zero();
		case less:
			return ex_to<numeric>(df)<_num0;
		case less_or_equal:
			return ex_to<numeric>(df)<=_num0;
		case greater:
			return ex_to<numeric>(df)>_num0;
		case greater_or_equal:
			return ex_to<numeric>(df)>=_num0;
		default:
			throw(std::logic_error("invalid relational operator"));
	}
}

} // namespace GiNaC
